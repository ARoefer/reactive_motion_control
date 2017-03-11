#pragma once
#define SIM

#include "utils.h"

#include <boost/property_tree/json_parser.hpp>
#include <giskard/giskard.hpp>
#include <urdf/model.h>
#include <moveit/planning_scene/planning_scene.h>

#include <map>

#include "MeshManager.h"
#include "Scenery.h"

struct RobotWrapper {
	boost::shared_ptr<urdf::Model> urdf;
	boost::shared_ptr<srdf::Model> srdf;
	vector<string> controlledJoints;
	giskard::QPController controller;
};

class Test {
public:
	Test() : rate(1) {};

	void init(ptree &root, MeshManager<ParticleCloud>* pMeshManager, MeshManager<shape_msgs::Mesh>* _pPhysMeshManager, tf::TransformListener* pTfListener);
	void run(NodeHandle& _nh);

private:
	struct SPath {
		vector<string> points;
		double timeLimit;
		string name;
		unordered_map<string, sensor_msgs::JointState> initialStates;
	};

	void parseRobots(ptree &robotsNode);
	void parsePaths(ptree &pathsNode);
	void parseConfigs(ptree &configsNode);

	string name;
	string ctrlType;
	string file;
	double rate;
	vector<SPath> paths;
	vector<CollisionAvoidance<ParticleCloud>::SParameters> configs;

	boost::shared_ptr<Scenery<ParticleCloud>> scene;

	unordered_map<string, RobotWrapper> robots;
};

class PlanningSceneWrapper {
	planning_scene::PlanningScenePtr ps_ptr;

public:
	PlanningSceneWrapper(boost::shared_ptr<const urdf::Model> urdf_ptr, boost::shared_ptr<const srdf::Model> srdf_ptr) {
		ps_ptr = planning_scene::PlanningScenePtr(new planning_scene::PlanningScene(urdf_ptr, srdf_ptr));
	}


	void updateJointStates(const sensor_msgs::JointState& joint_states)
	{
		moveit_msgs::PlanningScene diff_ps_msg;
		diff_ps_msg.is_diff = true;
		diff_ps_msg.robot_state.joint_state = joint_states;
		ps_ptr->setPlanningSceneDiffMsg(diff_ps_msg);
	}

	void updateCollisionObject(const moveit_msgs::CollisionObject& object) {
		ps_ptr->processCollisionObjectMsg(object);
		ROS_INFO("Update object '%s' in planning scene done.", object.id.c_str());
	}

	bool check_collisions(const sensor_msgs::JointState& joint_states)
	{
		updateJointStates(joint_states);

		collision_detection::CollisionRequest collision_request;
		collision_detection::CollisionResult collision_result;
		collision_request.contacts = true;
		collision_request.max_contacts = 1;
		collision_result.clear();
		ps_ptr->checkCollision(collision_request, collision_result);

		return collision_result.collision;
	}

	void getPlanningSceneMsg(moveit_msgs::PlanningScene& msg) {
		ps_ptr->getPlanningSceneMsg(msg);
	}
};