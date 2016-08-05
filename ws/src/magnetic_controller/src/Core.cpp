#include "magnetic_controller/Core.h"

#include "Mesh.h"
#include "ObjectBase.h"

#include <visualization_msgs/MarkerArray.h>

#include <iostream>
#include <cmath>

using namespace std;

typedef ObjectBase<Mesh> TMeshObject;
typedef ObjectBase<ParticleCloud> TParticleObject;

template<class T>
MagneticCore<T>::MagneticCore(const NodeHandle& _nh)
: nh(_nh)
, bDrawDebug(true)
 {
 	namespaces.push_back("current");
 	namespaces.push_back("math");

	visPub = nh.advertise<visualization_msgs::MarkerArray>("/magnetic_core/visualization", 10);

	for (int i = 0; i < eMAX; i++) {
		visManager.addNamespace(i, namespaces[i]);
	}
}

MagneticCore_ParticleModel::MagneticCore_ParticleModel(const NodeHandle& _nh) 
: MagneticCore<ParticleCloud>(_nh)
{}

MagneticCore_SurfaceModel::MagneticCore_SurfaceModel(const NodeHandle& _nh)
: MagneticCore<Mesh>(_nh)
{}

Vector3d MagneticCore_ParticleModel::calculateAccel(Vector3d pos, Vector3d goalPos, Vector3d velV) {
	visualization_msgs::MarkerArray vis;
	if (bDrawDebug)
		visManager.beginNewDrawCycle();
	//visManager.clearAllMarkers(&visPub);

	double vel = parameters.vel;
	Vector3d b = goalPos - pos;
	double b_mag = b.norm();
	Vector3d velDir = velV.normalized();//b.normalized();
	//Vector3d velV = velDir * vel;

	//visManager.annotatedVector(vis.markers, eMath, pos, b, "b");

	Vector3d F_v = Vector3d::Zero();// Virtual obstacle force
	double mass = parameters.mass;
	double I_k = parameters.I_k; // Virtual current factor

	double k_d = parameters.k_d;// Dampening constant
	double k_a = (k_d * k_d) / (4 * mass);// Attractive constant

	Vector3d goalDiff = -b;

	// cout << "pos(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << endl
	// 	 << "goalPos(" << goalPos[0] << ", " << goalPos[1] << ", " << goalPos[2] << ")" << endl
	// 	 << "b(" << b[0] << ", " << b[1] << ", " << b[2] << ")" << endl
	// 	 << "b_mag = " << b_mag << endl
	// 	 << "velDir(" << velDir[0] << ", " << velDir[1] << ", " << velDir[2] << ")" << endl
	// 	 << "velV(" << velV[0] << ", " << velV[1] << ", " << velV[2] << ")" << endl;

	Vector3d B_superSum = Vector3d::Zero();
	for (TParticleObject* pObj: objects) {
		// PER OBJECT
		Affine3d objPose = pObj->getTransform();
		Vector3d obj_com = objPose.translation();
		Matrix3d rotM = objPose.rotation();

		// cout << "objPose(" << objPose.matrix() << ")" << endl
		// 	 << "obj_com(" << obj_com[0] << ", " << obj_com[1] << ", " << obj_com[2] << ")" << endl
		// 	 << "rotM(" << rotM << ")" << endl;

		Vector3d d_j = pos + b * ((obj_com - pos).dot(b) / (b_mag * b_mag)) - obj_com;

		Vector3d incidentDirection = (obj_com - pos).normalized();
		Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;
		
		Vector3d rotV = d_j.cross(b).normalized();
		
		if (bDrawDebug) {
			visManager.annotatedVector(vis.markers, eMath, pos, rotV, "rotV");
			visManager.annotatedVector(vis.markers, eMath, obj_com, d_j, "d_j");
			visManager.annotatedVector(vis.markers, eMath, pos, b, "b");
		}
		
		for (size_t i = 0; i < pObj->pMesh->particles.size(); i++) {
			SParticle* p = &(pObj->pMesh->particles[i]);
			Vector3d paPos = objPose * p->position;
			Vector3d paN = rotM * p->normal;
			Vector3d distSP = (paPos - pos);

			Vector3d incidentDirection = (paPos- pos).normalized();
			//cout << "paPos(" << paPos[0] << ", " << paPos[1] << ", " << paPos[2] << ")" << endl
				 // << "sprNormal(" << sprNormal[0] << ", " << sprNormal[1] << ", " << sprNormal[2] << ")" << endl
				 // << "sprCurrent(" << sprCurrent[0] << ", " << sprCurrent[1] << ", " << sprCurrent[2] << ")" << endl;
			//B_sum += parameters.I_k / l_i_j * Vector3d(0,0,1);
			//B_sum += (sprCurrent.cross(incidentDirection)) * (parameters.I_k / (l_i_j / 0.2)) * sprArea; 
			
			//if (incidentDirection.dot(distSP.normalized()) > 0) {
				Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;

				double l_i_j = distSP.squaredNorm();
				Vector3d B = parameters.I_k / l_i_j * p->size * paN.cross(rotV);
				F_v += incidentVelocity.cross(B);
				B_superSum += B;

				if (bDrawDebug) {
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paPos, incidentDirection, 1,1,0));
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, incidentVelocity, 1,0,1));				
				}
			//}
		}//*/

		//F_v += incidentVelocity.cross(B_sum);
		// cout << "B_sum(" << B_sum[0] << ", " << B_sum[1] << ", " << B_sum[2] << ")" << endl;
	}

	// Allow some introspection for debugging
	debug.F_v = F_v;
	debug.B = B_superSum.norm() / I_k;

	// point mass dynamic system
	Vector3d accel = (-k_a * goalDiff + F_v - k_d * velV) / mass; 
	double U_a = 0.5 * k_a * (goalDiff.dot(goalDiff));

	if (bDrawDebug) {
		visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
		vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, B_superSum, 1,0,0));

		visManager.endDrawCycle(vis.markers);
		visPub.publish(vis);
	}

	return accel;// * U_a;
}

Vector3d MagneticCore_SurfaceModel::calculateAccel(Vector3d pos, Vector3d goalPos, Vector3d velV) {
	visualization_msgs::MarkerArray vis;
	if (bDrawDebug)
		visManager.beginNewDrawCycle();
	//visManager.clearAllMarkers(&visPub);

	double vel = parameters.vel;
	Vector3d b = goalPos - pos;
	double b_mag = b.norm();
	Vector3d velDir = velV.normalized();//b.normalized();
	//Vector3d velV = velDir * vel;

	//visManager.annotatedVector(vis.markers, eMath, pos, b, "b");

	Vector3d F_v = Vector3d::Zero();// Virtual obstacle force
	double mass = parameters.mass;
	double I_k = parameters.I_k; // Virtual current factor

	double k_d = parameters.k_d;// Dampening constant
	double k_a = (k_d * k_d) / (4 * mass);// Attractive constant

	Vector3d goalDiff = -b;

	// cout << "pos(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << endl
	// 	 << "goalPos(" << goalPos[0] << ", " << goalPos[1] << ", " << goalPos[2] << ")" << endl
	// 	 << "b(" << b[0] << ", " << b[1] << ", " << b[2] << ")" << endl
	// 	 << "b_mag = " << b_mag << endl
	// 	 << "velDir(" << velDir[0] << ", " << velDir[1] << ", " << velDir[2] << ")" << endl
	// 	 << "velV(" << velV[0] << ", " << velV[1] << ", " << velV[2] << ")" << endl;

	Vector3d B_superSum = Vector3d::Zero();
	for (TMeshObject* pObj: objects) {
		// PER OBJECT
		Affine3d objPose = pObj->getTransform();
		Vector3d obj_com = objPose.translation();
		Matrix3d rotM = objPose.rotation();

		// cout << "objPose(" << objPose.matrix() << ")" << endl
		// 	 << "obj_com(" << obj_com[0] << ", " << obj_com[1] << ", " << obj_com[2] << ")" << endl
		// 	 << "rotM(" << rotM << ")" << endl;

		Vector3d d_j = pos + b * ((obj_com - pos).dot(b) / (b_mag * b_mag)) - obj_com;

		Vector3d incidentDirection = (obj_com - pos).normalized();
		Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;
		
		Vector3d rotV = d_j.cross(b).normalized();
		
		//visManager.annotatedVector(vis.markers, eMath, obj_com, d_j, "d_j");
		if (bDrawDebug)
			visManager.annotatedVector(vis.markers, eMath, pos, rotV, "rotV");

		// Prevent Batman
		//if (d_j == Vector3d::Zero())
		//	rotV = Vector3d(0,0,1);
			
		Vector3d obj2Pos = (pos - obj_com);
		double objDist = max(obj2Pos.norm() - pObj->pMesh->dimensions.norm() * 0.5, 0.0);
		Vector3d B_sum = Vector3d(0,0, parameters.I_k /(1 + (objDist * objDist / 0.02)));

		//cout << "d_j(" << d_j[0] << ", " << d_j[1] << ", " << d_j[2] << ")" << endl
		// 	 << "rotV(" << rotV[0] << ", " << rotV[1] << ", " << rotV[2] << ")" << endl;
		double fieldScale = parameters.I_k / pObj->pMesh->surfaces.size();
		
		for (size_t i = 0; i < pObj->pMesh->surfaces.size(); i++) {
			Surface* s = &(pObj->pMesh->surfaces[i]);
			// PER SURFACE
			Vector3d surfMid = objPose * s->mid;
			Vector3d surfNormal = rotM * s->normal;
			Vector3d distSP = (surfMid - pos);

			Vector3d incidentDirection = (obj_com - pos).normalized();

			//if ( surfNormal.dot(-distSP) >= 0) 
			{
				Vector3d surfCurrent = rotV.cross(surfNormal);
				//cout << "surfMid(" << surfMid[0] << ", " << surfMid[1] << ", " << surfMid[2] << ")" << endl
					 // << "surfNormal(" << surfNormal[0] << ", " << surfNormal[1] << ", " << surfNormal[2] << ")" << endl
					 // << "surfCurrent(" << surfCurrent[0] << ", " << surfCurrent[1] << ", " << surfCurrent[2] << ")" << endl;
				double l_i_j = distSP.squaredNorm();
				//B_sum += parameters.I_k / l_i_j * Vector3d(0,0,1);
				//B_sum += (surfCurrent.cross(incidentDirection)) * (parameters.I_k / (l_i_j / 0.2)) * surfArea; 
				

				Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;

				if (s->area < 0)
					cerr << "NEGATIVE SURFACE AREA! WTF?! " << s->area << endl;	 
				Vector3d B = fieldScale / l_i_j * s->area * rotV;
				F_v += velV.cross(B);
				B_sum += B;

				if (bDrawDebug) {
					vis.markers.push_back(visManager.vectorMarker(eCurrent, surfMid, surfCurrent, 1,1,0));
					vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, incidentVelocity, 1,0,1));				
				}
			}
		}//*/

		// cout << "B_sum(" << B_sum[0] << ", " << B_sum[1] << ", " << B_sum[2] << ")" << endl;

		B_superSum += B_sum;
	}

	// Allow some introspection for debugging
	debug.F_v = F_v;
	debug.B = B_superSum.norm() / I_k;

	// point mass dynamic system
	Vector3d accel = (-k_a * goalDiff + F_v - k_d * velV) / mass; 
	double U_a = 0.5 * k_a * (goalDiff.dot(goalDiff));

	if (bDrawDebug) {
		visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
		vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, B_superSum, 1,0,0));
	}
	/*Vector3d newVel = velV + accel * U_a * dT; //(velV + accel).normalized() * 0.2; // RETURN THIS
	// ROTATION MISSING


	cout << "F_v(" << F_v[0] << ", " << F_v[1] << ", " << F_v[2] << ")" << endl
		 << "accel(" << accel[0] << ", " << accel[1] << ", " << accel[2] << ")" << endl
		 << "newVel(" << newVel[0] << ", " << newVel[1] << ", " << newVel[2] << ")" << endl;
	//*/

	if (bDrawDebug) {
		visManager.endDrawCycle(vis.markers);
		visPub.publish(vis);
	}

	return accel;// * U_a;
}
