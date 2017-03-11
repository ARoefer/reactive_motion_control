
//#define SIM
#define PR2
#define MODULATION
#include "utils.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "ControllerBridge.h"
#include "Scenery.h"


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
		, goalIndex(0)
		, scene(&meshManager, &physMeshManager, &tfListener)
#ifdef SIM
		, commander(0.005)
#else
#ifdef PR2
		, commander(nh, "base_link", "r_gripper_tool_frame")
#else
		, commander(nh, "link0", "eef_link")
#endif
#endif
	{ }

	void init(string scenePath) {
		pubVis = nh.advertise<visualization_msgs::MarkerArray>("/magnetic_controller/visualization", 10);
		cmdSub = nh.subscribe("mg_goal", 1, &ControllerNode::refreshGoal, this);

		for (int i = 0; i < eMAX; i++) {
			visManager.addNamespace(i, namespaces[i]);
		}
		visObjManager.addNamespace(0, "point_cloud_objects");

		scene.loadScene(scenePath);

		core.objects = scene.getObjects();

		SNamedPoint start;
		if (scene.getStart(0, start)) {
			commander.setPosition(start.pos);
		} else {
			cerr << "Could not set starting position" << endl;
		}
	}
	
	void refreshGoal(geometry_msgs::Point point) {
		// goal[0] = point.x;
		// goal[1] = point.y;
		// goal[2] = point.z;
		// recalcVelocity = true;
		// cout << "refreshed goal: " << goal << endl;
	}

	void run() {
		double step = 0.005;
		ros::Rate ctrlRate(100);
		Vector3d pos;
		Vector3d vel;

		visualization_msgs::MarkerArray visFieldMsg;
		//drawField(visFieldMsg, Vector3d::Zero());

		visObjManager.beginNewDrawCycle();
		visualizeObjects(core.objects, visFieldMsg.markers, eObjects, visObjManager);
		visObjManager.endDrawCycle(visFieldMsg.markers);

		cout << "Init finished" << endl;
		pubVis.publish(visFieldMsg);

#ifdef SIM
		core.bDrawDebug = false;
		ros::spinOnce();
#endif

		cout << "Debug mode: " << core.bDrawDebug << endl;

		core.refreshParams();

		vector<string>	 pointNames;
		scene.getPointNames(pointNames);
		
		bool quit = false;
		while(!quit) {

			double distanceTraveled = 0;
			double timeTaken = 0;
			bool outputError = false;

			cout << "Waypoints: ";
			for (string name: pointNames) 
				cout << "'" << name << "'" << endl;

			cout << "Enter a sequence of waypoints to reach" << endl;
			string pathSeq;
			getline(std::cin, pathSeq);
			
			string::iterator beginIt, endIt;
			beginIt = pathSeq.begin();
			endIt = beginIt;

			vector<string> path;

			while (beginIt != pathSeq.end()) {
				while(beginIt != pathSeq.end() && *beginIt == ' ')
					beginIt++;

				endIt = beginIt;

				while(endIt != pathSeq.end() && *endIt != ' ')
					endIt++;

				if (beginIt != pathSeq.end()) {
					path.push_back(string(beginIt, endIt));
				}

				beginIt = endIt;
			}

			SNamedPoint goalPoint;
			bool goalsLeft = goalIndex < path.size();
			while (!quit && goalsLeft) {
				scene.getPoint(path[goalIndex], goalPoint);
				quit = !ros::ok();
				
				Affine3d eef;
				if (commander.getCurrentEEFPose(eef)) {
					Vector3d tempPos = eef.translation();
					distanceTraveled += (pos - tempPos).norm();
					pos = tempPos;

					if (visitedPoints.size() == 0 || (visitedPoints[visitedPoints.size() - 1] - pos).norm() >= 0.05)
						visitedPoints.push_back(pos);

					Vector3d goal = goalPoint.pos;
					if ((pos - goal).norm() > 0.02) {
						
#ifndef SIM
						core.refreshParams();
#endif
#ifdef MODULATION
						Vector3d vel = goal - pos;
						if (vel.norm() > 0.2)
							vel = vel.normalized() * 0.2;				
#endif
						ros::Time now = ros::Time::now();
						Vector3d newVel = core.calculateAvoidance(pos, goal, vel, step);// * step;
						ros::Duration passed = ros::Time::now() - now;

						// cout << "Calculation took " << passed.toSec() << " seconds" << endl;
						// cout << "   Particles: " << core.debug.particlesExamined << endl;

						if (newVel[0] == newVel[0] && newVel[1] == newVel[1] && newVel[2] == newVel[2]) {
							timeTaken += step;
							
#ifndef MODULATION
							vel = newVel;
							if (vel.norm() > 0.2)
								vel = vel.normalized() * 0.2;
							
							Vector3d direction = vel.normalized();
							double magnitude = min(vel.norm(), 0.2);
							Vector3d velLimited = magnitude*direction;
							commander.setVelocity(velLimited);
							vel = velLimited;
#else
							commander.setVelocity(newVel);
							printf("%f = |%f, %f, %f|\n", newVel.norm(), newVel[0], newVel[1], newVel[2]);
							outputError = false;
#endif
						} 
#ifndef SIM
						else if(!outputError) {
							outputError = true;
							cout << "vel contains NaN" << endl;
							printMatrices();
						}
#endif

					} else {
						goalIndex++;
						commander.setVelocity(Vector3d::Zero());
						goalsLeft = goalIndex < path.size();
						if (!goalsLeft) {
							cout << "All goals reached!" << endl 
								 << "          Time taken: " << timeTaken << endl
								 << "   Distance traveled: " << distanceTraveled << endl
								 << "       Average Speed: " << distanceTraveled / timeTaken << endl;
#ifdef SIM
							visManager.beginNewDrawCycle();
							visualization_msgs::MarkerArray visMsg;
							visMsg.markers.push_back(visManager.trailMarker(eTrail, visitedPoints, 0.02f));
							
							for (SNamedPoint g: goals) {
								visMsg.markers.push_back(visManager.sphereMarker(eGoal, g.pos, 0.1f, 1,0,1,0.5f));
								visMsg.markers.push_back(visManager.textMarker(eGoal, g.pos + Vector3d(0,0,0.2), g.name, 1,1,1,1,.1));
							}

							visualizeObjects(core.objects, visMsg.markers, 0, visObjManager);

							visManager.endDrawCycle(visMsg.markers);


							pubVis.publish(visMsg);
							ros::spinOnce();
#endif
						} else {
							scene.getPoint(path[goalIndex], goalPoint);
							vel = Vector3d::Zero();
							cout << "Goal reached in " << timeTaken << " s. Distance: " << distanceTraveled << endl;
							cout << "Next goal is " << goalPoint.name << endl;
						}
					}
#ifndef SIM
					visManager.beginNewDrawCycle();
					visualization_msgs::MarkerArray visMsg;
					visMsg.markers.push_back(visManager.trailMarker(eTrail, visitedPoints, 0.02f));
					SNamedPoint tempGoal;
					for (size_t i = 0; i < pointNames.size(); i++) {
						scene.getPoint(pointNames[i], tempGoal);
						Affine3d gPose = Affine3d::Identity();
						gPose.translate(tempGoal.pos);
						Vector3d scale = Vector3d(1,1,1);
						Vector4f color = Vector4f::Zero();
						if (i == goalIndex) {
							scale *= 1.2;
							visMsg.markers.push_back(visManager.sphereMarker(eGoal, tempGoal.pos, 0.02f, 1,0,1,0.5f));
							color[1] = color[3] = 1.f;
						}
						visMsg.markers.push_back(visManager.meshMarker(eGoal, gPose, scale, "package://magnetic_controller/meshes/marker_goal.dae", "odom_combined", color));
						visMsg.markers.push_back(visManager.textMarker(eGoal, tempGoal.pos + Vector3d(0,0,0.2), tempGoal.name, 1,1,1,1,.1));
					}
					visualizeObjects(core.objects, visMsg.markers, eObjects, visManager);
					visManager.endDrawCycle(visMsg.markers);

					pubVis.publish(visMsg);
#endif
					
				}
			}
#ifdef SIM
			/*else {
				ros::spinOnce();
				ctrlRate.sleep();
			}//*/
#else
			ros::spinOnce();
			//ctrlRate.sleep();
#endif

			if (!quit) {
				cout << "input 'q' to quit or 'r' to re-run" << endl;
				string cmd;
				cin >> cmd;
				if (cmd.compare("r") == 0) {
					commander.setVelocity(Vector3d::Zero());
					SNamedPoint start;
					if (scene.getStart(0, start)) {
						commander.setPosition(start.pos);
						pos = start.pos;
					} else {
						cerr << "Could not set starting position" << endl;
					}
					goalIndex = 0;
					visitedPoints.clear();
					core.refreshParams();
					cin.ignore();
				} else if (cmd.compare("q") == 0) {
					quit = true;
				}
			}
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
		marker.scale.x = marker.scale.y = 0.01;

		core.bDrawDebug = false;

		vector<double> strengths;
		double highestS = 0.1;

		Vector3d goal(4,0,0);

		for (int x = 0; x <= gridWidth * gridRes; ++x)
		{
			for (int y = 0; y <= gridWidth * gridRes; ++y)
			{
				for (int z = 0; z <= gridHeight * gridRes; ++z)
				{
					Vector3d pos = zero + Vector3d(x * spaceStep, y * spaceStep, z * spaceStep);
					core.calculateAvoidance(pos, goal, vel, 0.01);

                    double auxB = core.debug.B;
					//if (auxB > 0.1) 
					{
						geometry_msgs::Point point;
						point.x = pos[0];
						point.y = pos[1];
						point.z = pos[2] - 0.5; // Visual offset
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

	void printMatrices() {
		for (size_t n = 0; n < core.debug.V.size(); n++) {
			cout << "Matrix E_" << n << " : " << endl << core.debug.E[n] << endl;
			cout << "Matrix V_" << n << " : " << endl << core.debug.V[n] << endl;
			cout << "Matrix Vi_" << n << ": " << endl << core.debug.Vi[n] << endl;
		}
		cout << "Matrix M: " << endl << core.debug.M << endl;
	}
private:
	NodeHandle nh;
	Publisher pubVis;
	Subscriber cmdSub;
	tf::TransformListener tfListener;

#ifdef SIM
	SimCommander commander;
#else
	GiskardCommander commander;
#endif

#ifdef MODULATION
	DynamicSystemModulation core;
#else
	CircularFields core;
#endif
	Scenery<ParticleCloud> scene;
	MeshManager<ParticleCloud> meshManager;
	MeshManager<shape_msgs::Mesh> physMeshManager;
	vector<boost::shared_ptr<ObjectBase<ParticleCloud>>> objects;

	int goalIndex;

	VisualizationManager visManager;
	VisualizationManager visObjManager;

	vector<Vector3d> visitedPoints;
};

int main(int argc, char** argv)
{
	if (argc < 2) {
		cout << "Path to scene to load needed!" << endl;
		return 0;
	}
	cout << argv[1] << endl;

	ros::init(argc, argv, "magnetic_controller");
	
	NodeHandle nh;
	ControllerNode ctrlNode(nh);
	ctrlNode.init(argv[1]);
	ctrlNode.run();

	return 0;
}