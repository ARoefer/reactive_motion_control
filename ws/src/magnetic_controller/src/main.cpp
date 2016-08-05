
#include "utils.h"
#include "MeshManager.h"
#include "Mesh.h"

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

		ParticleCloud* pBoardMesh, *pWallMesh, *pInnerWallMesh, *pBackWallMesh, *pUnitCubeMesh;
		pBoardMesh      = meshManager.getNewMesh("board");
		pWallMesh 		= meshManager.getNewMesh("wall");
		pInnerWallMesh  = meshManager.getNewMesh("inner_wall");
		pBackWallMesh   = meshManager.getNewMesh("back_wall");
		pUnitCubeMesh   = meshManager.getNewMesh("unit_cube");
		makeCubeMesh(pBoardMesh, Vector3d(0.4, 1.2, 0.04));
		makeCubeMesh(pWallMesh, Vector3d(0.4, 0.04, 1.2));
		makeCubeMesh(pInnerWallMesh, Vector3d(0.4, 0.04, 0.4));
		makeCubeMesh(pBackWallMesh, Vector3d(0.04, 1.2, 1.2));
		makeCubeMesh(pUnitCubeMesh, Vector3d(1, 1, 1));

		objects.reserve(10);

		objects.push_back(TFObject<ParticleCloud>(pUnitCubeMesh, "cylinder1", &tfListener));
		objects.push_back(TFObject<ParticleCloud>(pUnitCubeMesh, "cylinder2", &tfListener));
		/*objects.push_back(TFObject<ParticleCloud>(pWallMesh, "wall_r", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pWallMesh, "wall_l", &tfListener));
		objects.push_back(TFObject<ParticleCloud>(pBackWallMesh, "wall_b", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pInnerWallMesh, "wall_1", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pInnerWallMesh, "wall_2", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pInnerWallMesh, "wall_3", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pBoardMesh, "board_0", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pBoardMesh, "board_1", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pBoardMesh, "board_2", &tfListener)); 
		objects.push_back(TFObject<ParticleCloud>(pBoardMesh, "board_3", &tfListener));//*/

		for (int i = 0; i < objects.size(); i++) {
			core.objects.push_back(&objects[i]);
		}

		cmdSub = nh.subscribe("mg_goal", 1, &ControllerNode::refreshGoal, this);

		for (int i = 0; i < eMAX; i++) {
			visManager.addNamespace(i, namespaces[i]);
		}
		visObjManager.addNamespace(0, "point_cloud_objects");
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

		visualizeObjects(core.objects, visFieldMsg.markers, 0, visObjManager);

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
					
					if (velX.data == velX.data && velY.data == velY.data && velZ.data == velZ.data) {
						pubX.publish(velX);
						pubY.publish(velY);
						pubZ.publish(velZ);//*/
					}

					cout << vel << endl;
				}
			} catch (tf::TransformException ex) {
				cerr << "Lookup of frame 'eef_link' failed!" << endl;
			}

			visMsg.markers.push_back(visManager.trailMarker(eTrail, visitedPoints, 0.02f));
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
		int gridHeight = 0;
		double spaceStep = 1.0 / gridRes;
		Vector3d zero(0, -2, -0); 

		visualization_msgs::Marker marker;
		marker.ns = "field";
		marker.header.frame_id = "odom_combined";
		marker.header.stamp = ros::Time::now();
		marker.id = 0;
		marker.type = visualization_msgs::Marker::POINTS;
		marker.scale.x = marker.scale.y = 0.04;

		core.bDrawDebug = false;

		vector<double> strengths;
		double highestS = 0.1;

		for (int x = 0; x <= gridWidth * gridRes; ++x)
		{
			for (int y = 0; y <= gridWidth * gridRes; ++y)
			{
				for (int z = 0; z <= gridHeight * gridRes; ++z)
				{
					Vector3d pos = zero + Vector3d(x * spaceStep, y * spaceStep, z * spaceStep);
					core.calculateAccel(pos, goal, vel);

                    double auxB = 0.02 * core.debug.B;
					if (auxB > 0.1) 
					{
						geometry_msgs::Point point;
						point.x = pos[0];
						point.y = pos[1];
						point.z = pos[2]; // Visual offset
						marker.points.push_back(point);
						marker.colors.push_back(hsv2rgb(max(0.0, 1.0 - auxB) * 240, 1 , 1));
					} //else
						//cout << core.debug.B << endl;
				}
			}
		}

		//marker.colors.reserve(strengths.size());
		
		core.bDrawDebug = true;

		visMsg.markers.push_back(marker);
	}

private:
	NodeHandle nh;
	Publisher pubX, pubY, pubZ, pubVis;
	Subscriber cmdSub;

	tf::TransformListener tfListener;

	MagneticCore_ParticleModel core;
	MeshManager<ParticleCloud> meshManager;
	vector<TFObject<ParticleCloud>> objects;

	Vector3d goal;

	VisualizationManager visManager;
	VisualizationManager visObjManager;

	vector<Vector3d> visitedPoints;
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