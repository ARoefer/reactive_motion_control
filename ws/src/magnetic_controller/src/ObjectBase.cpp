#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>

#include "ObjectBase.h"


ObjectBase::ObjectBase(Mesh* _pMesh, string _name)
	: pMesh(_pMesh)
	, name(_name)
	{}

TFObject::TFObject(Mesh* _pMesh, string _name, tf::TransformListener* _pTfListener) 
	: ObjectBase(_pMesh, _name)
{
	pTfListener = _pTfListener;
}

Affine3d TFObject::getTransform() const {
	try {
		tf::StampedTransform temp;
		pTfListener->waitForTransform("odom_combined", name, ros::Time(0), ros::Duration(0.5));
		pTfListener->lookupTransform("odom_combined", name, ros::Time(0), temp);

		Affine3d out;
		tf::poseTFToEigen(temp, out);

		return out;
	} catch (tf::TransformException ex) {
		return Affine3d::Identity();
	}
}