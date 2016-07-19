#pragma once


#include <ros/ros.h>
#include <eigen3/Eigen/Eigen>

#include <vector>

#include <r_libs/VisualizationManager.h>

class ObjectBase;

using namespace std;
using namespace Eigen;
using namespace ros;

class MagneticCore
{
	enum ENameSpaces
	{
		eCurrent,
		eMath,
		eMAX
	};

	string namespaces[eMAX] = {"current", "math"};

	struct SParameters {
		SParameters() 
		: vel(0.2)
		, mass(1)
		, I_k(1)
		, k_a(1)
		, k_d(1)
		{}


		void refresh() {
			ros::param::get("vel", vel);
			ros::param::get("mass", mass);
			ros::param::get("I_k", I_k);
			ros::param::get("k_a", k_a);
			ros::param::get("k_d", k_d);
		}

		double vel;
		double mass;
		double I_k;
		double k_a;
		double k_d;
	} parameters;

public:
	MagneticCore(const NodeHandle &_nh);

	vector<ObjectBase*> objects;

	Vector3d calculateAccel(Vector3d pos, Vector3d goal, Vector3d velV);

	struct 
	{
		Vector3d F_v;
		double B;
	} debug;

	void refreshParams() { parameters.refresh(); }

private:
	NodeHandle nh;
	VisualizationManager visManager;
	Publisher visPub;

};