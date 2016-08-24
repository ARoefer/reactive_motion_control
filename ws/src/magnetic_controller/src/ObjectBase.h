#pragma once
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>

#include <eigen3/Eigen/Eigen>
#include <string>

using namespace std;
using namespace Eigen;

class Mesh;

template<class T>
class ObjectBase
{
public:
	ObjectBase(T* _pMesh, string _name, bool _statc)
	: pMesh(_pMesh)
	, name(_name)
	, statc(_statc)
	{}
	
	virtual Affine3d getTransform() const = 0;

	T* pMesh;
	const string name;
	const bool statc;
};

template<class T>
class TFObject : public ObjectBase<T>
{
public:
	TFObject(T* _pMesh, string _name, tf::TransformListener* _pTfListener)
	: ObjectBase<T>(_pMesh, _name, false)
	{
		pTfListener = _pTfListener;
	}
	
	Affine3d getTransform() const  {
			try {
			tf::StampedTransform temp;
			pTfListener->waitForTransform("odom_combined", this->name, ros::Time(0), ros::Duration(0.5));
			pTfListener->lookupTransform("odom_combined", this->name, ros::Time(0), temp);

			Affine3d out;
			tf::poseTFToEigen(temp, out);

			return out;
		} catch (tf::TransformException ex) {
			return Affine3d::Identity();
		}
	}

private:
	tf::TransformListener* pTfListener;
};

template<class T>
class StaticObject : public ObjectBase<T>
{
public:
	StaticObject(T* _pMesh, string _name, Affine3d _transform)
	: ObjectBase<T>(_pMesh, _name, true)
	{
		transform = _transform;
	}
	
	Affine3d getTransform() const  {
		return transform;
	}

private:
	Affine3d transform;
};