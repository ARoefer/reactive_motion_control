#include "magnetic_controller/Core.h"

#include "Mesh.h"
#include "ObjectBase.h"

#include <visualization_msgs/MarkerArray.h>

#include <iostream>

using namespace std;

MagneticCore::MagneticCore(const NodeHandle& _nh)
: nh(_nh)
 {
	visPub = nh.advertise<visualization_msgs::MarkerArray>("/magnetic_core/visualization", 10);

	for (int i = 0; i < eMAX; i++) {
		visManager.addNamespace(i, namespaces[i]);
	}
}

Vector3d MagneticCore::calculateAccel(Vector3d pos, Vector3d goalPos, Vector3d velV) {
	//visualization_msgs::MarkerArray vis;
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
	double k_a = parameters.k_a; //(k_d * k_d) / (4 * mass);// Attractive constant

	Vector3d goalDiff = -b;

	cout << "pos(" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << endl
		 << "goalPos(" << goalPos[0] << ", " << goalPos[1] << ", " << goalPos[2] << ")" << endl
		 << "b(" << b[0] << ", " << b[1] << ", " << b[2] << ")" << endl
		 << "b_mag = " << b_mag << endl
		 << "velDir(" << velDir[0] << ", " << velDir[1] << ", " << velDir[2] << ")" << endl
		 << "velV(" << velV[0] << ", " << velV[1] << ", " << velV[2] << ")" << endl;

	Vector3d B_superSum = Vector3d::Zero();
	for (ObjectBase* pObj: objects) {
		// PER OBJECT
		Affine3d objPose = pObj->getTransform();
		Vector3d obj_com = objPose.translation();
		Matrix3d rotM = objPose.rotation();

		cout << "objPose(" << objPose.matrix() << ")" << endl
			 << "obj_com(" << obj_com[0] << ", " << obj_com[1] << ", " << obj_com[2] << ")" << endl
			 << "rotM(" << rotM << ")" << endl;

		Vector3d d_j = pos + b * ((obj_com - pos).dot(b) / (b_mag * b_mag)) - obj_com;
		
		Vector3d rotV = d_j.cross(b).normalized();
		
		//visManager.annotatedVector(vis.markers, eMath, obj_com, d_j, "d_j");
		//visManager.annotatedVector(vis.markers, eMath, pos, rotV, "rotV");

		// Prevent Batman
		if (d_j == Vector3d::Zero())
			rotV = Vector3d(0,0,1);
			
		Vector3d B_sum = Vector3d(0, 0, parameters.I_k /(1 + ((pos - obj_com).squaredNorm() / 0.25)));

		cout << "d_j(" << d_j[0] << ", " << d_j[1] << ", " << d_j[2] << ")" << endl
			 << "rotV(" << rotV[0] << ", " << rotV[1] << ", " << rotV[2] << ")" << endl;
		/*for (size_t i = 0; i < pObj->pMesh->surfaces.size(); i++) {
			Surface* s = &(pObj->pMesh->surfaces[i]);
			// PER SURFACE
			Vector3d surfMid = objPose * s->mid;
			Vector3d surfNormal = rotM * s->normal;
			Vector3d surfCurrent = surfNormal.cross(rotV);
			vis.markers.push_back(visManager.vectorMarker(eCurrent, surfMid, surfCurrent, 1,1,0));
			double l_i_j = (pos - surfMid).norm();
			double surfArea = 1;
			B_sum += I_k * (surfNormal.cross(surfCurrent).cross(velDir) / (l_i_j * l_i_j)) * surfArea; 
		}//*/

		cout << "B_sum(" << B_sum[0] << ", " << B_sum[1] << ", " << B_sum[2] << ")" << endl;

		B_superSum += B_sum;
		F_v += velV.cross(B_sum);
	}

	// Allow some introspection for debugging
	debug.F_v = F_v;
	debug.B = B_superSum.norm() / I_k;

	// point mass dynamic system
	Vector3d accel = (-k_a * goalDiff + F_v - k_d * velV) / mass; 
	double U_a = 0.5 * k_a * (goalDiff.dot(goalDiff));

	//visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
	/*Vector3d newVel = velV + accel * U_a * dT; //(velV + accel).normalized() * 0.2; // RETURN THIS
	// ROTATION MISSING


	cout << "F_v(" << F_v[0] << ", " << F_v[1] << ", " << F_v[2] << ")" << endl
		 << "accel(" << accel[0] << ", " << accel[1] << ", " << accel[2] << ")" << endl
		 << "newVel(" << newVel[0] << ", " << newVel[1] << ", " << newVel[2] << ")" << endl;
	//*/

	//visPub.publish(vis);

	return accel;// * U_a;
}

