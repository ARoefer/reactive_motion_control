#pragma once

#include <eigen3/Eigen/Eigen>
#include <string>

using namespace std;
using namespace Eigen;

class Mesh;
namespace tf {
	class TransformListener;
}

class ObjectBase
{
public:
	ObjectBase(Mesh* _pMesh, string _name);
	
	virtual Affine3d getTransform() const = 0;

	Mesh* pMesh;
	const string name;

};

class TFObject : public ObjectBase
{
public:
	TFObject(Mesh* _pMesh, string _name, tf::TransformListener* _pTfListener);
	
	Affine3d getTransform() const;

private:
	tf::TransformListener* pTfListener;
};