#include <ros/ros.h>
#include <eigen3/Eigen/Eigen>
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>

#include <std_msgs/Float64.h>
#include <std_msgs/ColorRGBA.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

#include <r_libs/VisualizationManager.h>

#include "magnetic_controller/Core.h"
#include "Mesh.h"
#include "ObjectBase.h"
#include <cmath>

using namespace Eigen;
using namespace ros;
using namespace std;

std_msgs::ColorRGBA hsv2rgb(double h, double s, double v)
{
    double      hh, p, q, t, ff;
    long        i;
    std_msgs::ColorRGBA out;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = v;
        out.g = v;
        out.b = v;
        return out;
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = v;
        break;
    case 5:
    default:
        out.r = v;
        out.g = p;
        out.b = q;
        break;
    }

    out.a = 1;
    return out;     
}

void makeCubeMesh(Mesh &m, Vector3d dim) {
	Vector3d X(1,0,0);
	Vector3d Y(0,1,0);
	Vector3d Z(0,0,1);

	m.surfaces.push_back(Surface( X * 0.5 * dim[0],  X, dim[1] * dim[2]));
	m.surfaces.push_back(Surface(-X * 0.5 * dim[0], -X, dim[1] * dim[2]));
	m.surfaces.push_back(Surface( Y * 0.5 * dim[1],  Y, dim[0] * dim[2]));
	m.surfaces.push_back(Surface(-Y * 0.5 * dim[1], -Y, dim[0] * dim[2]));
	m.surfaces.push_back(Surface( Z * 0.5 * dim[2],  Z, dim[1] * dim[0]));
	m.surfaces.push_back(Surface(-Z * 0.5 * dim[2], -Z, dim[1] * dim[0]));

	m.dimensions = dim;
}

class ControllerNode
{
	enum ENameSpaces
	{
		eTrail,
		eForce,
		eObjects,
		eGoal,
		eAccel,
		eMAX
	};

	string namespaces[eMAX] = {"trail", "force", "objects", "goal", "acceleration"};

	bool recalcVelocity;

public:
	ControllerNode(const NodeHandle &_nh) 
		: nh(_nh)
		, core(_nh)
		, recalcVelocity(false)
	{ }

	void init() {
		pubX = nh.advertise<std_msgs::Float64>("/jointX_velocity_controller/command", 1);
		pubY = nh.advertise<std_msgs::Float64>("/jointY_velocity_controller/command", 1);
		pubZ = nh.advertise<std_msgs::Float64>("/jointZ_velocity_controller/command", 1);
		pubVis = nh.advertise<visualization_msgs::MarkerArray>("/magnetic_controller/visualization", 10);

		Mesh boardMesh, wallMesh, innerWallMesh, backWallMesh;
		makeCubeMesh(boardMesh, Vector3d(0.4, 1.2, 0.04));
		makeCubeMesh(wallMesh, Vector3d(0.4, 0.04, 1.2));
		makeCubeMesh(innerWallMesh, Vector3d(0.4, 0.04, 0.4));
		makeCubeMesh(backWallMesh, Vector3d(0.04, 1.2, 1.2));

		meshes.push_back(boardMesh);
		meshes.push_back(wallMesh);
		meshes.push_back(innerWallMesh);
		meshes.push_back(backWallMesh);

		Mesh* pBoardMesh = &meshes[0];
		Mesh* pWallMesh = &meshes[1];
		Mesh* pInnerWallMesh = &meshes[2];
		Mesh* pBackWallMesh = &meshes[3];

		objects.reserve(10);

		objects.push_back(TFObject(pWallMesh, "wall_r", &tfListener)); 
		objects.push_back(TFObject(pWallMesh, "wall_l", &tfListener));
		objects.push_back(TFObject(pBackWallMesh, "wall_b", &tfListener)); 
		objects.push_back(TFObject(pInnerWallMesh, "wall_1", &tfListener)); 
		objects.push_back(TFObject(pInnerWallMesh, "wall_2", &tfListener)); 
		objects.push_back(TFObject(pInnerWallMesh, "wall_3", &tfListener)); 
		objects.push_back(TFObject(pBoardMesh, "board_0", &tfListener)); 
		objects.push_back(TFObject(pBoardMesh, "board_1", &tfListener)); 
		objects.push_back(TFObject(pBoardMesh, "board_2", &tfListener)); 
		objects.push_back(TFObject(pBoardMesh, "board_3", &tfListener));

		for (int i = 0; i < objects.size(); i++) {
			core.objects.push_back(&objects[i]);
		}

		cmdSub = nh.subscribe("mg_goal", 1, &ControllerNode::refreshGoal, this);

		for (int i = 0; i < eMAX; i++) {
			visManager.addNamespace(i, namespaces[i]);
		}
	}
	
	void refreshGoal(geometry_msgs::Point point) {
		goal[0] = point.x;
		goal[1] = point.y;
		goal[2] = point.z;
		recalcVelocity = true;
		cout << "refreshed goal: " << goal << endl;
	}

	void run() {
		double step = 0.005;
		ros::Rate ctrlRate(100);
		Vector3d pos;
		Vector3d vel;

		visualization_msgs::MarkerArray visFieldMsg;
		drawField(visFieldMsg, Vector3d::Zero());
		cout << "Init finished" << endl;
		pubVis.publish(visFieldMsg);

		while(ros::ok()) {
			visManager.beginNewDrawCycle();
			visualization_msgs::MarkerArray visMsg;
			try {	
				tf::StampedTransform temp;
				tfListener.waitForTransform("odom_combined", "eef_link", ros::Time(0), ros::Duration(0.5));
				tfListener.lookupTransform("odom_combined", "eef_link", ros::Time(0), temp);

				Affine3d eef;
				tf::poseTFToEigen(temp, eef);

				pos = eef.translation();

				if (visitedPoints.size() == 0 || (visitedPoints[visitedPoints.size() - 1] - pos).norm() >= 0.05)
					visitedPoints.push_back(pos);

				if ((pos - goal).norm() > 0.02) {
					//if (recalcVelocity) {
						//vel = (goal - pos).normalized() * 0.2;
						//recalcVelocity = false;
					//}

					core.refreshParams();
					vel += core.calculateAccel(pos, goal, vel) * step;

					// if (vel.norm() > 0.2)
					// 	vel = vel.normalized() * 0.2;

					Vector3d direction = vel.normalized();
					double magnitude = min(vel.norm(), 1.0);
					Vector3d velLimited = magnitude*direction;
					std_msgs::Float64 velX;
					velX.data = velLimited[0];
					std_msgs::Float64 velY;
					velY.data = velLimited[1];
					std_msgs::Float64 velZ;
					velZ.data = velLimited[2];
					
					pubX.publish(velX);
					pubY.publish(velY);
					pubZ.publish(velZ);//*/

					cout << vel << endl;
				}
			} catch (tf::TransformException ex) {
				cerr << "Lookup of frame 'eef_link' failed!" << endl;
			}

			visMsg.markers.push_back(visManager.trailMarker(eTrail, visitedPoints, 0.02f));
			//visMsg.markers.push_back(visManager.vectorMarker(eForce, pos, core.debug.F_v, 1, 0, 0));
			for (ObjectBase* pObj: core.objects) {
				visMsg.markers.push_back(visManager.shapeMarker(eObjects, Affine3d::Identity(), visualization_msgs::Marker::CUBE, pObj->pMesh->dimensions, 0.5f,0.5f,0.5f, 1.f, pObj->name));
			}
			visMsg.markers.push_back(visManager.sphereMarker(eGoal, goal, 0.1f, 1,0,1,0.5f));
			visManager.endDrawCycle(visMsg.markers);

			pubVis.publish(visMsg);

			ros::spinOnce();
			ctrlRate.sleep();
		}
	}

	void drawField(visualization_msgs::MarkerArray& visMsg, Vector3d vel) {
		int gridWidth = 4;
		int gridRes = 5;
		int gridHeight = 2;
		double spaceStep = 1.0 / gridRes;
		Vector3d zero(0, -2, -0.5); 

		visualization_msgs::Marker marker;
		marker.ns = "field";
		marker.header.frame_id = "odom_combined";
		marker.header.stamp = ros::Time::now();
		marker.id = 0;
		marker.type = visualization_msgs::Marker::POINTS;
		marker.scale.x = marker.scale.y = 0.04;

		core.bDrawDebug = false;
		for (int x = 0; x <= gridWidth * gridRes; ++x)
		{
			for (int y = 0; y <= gridWidth * gridRes; ++y)
			{
				for (int z = 0; z <= gridHeight * gridRes; ++z)
				{
					Vector3d pos = zero + Vector3d(x * spaceStep, y * spaceStep, z * spaceStep);
					core.calculateAccel(pos, goal, vel);

					if (core.debug.B > 0.1) 
					{
						geometry_msgs::Point point;
						point.x = pos[0];
						point.y = pos[1];
						point.z = pos[2]; // Visual offset
						marker.points.push_back(point);
						marker.colors.push_back(hsv2rgb((1 - core.debug.B) * 240, 1 , 1));
					} //else
						//cout << core.debug.B << endl;
				}
			}
		}
		core.bDrawDebug = true;

		visMsg.markers.push_back(marker);
	}

private:
	NodeHandle nh;
	Publisher pubX, pubY, pubZ, pubVis;
	Subscriber cmdSub;

	tf::TransformListener tfListener;

	MagneticCore core;

	Vector3d goal;

	VisualizationManager visManager;

	vector<Vector3d> visitedPoints;
	vector<Mesh> meshes;
	vector<TFObject> objects;
};

int main(int argc, char** argv)
{
	ros::init(argc, argv, "magnetic_controller");
	
	NodeHandle nh;
	ControllerNode ctrlNode(nh);
	ctrlNode.init();
	ctrlNode.run();

	return 0;
}