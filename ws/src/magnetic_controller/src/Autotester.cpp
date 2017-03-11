#include "Autotester.h"
#include "ControllerBridge.h"
#include <iai_naive_kinematics_sim/simulator.hpp>


#include <moveit_msgs/DisplayTrajectory.h>

#include <yaml-cpp/yaml.h>

#include <sstream>

#include <ros/package.h>

using namespace iai_naive_kinematics_sim;

inline string resolveROSPath(const string& package, const string& file) {
    std::string package_path = ros::package::getPath(package);
    return package_path + "/" + file;
}

inline Vector3d KDL2Eigen(KDL::Vector vec) {
	return Vector3d(vec[0], vec[1], vec[2]);
}

trajectory_msgs::JointTrajectoryPoint makeTrajectoryPoint(const vector<string>& joints, const sensor_msgs::JointState& js, double dt) {
	trajectory_msgs::JointTrajectoryPoint out;
	out.time_from_start = ros::Duration(dt);
	size_t found = 0;
	for (size_t i = 0; i < js.name.size() && found < joints.size(); i++) {
		for (size_t n = 0; n < joints.size(); n++) {
			if (js.name[i].compare(joints[n]) == 0) {
				out.positions.push_back(js.position[i]);
				out.velocities.push_back(js.velocity[i]);
				found++;
				break;
			}
		}
	}

	return out;
}

void Test::init(ptree &root, MeshManager<ParticleCloud>* _pMeshManager, MeshManager<shape_msgs::Mesh>* _pPhysMeshManager, tf::TransformListener* pTfListener) {
	name = root.get<string>("<xmlattr>.name");
	file = root.get<string>("<xmlattr>.file");
	rate = root.get<double>("<xmlattr>.rate");
	ctrlType = root.get<string>("<xmlattr>.controller");
	scene = boost::shared_ptr<Scenery<ParticleCloud>>(new Scenery<ParticleCloud>(_pMeshManager, _pPhysMeshManager, pTfListener));
	scene->loadScene(file);

	ptree &robotsNode = root.get_child("robots");
	ptree &pathsNode = root.get_child("paths");
	ptree &configsNode = root.get_child("configs");

	parseRobots(robotsNode);
	parsePaths(pathsNode);
	parseConfigs(configsNode);
}

void Test::parseRobots(ptree& robotsNode) {
	for (const pair<string, ptree> &p: robotsNode) {
		if (p.first.compare("robot") == 0) {
			RobotWrapper robot;
			string package = p.second.get<string>("<xmlattr>.package");
			string urdfFile = resolveROSPath(package, p.second.get<string>("<xmlattr>.urdf"));
			string srdfFile = resolveROSPath(package, p.second.get<string>("<xmlattr>.srdf"));
			string contFile = resolveROSPath(package, p.second.get<string>("<xmlattr>.controlled_joints"));
			string giskFile = resolveROSPath(package, p.second.get<string>("<xmlattr>.giskard"));

			try {
				YAML::Node giskardNode = YAML::LoadFile(giskFile);
				giskard::QPControllerSpec spec = giskardNode.as< giskard::QPControllerSpec >();
				robot.controller = giskard::generate(spec);
			} catch (const YAML::Exception& e) {
				cerr << "parsing of giskard file failed! File: " << giskFile << endl;
				throw e;
			}

			try {
				YAML::Node controlledNode = YAML::LoadFile(contFile);
				robot.controlledJoints = controlledNode.as<vector<string>>();
			} catch (const YAML::Exception& e) {
				cerr << "parsing of controlled joints file failed! File: " << contFile << endl;
				throw e;
			}

			robot.urdf = boost::shared_ptr<urdf::Model>(new urdf::Model());
			if(!robot.urdf->initFile(urdfFile)) {
				cerr << "Parsing of URDF '" << urdfFile << "' failed in Test '" << name << "'" << endl;
			}

			robot.srdf = boost::shared_ptr<srdf::Model>(new srdf::Model());
			if (!robot.srdf->initFile(*(robot.urdf), srdfFile)) {
				cerr << "Parsing of SRDF '" << srdfFile << "' failed in Test '" << name << "'" << endl;	
			}

			robots[robot.urdf->getName()] = robot;
		}
	}	
}

void Test::parsePaths(ptree &pathsNode) {
	for (const pair<string, ptree> &p: pathsNode) {
		if (p.first.compare("path") == 0) {
			SPath path;
			path.name = p.second.get<string>("<xmlattr>.name");
			path.timeLimit = p.second.get<double>("<xmlattr>.maxTime");
			
			for (const pair<string, ptree> &n: p.second.get_child("initial_states")) {
				if (n.first.compare("state") == 0) {
					string robot = n.second.get<string>("<xmlattr>.robot");

					if (robots.find(robot) != robots.end()) {
						string file = resolveROSPath(n.second.get<string>("<xmlattr>.package"), n.second.get<string>("<xmlattr>.file"));
						YAML::Node jointstatesNode = YAML::LoadFile(file);
						map<string, double> statesMap = jointstatesNode.as<map<string, double>>();
						path.initialStates[robot] = sensor_msgs::JointState();

						for (auto it = statesMap.begin(); it != statesMap.end(); it++)
						{
							path.initialStates[robot].name.push_back(it->first);
							path.initialStates[robot].position.push_back(it->second);
							path.initialStates[robot].velocity.push_back(0.0);
							path.initialStates[robot].effort.push_back(0.0);							
						}
					}
				}
			}

			for (const pair<string, ptree> &n: p.second.get_child("nodes")) {
				if (n.first.compare("node") == 0) {
					path.points.push_back(n.second.get<string>("<xmlattr>.name"));
				}
			}
			paths.push_back(path);
		}
	}
}

void Test::parseConfigs(ptree &configsNode) {
	for (const pair<string, ptree> &c: configsNode) {
		if (c.first.compare("config") == 0) {
			CollisionAvoidance<ParticleCloud>::SParameters config;
			config.I_k = c.second.get<double>("<xmlattr>.I_k");
			config.k_a = c.second.get<double>("<xmlattr>.k_a");
			config.k_d = c.second.get<double>("<xmlattr>.k_d");
			config.filter = c.second.get<char>("<xmlattr>.filter");
			config.rotVector = c.second.get<char>("<xmlattr>.rotation_vector");
			config.surfFollowing = c.second.get<bool>("<xmlattr>.surfFollow");
			config.p = c.second.get<double>("<xmlattr>.p");
			config.m = c.second.get<bool>("<xmlattr>.m");
			config.margin = c.second.get<double>("<xmlattr>.margin");
			config.noise = c.second.get<double>("<xmlattr>.noise");
			configs.push_back(config);
		}
	}
}

void Test::run(NodeHandle& nh) {
	double timeStep = 1.0/rate;

	boost::shared_ptr<CollisionAvoidance<ParticleCloud>> core;
	if (ctrlType.compare("circular") == 0) {
		core = boost::shared_ptr<CollisionAvoidance<ParticleCloud>>(new CircularFields(nh));
	} else if(ctrlType.compare("modulation") == 0) {
		core = boost::shared_ptr<CollisionAvoidance<ParticleCloud>>(new DynamicSystemModulation(nh));
	} else {
		cerr << "Unknown controller type '" << ctrlType << "' aborting " << name << endl;
		return;
	}

	ros::Publisher planningScenePub = nh.advertise<moveit_msgs::PlanningScene>("/sim_planning_scene", 1);
	ros::Publisher trajectoryPub = nh.advertise<moveit_msgs::DisplayTrajectory>("/sim_trajectory", 1);

	core->objects = scene->getObjects();
	core->bDrawDebug = false;

	ofstream csv;
	csv.open ("Test_results/" + name + ".csv", ios::out);
	csv << "Path,Robot,I_k,k_a,k_d,Filter,Surface Following,Rotation Vector,P,M,Margin,Noise,Path Length,Time,avg. Velocity,Goal reached,Collision" << endl;

	ptree testNode;
	testNode.put("name", name);
	testNode.put("file", file);
	testNode.put("rate", rate);

	ptree configsNode;
	
	cout << "Starting test '" << name << "'" << endl
		 << "    Configurations : " << configs.size() << endl
		 << "    Paths          : " << paths.size() << endl;

	for (auto it = robots.begin(); it != robots.end() && ros::ok(); it++) {

		RobotWrapper& robot = it->second;
		SimulatorVelocityResolved sim;
		sim.init(*robot.urdf.get());
		const sensor_msgs::JointState& currentState = sim.getJointState();
		vector<size_t> jointIdx;
		VectorXd state = VectorXd::Zero(robot.controlledJoints.size() + 3);
		const giskard::Scope& scope = robot.controller.get_scope();
		const KDL::Expression<KDL::Vector>::Ptr posExpression = scope.find_vector_expression("robot_pos");
		robot.controller.start(state, 1000);
		
		PlanningSceneWrapper planningScene(robot.urdf, robot.srdf);

		moveit_msgs::DisplayTrajectory baseTrajMsg;
		baseTrajMsg.model_id = it->first;
		baseTrajMsg.trajectory.push_back(moveit_msgs::RobotTrajectory());
		baseTrajMsg.trajectory[0].joint_trajectory.joint_names = robot.controlledJoints;

		for (ObjectBase<ParticleCloud>* pObj: core->objects) {
			moveit_msgs::CollisionObject cObj;
			cObj.header.stamp = ros::Time::now();
			cObj.header.frame_id = "odom_combined";
			cObj.id = pObj->name;

			cObj.meshes.push_back(*(pObj->pPhys));
			cObj.mesh_poses.push_back(EigenToGeometrymsgs(pObj->getTransform()));

			cObj.operation = 0;

			planningScene.updateCollisionObject(cObj);
		}

		for (size_t i = 0; i < robot.controlledJoints.size(); i++)
		{
			size_t idx = sim.getJointIndex(robot.controlledJoints[i]);
			jointIdx.push_back(idx);
		}

		for(size_t k = 0; k < configs.size() && ros::ok(); k++) {
			CollisionAvoidance<ParticleCloud>::SParameters config = configs[k];
			cout << "Running configuration: " << endl
				 << "     Robot = " << it->first << endl
				 << "       I_k = " << config.I_k << endl
				 << "       k_a = " << config.k_a << endl
				 << "       k_d = " << config.k_d << endl
				 << "    filter = " << config.filter << endl
				 << "    follow = " << config.surfFollowing << endl;

			core->setParams(config);

			ptree configNode;
			configNode.put("robot", it->first);
			configNode.put("I_k", config.I_k);
			configNode.put("k_a", config.k_a);
			configNode.put("k_d", config.k_d);
			configNode.put("filter", config.filter);
			configNode.put("rotation_vector", config.rotVector);
			configNode.put("follow", config.surfFollowing);
			configNode.put("p", config.p);
			configNode.put("margin", config.margin);
			configNode.put("m", config.m);
			configNode.put("noise", config.noise);
			ptree pathsNode;

			std::ostringstream strs;
			strs << "(" << it->first 
			     << "," << config.I_k 
			     << "," << config.k_a 
			     << "," << config.k_d 
			     << "," << config.filter 
			     << "," << config.surfFollowing 
			     << "," << config.rotVector
			     << "," << config.p
			     << "," << config.m
			     << "," << config.margin
			     << "," << config.noise
			     << ")";
			string confName = strs.str();

			for (size_t n = 0; n < paths.size() && ros::ok(); n++) {
				SPath& p = paths[n];

				sim.setSubJointState(p.initialStates[it->first]);

				//moveit_msgs::DisplayTrajectory curTrajMsg = baseTrajMsg;
				//curTrajMsg.trajectory_start.joint_state = currentState;
				//curTrajMsg.trajectory[0].joint_trajectory.points.push_back(makeTrajectoryPoint(curTrajMsg.trajectory[0].joint_trajectory.joint_names, currentState, 0));

				planningScene.updateJointStates(currentState);
				moveit_msgs::PlanningScene msgPS;
				planningScene.getPlanningSceneMsg(msgPS);
				planningScenePub.publish(msgPS);
				ros::spinOnce();

				for (size_t i = 0; i < jointIdx.size(); i++) {
					state[i] = currentState.position[jointIdx[i]];
				}

				if (!robot.controller.start(state, rate)) {
				    cerr << "Giskard start failed while setting initial state! State: " << endl << state << endl;
					break;
				}

				SNamedPoint goalP;
				size_t nodeIdx = 0;
				
				if (!scene->getPoint(p.points[nodeIdx], goalP)) {
					cout << "Failed to get starting point: " << p.points[nodeIdx] << endl
						 << "Test: " << name << " Path: " << p.name << endl;
					break;
				}

				Vector3d pos = KDL2Eigen(posExpression->value());
				
//////////////////////////// OUTPUT /////////////////////////////////////////////
				ofstream o_path;
				string pathName = "paths/" + name + "_" + confName + "_" + p.name + ".path";
				o_path.open ("Test_results/" + pathName, ios::out | ios::binary);

				double time = 0;
				double pathLength = 0;

				cout << "   Trying path '" << p.name << "'" << endl;

				ptree pathNode;
				pathNode.put("name", p.name);

				ptree pointsNode;
/////////////////////////// ~OUTPUT /////////////////////////////////////////////

				bool collided = false;
				while(ros::ok() && nodeIdx < p.points.size() && !collided) {
					bool ok = scene->getPoint(p.points[nodeIdx], goalP);
					Vector3d goal = goalP.pos;

					cout << "Next goal: " << goalP.name << endl;
					printf("Position: (%f, %f, %f) \n", pos[0], pos[1], pos[2]);
					printf("    Goal: (%f, %f, %f) \n", goal[0], goal[1], goal[2]);

					Vector3d vel = Vector3d::Zero();
					while(ros::ok() && ok && time <= p.timeLimit && (pos-goal).norm() > 0.02 && !collided) {
						float buffer[4];
						buffer[0] = (float)pos[0];
						buffer[1] = (float)pos[1];
						buffer[2] = (float)pos[2];
						buffer[3] = (float)(vel.norm());
						o_path.write ((char*)&buffer, sizeof (buffer));

						Vector3d temp = KDL2Eigen(posExpression->value());
						double lastVel = (pos - temp).norm() * rate; 	
						pathLength += (pos - temp).norm();
						pos = temp;
						
						//printf("Desired Vel: %f \n", vel.norm());
						//printf("Actual  Vel: %f \n", lastVel);

						Vector3d newVel = core->calculateAvoidance(pos, goal, vel, timeStep);
						if (newVel[0] == newVel[0] && 
							newVel[1] == newVel[1] && 
							newVel[2] == newVel[2]) {
							vel = newVel;
							if (vel.norm() > 0) {
								Vector3d direction = vel.normalized();
								double magnitude = min(vel.norm(), 1.0);
								Vector3d velLimited = magnitude*direction;

								//printf("Vel Lim: |(%f, %f, %f)| = %f \n", velLimited[0], velLimited[1], velLimited[2], velLimited.norm());
								vel = velLimited;
							}
							
							state[robot.controlledJoints.size() + 0] = vel[0] * 2;
							state[robot.controlledJoints.size() + 1] = vel[1] * 2;
							state[robot.controlledJoints.size() + 2] = vel[2] * 2;

							
							if (!robot.controller.update(state, 1000)) {
							    cerr << "Giskard update failed! State: " << endl << state << endl;
								break;
							}
							Eigen::VectorXd commands = robot.controller.get_command();

							//Vector3d velCmd(commands[0], commands[1], commands[2]);
							//printf("Cmd Vel: |(%f, %f, %f)| = %f \n", velCmd[0], velCmd[1], velCmd[2], velCmd.norm());

							for (size_t i = 0; i < robot.controlledJoints.size(); i++) {
								sim.setNextJointVelocity(robot.controlledJoints[i], commands[i]);
							}

							sim.update(ros::Time::now(), timeStep);
							collided = planningScene.check_collisions(currentState);
							//currentState = sim.getJointState();

							//cout << "Sim:" << endl;
							for (size_t i = 0; i < jointIdx.size(); i++) {
								state[i] = currentState.position[jointIdx[i]];
								//cout << state[i] << endl;
							}

						}
						time += timeStep;
						//curTrajMsg.trajectory[0].joint_trajectory.points.push_back(makeTrajectoryPoint(curTrajMsg.trajectory[0].joint_trajectory.joint_names, currentState, time));
					}
					
					nodeIdx++;
				}

				//trajectoryPub.publish(curTrajMsg);
				ros::spinOnce();

				pathNode.put("points", pathName);
				
				o_path.close();

				bool goalReached = time <= p.timeLimit && !collided;

				csv << p.name 
					<< "," << it->first 
					<< "," <<  config.I_k 
					<< "," << config.k_a 
					<< "," << config.k_d 
					<< "," << config.filter 
					<< "," << config.surfFollowing
					<< "," << config.rotVector
					<< "," << config.p
					<< "," << config.m
					<< "," << config.margin
					<< "," << config.noise
					<< "," << pathLength 
					<< "," << time 
					<< "," << (pathLength / time) 
					<< "," << goalReached 
					<< "," << collided << endl;

				if (!goalReached) {
					if (collided)
						cout << "        FAILED: Robot collided" << endl;
					else
						cout << "        FAILED: Time expired!" << endl;
				} else {
					cout << "        SUCCESS: " << time << "s " << pathLength << "m " << (pathLength / time) << "m/s" << endl;
				}

				pathsNode.push_back(make_pair("", pathNode));
			}
	
			configNode.add_child("paths", pathsNode);
			configsNode.push_back(make_pair("", configNode));
		}

	}

	testNode.add_child("configs", configsNode);

	write_json("Test_results/" + name + ".json", testNode);
}



int main(int argc, char **argv)
{	
	if (argc < 2) {
		cout << "Path to testbench to load needed!" << endl;
		return 0;
	}
	cout << argv[1] << endl;

	ros::init(argc, argv, "magnetic_controller_autotest");
	NodeHandle nh;

	ptree pt;

	read_xml(argv[1], pt);

	MeshManager<ParticleCloud> meshManager;
	MeshManager<shape_msgs::Mesh> physMeshManager;
	vector<Test> tests;

	tf::TransformListener tfListener(nh);

	ptree &benchNode = pt.get_child("testbench");
	for (const pair<string, ptree> &t: benchNode) {
		ptree node = t.second;
		Test test;
		test.init(node, &meshManager, &physMeshManager, &tfListener);
		tests.push_back(test);
	}

	cout << tests.size() << " tests found." << endl;

	size_t testIdx = 0;
	while (testIdx < tests.size() && ros::ok()) {
		cout << "Running test number " << testIdx << "..." << endl;
		tests[testIdx].run(nh);
		testIdx++;
	}

	return 0;
}