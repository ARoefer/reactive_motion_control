#include "Autotester.h"
#include "ControllerBridge.h"

#include <sstream>

void Test::init(ptree &root, MeshManager<ParticleCloud>* _pMeshManager, tf::TransformListener* pTfListener) {
	name = root.get<string>("<xmlattr>.name");
	file = root.get<string>("<xmlattr>.file");
	rate = root.get<double>("<xmlattr>.rate");
	scene = boost::shared_ptr<Scenery<ParticleCloud>>(new Scenery<ParticleCloud>(_pMeshManager, pTfListener));
	scene->loadScene(file);

	ptree &pathsNode = root.get_child("paths");
	ptree &configsNode = root.get_child("configs");

	parsePaths(pathsNode);
	parseConfigs(configsNode);
}

void Test::parsePaths(ptree &pathsNode) {
	for (const pair<string, ptree> &p: pathsNode) {
		if (p.first.compare("path") == 0) {
			SPath path;
			path.name = p.second.get<string>("<xmlattr>.name");
			path.timeLimit = p.second.get<double>("<xmlattr>.maxTime");
			for (const pair<string, ptree> &n: p.second) {
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
			MagneticCore<ParticleCloud>::SParameters config;
			config.I_k = c.second.get<double>("<xmlattr>.I_k");
			config.k_a = c.second.get<double>("<xmlattr>.k_a");
			config.k_d = c.second.get<double>("<xmlattr>.k_d");
			config.filter = c.second.get<char>("<xmlattr>.filter");
			config.surfFollowing = c.second.get<bool>("<xmlattr>.surfFollow");
			configs.push_back(config);
		}
	}
}

void Test::run(const NodeHandle& nh) {
	double timeStep = 1.0/rate;

	MagneticCore_ParticleModel core(nh);
	core.objects = scene->getObjects();
	core.bDrawDebug = false;

	SimCommander commander(timeStep);

	ofstream csv;
	csv.open ("Test_results/" + name + ".csv", ios::out);
	csv << "Path,I_k,k_a,k_d,Filter,Surface Following,Path Length,Time,avg. Velocity,Goal reached,Collision" << endl;

	ptree testNode;
	testNode.put("name", name);
	testNode.put("file", file);
	testNode.put("rate", rate);

	ptree configsNode;
	
	cout << "Starting test '" << name << "'" << endl
		 << "    Configurations : " << configs.size() << endl
		 << "    Paths          : " << paths.size() << endl;

	for(MagneticCore<ParticleCloud>::SParameters config: configs) {
		cout << "Running configuration: " << endl
			 << "       I_k = " << config.I_k << endl
			 << "       k_a = " << config.k_a << endl
			 << "       k_d = " << config.k_d << endl
			 << "    filter = " << config.filter << endl
			 << "    follow = " << config.surfFollowing << endl;

		core.setParams(config);
		commander.setVelocity(Vector3d::Zero());
		commander.setPosition(Vector3d::Zero());

		ptree configNode;
		configNode.put("I_k", config.I_k);
		configNode.put("k_a", config.k_a);
		configNode.put("k_d", config.k_d);
		configNode.put("filter", config.filter);
		configNode.put("follow", config.surfFollowing);
		ptree pathsNode;

		std::ostringstream strs;
		strs << "(" << config.I_k << "," << config.k_a << "," << config.k_d << "," << config.filter << "," << config.surfFollowing << ")_";
		string confName = strs.str();

		for (SPath p: paths) {
			ofstream o_path;
			string pathName = name + "_" + confName + "_" + p.name + ".path";
			o_path.open ("Test_results/" + pathName, ios::out | ios::binary);

			double time = 0;
			double pathLength = 0;
			size_t nodeIdx = 1;
			SNamedPoint start, goalP;

			cout << "   Trying path '" << p.name << "'" << endl;

			ptree pathNode;
			pathNode.put("name", p.name);

			ptree pointsNode;

			if (scene->getPoint(p.points[0], start)) {
				commander.setPosition(start.pos);
				Vector3d pos = start.pos;

				while(ros::ok() && nodeIdx < p.points.size()) {
					bool ok = scene->getPoint(p.points[nodeIdx], goalP);
					Vector3d goal = goalP.pos;

					Vector3d vel = Vector3d::Zero();
					while(ros::ok() && ok && time <= p.timeLimit && (pos-goal).norm() > 0.02) {
						float buffer[4];
						buffer[0] = (float)pos[0];
						buffer[1] = (float)pos[1];
						buffer[2] = (float)pos[2];
						buffer[3] = (float)(vel.norm());
						o_path.write ((char*)&buffer, sizeof (buffer));

						Affine3d eef = Affine3d::Identity();
						commander.getCurrentEEFPose(eef);
						Vector3d temp = eef.translation();
						pathLength += (pos - temp).norm();
						pos = temp;

						Vector3d newVel = core.calculateAccel(pos, goal, vel);
						if (newVel[0] == newVel[0] && 
							newVel[1] == newVel[1] && 
							newVel[2] == newVel[2]) {
							vel = newVel;
							Vector3d direction = vel.normalized();
							double magnitude = min(vel.norm(), 1.0);
							Vector3d velLimited = magnitude*direction;
							commander.setVelocity(velLimited);
							vel = velLimited;
						}
						time += timeStep;
					}
					
					commander.setVelocity(Vector3d::Zero());
					nodeIdx++;
				}
				pathNode.put("points", pathName);
			}
			bool goalReached = time <= p.timeLimit;

			csv << p.name << "," <<  config.I_k << "," << config.k_a << "," << config.k_d 
				<< "," << config.filter << "," << config.surfFollowing << "," 
				<< pathLength << "," << time << "," << (pathLength / time) << ","
				<< goalReached << "," << endl;

			if (!goalReached)
				cout << "        FAILED: Time expired!" << endl;
			else
				cout << "        SUCCESS: " << time << "s " << pathLength << "m " << (pathLength / time) << "m/s" << endl;

			pathsNode.push_back(make_pair("", pathNode));
		}

		configNode.add_child("paths", pathsNode);
		configsNode.push_back(make_pair("", configNode));
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
	vector<Test> tests;

	tf::TransformListener tfListener(nh);

	ptree &benchNode = pt.get_child("testbench");
	for (const pair<string, ptree> &t: benchNode) {
		ptree node = t.second;
		Test test;
		test.init(node, &meshManager, &tfListener);
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