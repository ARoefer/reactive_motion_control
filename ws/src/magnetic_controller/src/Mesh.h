#pragma once

#include <eigen3/Eigen/Eigen>
#include <vector>

using namespace std;
using namespace Eigen;

struct Surface {
	Surface(Vector3d _mid, Vector3d _normal, double _area)
		: mid(_mid)
		, normal(_normal)
		, area(_area) {}

	Vector3d mid;
	Vector3d normal;
	double area;
};

class Mesh
{
public:
	vector<Surface> surfaces;
};