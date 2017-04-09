#pragma once


#include <ros/ros.h>
#include <eigen3/Eigen/Eigen>

#include <vector>

#include <r_libs/VisualizationManager.h>

template<class A>
class ObjectBase;

class Mesh;
class ParticleCloud;

using namespace std;
using namespace Eigen;
using namespace ros;

template<class T>
class CollisionAvoidance
{
protected:
	enum ENameSpaces
	{
		eCurrent,
		eMath,
		eMAX
	};

	vector<string> namespaces;

public:
	struct SParameters {
		SParameters() 
		: vel(0.2)
		, mass(1)
		, I_k(2)
		, k_a(1)
		, k_d(1)
		, filter('n')
		, m(false)
		, surfFollowing(false)
		, p(1)
		, margin(0.05)
		, noise(0.0)
		, rotVector('a')
		{}


		void refresh() {
			ros::param::get("vel", vel);
			ros::param::get("mass", mass);
			ros::param::get("I_k", I_k);
			ros::param::get("k_a", k_a);
			ros::param::get("k_d", k_d);
			string fTemp = &filter;
			ros::param::get("filter", fTemp);
			if (fTemp.size() > 0)
				filter = fTemp[0];
			string rTemp = &rotVector;
			ros::param::get("rotation_vector", rTemp);
			if (rTemp.size() > 0)
				rotVector = rTemp[0];
			ros::param::get("surface_following", surfFollowing);
			ros::param::get("p", p);
			ros::param::get("margin", margin);
			ros::param::get("m", m);
			ros::param::get("noise", noise);
		}

		bool m;
		bool surfFollowing;
		// 'n', 'd', 'g', 'a'
		char filter;
		// 'a', 'x', 'l'
		char rotVector;
		double vel;
		double mass;
		double I_k;
		double k_a;
		double k_d;
		double p;
		double margin;
		double noise;
	};

	CollisionAvoidance(const NodeHandle &_nh);


	virtual Vector3d calculateAvoidance(Vector3d pos, Vector3d goal, Vector3d velV, double dT) = 0;
	virtual void addObject(ObjectBase<T>* pObj) {
		if (!pObj)
			return;

		objects.push_back(pObj);
	}

	struct 
	{
		Vector3d F_v;
		double B;
		Matrix3d M;
		vector<Matrix3d> V;
		vector<Matrix3d> Vi;
		vector<Matrix3d> E;
		unsigned long particlesExamined;
		unsigned int objectsExamined;
	} debug;

	void refreshParams() { parameters.refresh(); }
	void setParams(SParameters params) { parameters = params; }

	bool bDrawDebug;
	vector<ObjectBase<T>*> objects;
protected:
	SParameters parameters;
	size_t pointCount;
	NodeHandle nh;
	VisualizationManager visManager;
	Publisher visPub;

};

class CircularFields : public CollisionAvoidance<ParticleCloud>
{
public:
	CircularFields(const NodeHandle &_nh);

	Vector3d calculateAvoidance(Vector3d pos, Vector3d goal, Vector3d velV, double dT);
	void addObject(ObjectBase<ParticleCloud>* pObj);
};

class DynamicSystemModulation : public CollisionAvoidance<ParticleCloud>
{
public:
	DynamicSystemModulation(const NodeHandle &_nh);

	Vector3d calculateAvoidance(Vector3d pos, Vector3d goal, Vector3d velV, double dT);
};