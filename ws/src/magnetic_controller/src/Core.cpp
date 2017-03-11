#include "magnetic_controller/Core.h"

#include "Mesh.h"
#include "ObjectBase.h"

#include <r_libs/R_Conversions.h>

#include <visualization_msgs/MarkerArray.h>

#include <iostream>
#include <cmath>

#include <stdlib.h>  

using namespace std;

#define VISUALIZE

typedef ObjectBase<Mesh> TMeshObject;
typedef ObjectBase<ParticleCloud> TParticleObject;

template<class T>
CollisionAvoidance<T>::CollisionAvoidance(const NodeHandle& _nh)
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

CircularFields::CircularFields(const NodeHandle& _nh) 
: CollisionAvoidance<ParticleCloud>(_nh)
{}

DynamicSystemModulation::DynamicSystemModulation(const NodeHandle& _nh)
: CollisionAvoidance<ParticleCloud>(_nh)
{}

void CircularFields::addObject(ObjectBase<ParticleCloud>* pObj) {
	CollisionAvoidance<ParticleCloud>::addObject(pObj);
	pointCount += pObj->pMesh->particles.size();
}

Vector3d CircularFields::calculateAvoidance(Vector3d pos, Vector3d goalPos, Vector3d velV, double dT) {

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

	debug.particlesExamined = 0;

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
	markerA.scale.x = markerA.scale.y = 0.01;
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
		switch (parameters.rotVector) {
			case 'a': {
				if (d_j_O.squaredNorm() > 0)
					r_j_O = d_j_O.cross(b_O).normalized();
				}
				break;
			case 'x': {
				if (goalPos[1] > pos[1])
					r_j_O = rotM * (-Vector3d::UnitX()).cross(b.normalized());
				}
				break;
			case 'l': {
				r_j_O = rotM * -Vector3d::UnitZ();
				if (goalPos[1] > pos[1])
					r_j_O = -r_j_O;
				}
				break;
			default: 
				break;
		}


		Vector3d c_j = Vector3d::Zero();

		// DEBUG
		debug.particlesExamined += pObj->pMesh->particles.size();

		for (size_t i = 0; i < pObj->pMesh->particles.size(); i++) {
			SParticle* 	p 		 = &(pObj->pMesh->particles[i]);
			Vector3d 	paP 	 = p->position; // Particle position in OS. 
			Vector3d 	paN 	 = p->normal; // Particle normal in OS
			Vector3d	pGoal_dir= (goal_O - paP).normalized();
			double 		scale 	 = 1;//copysign(1.0, -paN.dot(vel_dir_O)); // Scale the particles influence accoring to its angle to vel_dir
			double 		l_j_i 	 = max((pos_O - paP).norm() - parameters.margin, 0.001); // Capped distance between point and particle
			double		l_j_i_sq = l_j_i * l_j_i; // Squared distance between position and particle i 
			Vector3d 	c_j_i 	 = r_j_O.cross(paN); // Current direction at this particle
			Vector3d 	inc_dir  = (paP - pos_O);
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
				default:
					break;
			}

			if (filterOK) 
			{
				if(parameters.surfFollowing && l_j_i <= 0.001 && goal_ang < 0.7) {
					c_j += c_j_i * 0.1 / l_j_i;
					overrideDirection = true;
				}

				Vector3d B_j_i_O = I_k * (c_j_i.cross(vel_dir_O) / l_j_i_sq) * p->size * scale;
				B_j_O += B_j_i_O;

#ifndef	SIM
#ifdef VISUALIZE
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
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, objPose * paP, (rotM * c_j_i) * 0.1, 1,0,0));
					//vis.markers.push_back(visManager.vectorMarker(eCurrent, paP, B_j_i, 0,1,0));
				}
#endif
#endif
			}
		}

		Vector3d B_j = rotM * B_j_O;
		overrideVector += rotM * c_j;
		F_v += velV.cross(B_j);
		B += B_j;

#ifndef SIM
#ifdef VISUALIZE
		if (bDrawDebug) {
			visManager.annotatedVector(vis.markers, eMath, m_j_com, (rotM * r_j_O), "r_j");
			visManager.annotatedVector(vis.markers, eMath, m_j_com, (rotM * d_j_O), "d_j");
		}
#endif
#endif

	}

#ifndef SIM
#ifdef VISUALIZE
	if (bDrawDebug) {
		visManager.annotatedVector(vis.markers, eMath, pos, F_v, "F_v");
		visManager.annotatedVector(vis.markers, eMath, pos, b, "b");
		visManager.annotatedVector(vis.markers, eMath, pos, velV, "v");
		vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, B, 1,0,0));
		vis.markers.push_back(markerA);
		visPub.publish(vis);
		cout << "Published vis" << endl;
	}
#endif
#endif

	debug.F_v = F_v; 
	debug.B = B.norm();

	Vector3d acceleration = -k_a * (pos - goalPos) + F_v - k_d * velV;
	if (overrideDirection) {
		cout << "overriding direction" << endl;
		return overrideVector.normalized() * 0.2;
	} else {
		return velV + acceleration * dT;
	}
}


Vector3d DynamicSystemModulation::calculateAvoidance(Vector3d pos, Vector3d goalPos, Vector3d velV, double dT) {

	Vector3d b = goalPos - pos; // Vector from position to goal

	velV = b.normalized() * .1;

	debug.V.clear();
	debug.Vi.clear();
	debug.E.clear();

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

	double dists[objects.size()];
	Vector3d closestParticle[objects.size()];
	Vector3d normals[objects.size()]; 

	double minDist = 10000;
	int zeroDists = 0;

	for (size_t i = 0; i < objects.size(); i++) {
		Affine3d objPose = objects[i]->getTransform(); // Pose of the object
		Affine3d objPoseI = objPose.inverse(); // Transform w -> object

		Matrix3d rotM = objPose.rotation(); // Rotation matrix

		// ALL THINGS IN OBJECT SPACE FROM HERE ON OUT
		Vector3d pos_O = objPoseI * pos;
		
		double sqDist = 100000;

		for (size_t n = 0; n < objects[i]->pMesh->particles.size(); n++) {
			SParticle* 	p 		 = &(objects[i]->pMesh->particles[n]);
			Vector3d p2r = pos_O - p->position;
			double pSqDist = p2r.squaredNorm();
			if (pSqDist < sqDist && p->normal.dot(p2r) > 0) {
				closestParticle[i] = objPose * p->position;
				normals[i] = rotM * p->normal;
				sqDist = pSqDist;
			} 
		}

		dists[i] = max(sqrt(sqDist) - parameters.margin, 0.0);
		if (dists[i] < minDist)
			minDist = dists[i];

		Vector3d p_ = pos - closestParticle[i];
		if (dists[i] == 0.0 && (velV.dot(p_) < 0 || parameters.m))
			zeroDists++;
	}

	Matrix3d M = Matrix3d::Identity();
	Vector3d escVel = Vector3d::Zero();

	//zeroDists = 0;

	double minContDist = 100000;

	double baseWeight = zeroDists? 1.0 / zeroDists : 1.0;
	
	for (int i = 0; i < objects.size(); i++) {
		Vector3d p_ = pos - closestParticle[i];
		
		if ((dists[i] == 0.0 || zeroDists == 0) && (velV.dot(p_) < 0 || parameters.m)) {
			double w = baseWeight;

			for (int k = 0; k < objects.size() - 1 && zeroDists == 0; k++) {
				size_t idx = (i + k + 1) % objects.size();

				Vector3d temp = pos - closestParticle[idx];	
				if (velV.dot(temp) < 0 || parameters.m)
					w *= dists[idx] / (dists[i] + dists[idx]);
			}

			if (w != w) { // Iffy
				w = 0.0;
				cerr << "Weight is NaN" << endl;
			}

			Vector3d normal = normals[i];
			normal[0] += (rand() % 200 - 100) / 100.0 * parameters.noise;
			normal[1] += (rand() % 200 - 100) / 100.0 * parameters.noise;
			normal[2] += (rand() % 200 - 100) / 100.0 * parameters.noise;
			normal.normalized();


			double distExpP = pow(minDist + 1, 1.0 / parameters.p); 
			double l_1 = 1 - w / distExpP;
			double l_2 = 1 + w / distExpP;
			if (zeroDists > 0) {
				l_1 = 0;
				//l_2 = 1;
			}

			double l_3 = l_2;


			Vector3d objPos = objects[i]->getTransform().translation();
			Vector3d d_j = pos + b * ((objPos - pos).dot(b) / b.squaredNorm()) - objPos;
			Matrix3d V, Vi;

			Vector3d r_j = d_j.cross(b).normalized();
			if (d_j.squaredNorm() < 0.0000001 || abs(r_j.dot(normal)) > 0.99) {
				double pitch = -asin(normal[2]);
				double yaw = atan2(normal[1], normal[0]); 

				V = AngleAxisd(yaw, Vector3d::UnitZ()) * AngleAxisd(pitch, Vector3d::UnitY());
			 	Vi = V.inverse();
			 	cout << "Using rotation base" << endl;
			} else {
				Vector3d yAxis = normal.cross(-r_j);
				Vector3d zAxis = normal.cross(yAxis);

				V << normal[0], yAxis[0], zAxis[0],
				     normal[1], yAxis[1], zAxis[1],
				     normal[2], yAxis[2], zAxis[2];
			
			  	Vi = V.inverse();
				//cout << "Using current base" << endl;
			}

			
			escVel += V * Vector3d::UnitY() * w * 0.2;
		 	if (dists[i] < minContDist)
	 			minContDist = dists[i]; 

			Matrix3d E;
			E << l_1,   0, 0,
			       0, l_2, 0,
			       0,   0, l_3;

			debug.V.push_back(V);
			debug.Vi.push_back(Vi);
			debug.E.push_back(E);

			Matrix3d M_ = V * E;

#ifndef SIM
			if (bDrawDebug) {
				vis.markers.push_back(visManager.vectorMarker(eCurrent, closestParticle[i], M_ * Vector3d::UnitX() * 0.1, 1,0,0));
				vis.markers.push_back(visManager.vectorMarker(eCurrent, closestParticle[i], M_ * Vector3d::UnitY() * 0.1, 0,1,0));
				vis.markers.push_back(visManager.vectorMarker(eCurrent, closestParticle[i], M_ * Vector3d::UnitZ() * 0.1, 0,0,1));
				vis.markers.push_back(visManager.sphereMarker(eMath, closestParticle[i], 0.05, 1,0,1, 0.5f));
				//vis.markers.push_back(visManager.vectorMarker(eCurrent, pos, p_, 0,0,0));
			}
#endif
			M *= M_ * Vi;
		}
	}

	debug.M = M;

	double frac = max(1.0 - (minContDist) / parameters.margin, 0.0);

//	Vector3d modVel = M * velV;
	Vector3d modVel = M * (velV * (1.0 - frac) + (escVel * frac));
	if (modVel.norm() > 0.2)
		modVel.normalized() * 0.2;
	//cout << "Min contributing distance: " << minContDist << endl;
	// if (minContDist <= 0) {
	// //   	modVel = M * (escVel);
	// }
	if (bDrawDebug) {
		vis.markers.push_back(visManager.vectorMarker(eMath, pos, escVel * 10, 1,1,0));
		visManager.endDrawCycle(vis.markers);
		visPub.publish(vis);
	}

	return modVel;
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