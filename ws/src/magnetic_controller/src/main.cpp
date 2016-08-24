
//#define SIM
#include "utils.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "ControllerBridge.h"


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
#ifdef SIM
		, commander(0.005)
#else
		, commander(nh)
#endif
	{ }

	struct SNamedPoint {
		string name;
		Vector3d pos;
	};

	void init(string scene) {
		pubVis = nh.advertise<visualization_msgs::MarkerArray>("/magnetic_controller/visualization", 10);
		cmdSub = nh.subscribe("mg_goal", 1, &ControllerNode::refreshGoal, this);

		for (int i = 0; i < eMAX; i++) {
			visManager.addNamespace(i, namespaces[i]);
		}
		visObjManager.addNamespace(0, "point_cloud_objects");

		loadScene(scene);
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
		drawField(visFieldMsg, Vector3d::Zero());

		visObjManager.beginNewDrawCycle();
		visualizeObjects(core.objects, visFieldMsg.markers, 0, visObjManager);
		visObjManager.endDrawCycle(visFieldMsg.markers);

		cout << "Init finished" << endl;
		pubVis.publish(visFieldMsg);

#ifdef SIM
		core.bDrawDebug = false;
		ros::spinOnce();
#endif

		core.refreshParams();
		
		bool quit = false;
		while(!quit) {

			double distanceTraveled = 0;
			double timeTaken = 0;
			while (!quit && goalIndex < goals.size()) {
				quit = !ros::ok();
				
				Affine3d eef;
				if (commander.getCurrentEEFPose(eef)) {
					pos = eef.translation();

					if (visitedPoints.size() == 0 || (visitedPoints[visitedPoints.size() - 1] - pos).norm() >= 0.05)
						visitedPoints.push_back(pos);

					Vector3d goal = goals[goalIndex].pos;
					if ((pos - goal).norm() > 0.02) {
						//if (recalcVelocity) {
							//vel = (goal - pos).normalized() * 0.2;
							//recalcVelocity = false;
						//}
#ifndef SIM
						core.refreshParams();
#endif
						Vector3d newVel = vel + core.calculateAccel(pos, goal, vel) * step;

						if (newVel[0] == newVel[0] && newVel[1] == newVel[1] && newVel[2] == newVel[2]) {
						// if (vel.norm() > 0.2)
						// 	vel = vel.normalized() * 0.2;
							timeTaken += step;
							vel = newVel;
							Vector3d direction = vel.normalized();
							double magnitude = min(vel.norm(), 1.0);
							Vector3d velLimited = magnitude*direction;
							distanceTraveled += step * velLimited.norm();
							commander.setVelocity(velLimited);
							//vel = velLimited;
						}

#ifndef SIM
						cout << vel << endl;
#endif
					} else {
						goalIndex++;
						if (goalIndex == goals.size()) {
							cout << "All goals reached!" << endl;
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
							vel = Vector3d::Zero();
							cout << "Goal reached in " << timeTaken << " s. Distance: " << distanceTraveled << endl;
							cout << "Next goal is " << goals[goalIndex].name << endl;
						}
					}
#ifndef SIM
					visManager.beginNewDrawCycle();
					visualization_msgs::MarkerArray visMsg;
					visMsg.markers.push_back(visManager.trailMarker(eTrail, visitedPoints, 0.02f));
					visMsg.markers.push_back(visManager.sphereMarker(eGoal, goal, 0.1f, 1,0,1,0.5f));
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
			ctrlRate.sleep();
#endif

			if (!quit) {
				cout << "input 'q' to quit or 'r' to re-run" << endl;
				string cmd;
				cin >> cmd;
				if (cmd.compare("r") == 0) {
					commander.setPosition(starts[0].pos);
					commander.setVelocity(Vector3d::Zero());
					goalIndex = 0;
					visitedPoints.clear();
					core.refreshParams();
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
		marker.scale.x = marker.scale.y = 0.04;

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
					core.calculateAccel(pos, goal, vel);

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


	void loadScene(string filename) {
		//vel = Vector3d::Zero();
		goals.clear();
		goalIndex = 0;
		core.objects.clear();
		objects.clear();

		ptree pt;

		read_xml(filename, pt);

		ptree &sceneNode = pt.get_child("scene");

		for (const pair<string, ptree> &p: sceneNode) {
			ptree node = p.second;
			if (p.first.compare("obstacle") == 0) {
				loadObject(node);
			} else if (p.first.compare("goal") == 0) {
				SNamedPoint goal;
				goal.name = node.get<string>("<xmlattr>.name");
				goal.pos = parsePosition(node);
				goals.push_back(goal);
			} else if (p.first.compare("start") == 0) {
				SNamedPoint start;
				start.name = node.get<string>("<xmlattr>.name");
				start.pos = parsePosition(node);
				starts.push_back(start);
			}
		}

		commander.setPosition(starts[0].pos);
	}

protected:
	void loadObject(ptree &node) {
		string mesh = node.get<string>("<xmlattr>.mesh");
		string name = node.get<string>("<xmlattr>.name");
		bool stat = node.get<bool>("<xmlattr>.static");
		
		ParticleCloud *pMesh = meshManager.getMeshByName(mesh);
		if (!pMesh) {
			pMesh = meshManager.getNewMesh(mesh);
			loadFromFile(pMesh, mesh);
		}

		boost::shared_ptr<ObjectBase<ParticleCloud>> objectPtr;
		if (stat) {
			Affine3d transform = parseTransform(node.get_child("transform"));
			objectPtr = boost::shared_ptr<ObjectBase<ParticleCloud>>(new StaticObject<ParticleCloud>(pMesh, name, transform));
		} else {
			objectPtr = boost::shared_ptr<ObjectBase<ParticleCloud>>(new TFObject<ParticleCloud>(pMesh, name, &tfListener));
		}

		objects.push_back(objectPtr);
		core.addObject(objectPtr.get());
	}

private:
	NodeHandle nh;
	Publisher pubVis;
	Subscriber cmdSub;
	tf::TransformListener tfListener;

#ifdef SIM
	SimCommander commander;
#else
	ROSCommander commander;
#endif

	MagneticCore_ParticleModel core;
	MeshManager<ParticleCloud> meshManager;
	vector<boost::shared_ptr<ObjectBase<ParticleCloud>>> objects;

	int goalIndex;
	vector<SNamedPoint> goals;
	vector<SNamedPoint> starts;

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