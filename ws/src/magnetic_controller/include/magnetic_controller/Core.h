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
class MagneticCore
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
		, surfFollowing(false)
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
			ros::param::get("surface_following", surfFollowing);
		}

		// 'n', 'd', 'g', 'a'
		char filter;
		bool surfFollowing;
		double vel;
		double mass;
		double I_k;
		double k_a;
		double k_d;
	};

	MagneticCore(const NodeHandle &_nh);


	virtual Vector3d calculateAccel(Vector3d pos, Vector3d goal, Vector3d velV) = 0;
	virtual void addObject(ObjectBase<T>* pObj) {
		if (!pObj)
			return;

		objects.push_back(pObj);
	}

	struct 
	{
		Vector3d F_v;
		double B;
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

class MagneticCore_ParticleModel : public MagneticCore<ParticleCloud>
{
public:
	MagneticCore_ParticleModel(const NodeHandle &_nh);

	Vector3d calculateAccel(Vector3d pos, Vector3d goal, Vector3d velV);
	void addObject(ObjectBase<ParticleCloud>* pObj);
};

class MagneticCore_SurfaceModel : public MagneticCore<Mesh>
{
public:
	MagneticCore_SurfaceModel(const NodeHandle &_nh);

	Vector3d calculateAccel(Vector3d pos, Vector3d goal, Vector3d velV);
};