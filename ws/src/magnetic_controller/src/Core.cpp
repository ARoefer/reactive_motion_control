#include "magnetic_controller/Core.h"

#include "Mesh.h"
#include "ObjectBase.h"

#include <r_libs/R_Conversions.h>

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

void MagneticCore_ParticleModel::addObject(ObjectBase<ParticleCloud>* pObj) {
	MagneticCore<ParticleCloud>::addObject(pObj);
	pointCount += pObj->pMesh->particles.size();
}

Vector3d MagneticCore_ParticleModel::calculateAccel(Vector3d pos, Vector3d goalPos, Vector3d velV) {

	Vector3d F_v = Vector3d::Zero(); // Total force
	double I_k = parameters.I_k; // Field strength
	double k_a = parameters.k_a; // Attractive constant
	double k_d = parameters.k_d; // Dampening constant

	Vector3d b = goalPos - pos; // Vector from position to goal
	double b_sq = b.squaredNorm(); // Squared length of b

	Vector3d vel_dir = velV.normalized(); // Direction of velocity
	if (velV.squaredNorm() == 0)
		vel_dir = b.normalized(); // Handle the case of no movement

	Vector3d B = Vector3d::Zero(); // Magnetic field vector

#ifndef SIM
	visualization_msgs::MarkerArray vis;
	if (bDrawDebug)
		visManager.beginNewDrawCycle();

	visualization_msgs::Marker markerA;
	markerA.ns = "used_points";
	markerA.header.frame_id = "odom_combined";
	markerA.header.stamp = ros::Time::now();
	markerA.id = 0;
	markerA.type = visualization_msgs::Marker::POINTS;
	markerA.scale.x = markerA.scale.y = 0.04;
	markerA.color.a = 1.0;
#endif

	bool overrideDirection = false;
	Vector3d overrideVector = Vector3d::Zero();

	for (ObjectBase<ParticleCloud>* pObj: objects) {
		Affine3d objPose = pObj->getTransform(); // Pose of the object
		Affine3d objPoseI = objPose.inverse(); // Transform w -> object

		Vector3d m_j_com = objPose.translation(); // Center of mass
		Matrix3d rotM = objPose.rotation(); // Rotation matrix
		Matrix3d rotMI = rotM.inverse(); // Inverted rotation matrix

		// ALL THINGS IN OBJECT SPACE FROM HERE ON OUT
		Vector3d pos_O 		= objPoseI * pos;
		Vector3d velV_O 	= objPoseI * velV;
		Vector3d vel_dir_O 	= rotMI * vel_dir;
		Vector3d b_O 		= rotMI * b;
		Vector3d goal_O		= objPoseI * goalPos;


		Vector3d B_j_O = Vector3d::Zero(); // Magnetic field vector per object
		Vector3d d_j_O = pos_O + b_O * (-pos_O.dot(b_O) / b_sq); // Shortest vector between b an m_j_com

		// Dynamic vector based on closest distance
		Vector3d r_j_O = Vector3d::UnitZ(); // Rotation vector default is Z-Axis of object rotation. Always normalized
		if (d_j_O.squaredNorm() > 0)
			r_j_O = d_j_O.cross(b_O).normalized();

		// if (goalPos[1] > pos[1])
		// 	r_j_O = -r_j_O;

		Vector3d c_j = Vector3d::Zero();

		for (size_t i = 0; i < pObj->pMesh->particles.size(); i++) {
			SParticle* 	p 		 = &(pObj->pMesh->particles[i]);
			Vector3d 	paP 	 = p->position; // Particle position in OS. 
			Vector3d 	paN 	 = p->normal; // Particle normal in OS
			Vector3d	pGoal_dir= (goal_O - paP).normalized();
			double 		scale 	 = 1;//copysign(1.0, -paN.dot(vel_dir_O)); // Scale the particles influence accoring to its angle to vel_dir
			double 		l_j_i 	 = max((pos_O - paP).norm(), 0.01); // Capped distance between point and particle
			double		l_j_i_sq = l_j_i * l_j_i; // Squared distance between position and particle i 
			Vector3d 	c_j_i 	 = r_j_O.cross(paN); // Current direction at this particle
			Vector3d 	inc_dir  = (paP - pos_O).normalized();
			double 		inc_ang	 = vel_dir_O.dot(inc_dir);
			double 		vel_face_ang= vel_dir_O.dot(paN);
			double 		face_ang = inc_dir.dot(paN);
			double		goal_ang = pGoal_dir.dot(paN);
			//scale *= inc_ang;

			bool filterOK = true;
			switch(parameters.filter) {
				case 'd':
				filterOK = face_ang < 0;
				break;
				case 'g':
				filterOK = face_ang < 0 && (goal_ang < 0);
				break;
				case 'a':
				filterOK = face_ang < 0 && ((goal_ang < 0) || (inc_ang > 0 && vel_face_ang < 0));
			}

			if (filterOK) 
			{
				if(parameters.surfFollowing && l_j_i < 0.06 && goal_ang < 0.7) {
					c_j += c_j_i * 0.1 / l_j_i;
					overrideDirection = true;
				}

				Vector3d B_j_i_O = I_k * (c_j_i.cross(vel_dir_O) / l_j_i_sq) * p->size * scale;
				B_j_O += B_j_i_O;

#ifndef	SIM
				if (bDrawDebug) {
					markerA.points.push_back(VectorToPoint(objPose * paP));

		            std_msgs::ColorRGBA color;
		            Vector3d B_norm = c_j_i;//B_j_i_O.normalized();
		            color.r = 0.5 + B_norm[0] * 0.5;
		            color.g = 0.5 + B_norm[1] * 0.5;
		            color.b = 0.5 + B_norm[2] * 0.5;
		            color.a = 1.0;//0.5 * scale + 0.5;
		            markerA.colors.push_back(color);
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paP, paN * 0.1, 1,1,0));
					vis.markers.push_back(visManager.vectorMarker(eCurrent, objPose * paP, (rotM * c_j_i) * 0.1, 1,0,0));
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paP, B_j_i, 0,1,0));
				}
#endif
			}
		}

		Vector3d B_j = rotM * B_j_O;
		overrideVector += rotM * c_j;
		F_v += velV.cross(B_j);
		B += B_j;

#ifndef SIM
		if (bDrawDebug) {
			visManager.annotatedVector(vis.markers, eMath, pos, (rotM * r_j_O), "r_j");
			visManager.annotatedVector(vis.markers, eMath, m_j_com, (rotM * d_j_O), "d_j");
		}
#endif

	}

#ifndef SIM
	if (bDrawDebug) {
		visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
		visManager.annotatedVector(vis.markers, eMath, pos, b, "b");
		visManager.annotatedVector(vis.markers, eMath, pos, velV, "v");
		vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, B, 1,0,0));
		vis.markers.push_back(markerA);
		visPub.publish(vis);
	}
#endif

	debug.F_v = F_v; 
	debug.B = B.norm();

	Vector3d acceleration = -k_a * (pos - goalPos) + F_v - k_d * velV;
	if (overrideDirection) {
		cout << "overriding direction" << endl;
		return overrideVector.normalized() * 0.2;
	} else {
		return velV + acceleration * 0.005;
	}
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


/*
	visualization_msgs::MarkerArray vis;
	if (bDrawDebug)
		visManager.beginNewDrawCycle();
	//visManager.clearAllMarkers(&visPub);

	double vel = parameters.vel;
	Vector3d b = goalPos - pos;
	Vector3d b_dir = b.normalized();
	double b_mag = b.norm();
	Vector3d velDir = velV.normalized();//b.normalized();
	if (velV.squaredNorm() == 0)
		velDir = b.normalized();
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

#ifndef SIM
	// VISUALIZATION
	visualization_msgs::Marker markerA;
	markerA.ns = "used_points";
	markerA.header.frame_id = "odom_combined";
	markerA.header.stamp = ros::Time::now();
	markerA.id = 0;
	markerA.type = visualization_msgs::Marker::POINTS;
	markerA.scale.x = markerA.scale.y = 0.04;
	markerA.color.a = 1.0;
	markerA.color.r = 1.0;

	visualization_msgs::Marker markerB;
	markerB.ns = "used_points";
	markerB.header.frame_id = "odom_combined";
	markerB.header.stamp = ros::Time::now();
	markerB.id = 1;
	markerB.type = visualization_msgs::Marker::POINTS;
	markerB.scale.x = markerB.scale.y = 0.04;
	markerB.color.a = 0.5;
	markerB.color.r = 1.0;
	markerB.color.g = 1.0;
	markerB.color.b = 1.0;

	vis.markers.push_back(markerA);
	vis.markers.push_back(markerB);
	// ~VISUALIZATION
#endif

	Vector3d B_superSum = Vector3d::Zero();
	// Iterate over all objects
	for (TParticleObject* pObj: objects) {
		// PER OBJECT
		Affine3d objPose = pObj->getTransform();
		Vector3d obj_com = objPose.translation();
		Matrix3d rotM = objPose.rotation();

		// cout << "objPose(" << objPose.matrix() << ")" << endl
		// 	 << "obj_com(" << obj_com[0] << ", " << obj_com[1] << ", " << obj_com[2] << ")" << endl
		// 	 << "rotM(" << rotM << ")" << endl;

		Vector3d d_j = pos + b * ((obj_com - pos).dot(b) / (b_mag * b_mag)) - obj_com;

		//Vector3d incidentDirection = (obj_com - pos).normalized();
		//Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;
		
		Vector3d rotV = d_j.cross(b).normalized();

#ifndef	SIM
		// VISUALIZATION
		if (bDrawDebug) {
			visManager.annotatedVector(vis.markers, eMath, pos, rotV, "rotV");
			visManager.annotatedVector(vis.markers, eMath, obj_com, d_j, "d_j");
			visManager.annotatedVector(vis.markers, eMath, pos, b, "b");
		}
		// ~VISUALIZATION
#endif

		// Iterate over all particles
		for (size_t i = 0; i < pObj->pMesh->particles.size(); i++) {
			SParticle* p = &(pObj->pMesh->particles[i]);
			Vector3d paPos = objPose * p->position;
			Vector3d paN = rotM * p->normal;
			Vector3d distSP = (paPos - pos);

			Vector3d incidentDirection = distSP.normalized();
			//B_sum += parameters.I_k / l_i_j * Vector3d(0,0,1);
			//B_sum += (sprCurrent.cross(incidentDirection)) * (parameters.I_k / (l_i_j / 0.2)) * sprArea; 
			
			// FILTER
			// Exclude all points that are not between the robot and the goal 
			// or in the movement direction of the robot
			//if ((incidentDirection.dot(velDir) > 0 && incidentDirection.dot(paN) < 0) || incidentDirection.dot(b_dir) > 0) 
			{
				Vector3d incidentVelocity = velV.dot(incidentDirection)*incidentDirection;

				double l_i_j = distSP.squaredNorm();
				Vector3d B = (parameters.I_k / l_i_j) * p->size * (paN.cross(rotV)).cross(velDir);
				B_superSum += B; // DEBUG
				F_v += velV.cross(B); // Add to overall force

			/*cout << "paPos(" << paPos[0] << ", " << paPos[1] << ", " << paPos[2] << ")" << endl
				 << "paN(" << paN[0] << ", " << paN[1] << ", " << paN[2] << ")" << endl
				 << "B(" << B[0] << ", " << B[1] << ", " << B[2] << ")" << endl
				 << "incidentDirection(" << incidentDirection[0] << ", " << incidentDirection[1] << ", " << incidentDirection[2] << ")" << endl
				 << "incidentVelocity(" << incidentVelocity[0] << ", " << incidentVelocity[1] << ", " << incidentVelocity[2] << ")" << endl
				 << "F_v(" << F_v[0] << ", " << F_v[1] << ", " << F_v[2] << ")" << endl;//
#ifndef SIM
				if (bDrawDebug) {
					geometry_msgs::Point point;
					point.x = paPos[0];
					point.y = paPos[1];
					point.z = paPos[2]; // Visual offset
					vis.markers[0].points.push_back(point);
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paPos, B * 5, 1,1,0));
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paPos, incidentVelocity, 1,0,1));				
				}
#endif
			} /*else if (bDrawDebug) {
				geometry_msgs::Point point;
				point.x = paPos[0];
				point.y = paPos[1];
				point.z = paPos[2]; // Visual offset
				vis.markers[1].points.push_back(point);
			}//
		}

		//F_v += incidentVelocity.cross(B_sum);
		// cout << "B_sum(" << B_sum[0] << ", " << B_sum[1] << ", " << B_sum[2] << ")" << endl;
	}

	// Allow some introspection for debugging
	debug.F_v = F_v;
	debug.B = B_superSum.norm() / I_k;

	// point mass dynamic system
	Vector3d accel = (-k_a * goalDiff + F_v - k_d * velV) / mass; 
	double U_a = 0.5 * k_a * (goalDiff.dot(goalDiff));

#ifdef SIM
	if (bDrawDebug) {
		visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
		vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, B_superSum, 1,0,0));

		visManager.endDrawCycle(vis.markers);
		visPub.publish(vis);
	}
#endif

	return accel;// * U_a; */