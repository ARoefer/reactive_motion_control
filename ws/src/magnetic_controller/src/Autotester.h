#pragma once
#define SIM

#include "utils.h"

#include <boost/property_tree/json_parser.hpp>

#include "MeshManager.h"
#include "Scenery.h"

class Test {
public:
	Test() : rate(1) {};

	void init(ptree &root, MeshManager<ParticleCloud>* pMeshManager, tf::TransformListener* pTfListener);
	void run(const NodeHandle& _nh);

private:
	struct SPath {
		vector<string> points;
		double timeLimit;
		string name;
	};

	void parsePaths(ptree &pathsNode);
	void parseConfigs(ptree &configsNode);

	string name;
	string file;
	double rate;
	vector<SPath> paths;
	vector<MagneticCore<ParticleCloud>::SParameters> configs;

	boost::shared_ptr<Scenery<ParticleCloud>> scene;
};