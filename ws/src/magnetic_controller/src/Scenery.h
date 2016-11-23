#pragma once

#include "utils.h"
#include "MeshManager.h"
#include "Mesh.h"

struct SNamedPoint {
	string name;
	Vector3d pos;
};

template<class T>
class Scenery
{
public:
	Scenery(MeshManager<T>* _pMeshManager, tf::TransformListener* _pTfListener) 
		: pMeshManager(_pMeshManager)
		, pTfListener(_pTfListener) 
		  {};

	void loadScene(string filename) {
		points.clear();
		goals.clear();
		starts.clear();
		objects.clear();

		ptree pt;

		read_xml(filename, pt);

		ptree &sceneNode = pt.get_child("scene");

		for (const pair<string, ptree> &p: sceneNode) {
			ptree node = p.second;
			if (p.first.compare("obstacle") == 0) {
				loadObject(node);
			} else if (p.first.compare("goal") == 0) {
				SNamedPoint goal;
				goal.name = node.get<string>("<xmlattr>.name");
				goal.pos = parsePosition(node);
				goals.push_back(goal);
				points[goal.name] = goal;
			} else if (p.first.compare("start") == 0) {
				SNamedPoint start;
				start.name = node.get<string>("<xmlattr>.name");
				start.pos = parsePosition(node);
				starts.push_back(start);
				points[start.name] = start;
			}
		}
	};

	bool getPoint(string name, SNamedPoint &point) {
		auto it = points.find(name);
		if (it != points.end()) {
			point = it->second;
			return true;
		}

		return false;
	};

	bool getGoal(size_t idx, SNamedPoint &point) {
		if (goals.size() > idx) {
			point = goals[idx];
			return true;
		}

		return false;
	};

	bool getStart(size_t idx, SNamedPoint &point) {
		if (starts.size() > idx) {
			point = starts[idx];
			return true;
		}

		return false;
	};

	vector<ObjectBase<T>*> getObjects() {
		vector<ObjectBase<T>*> out;
		for (auto ptr: objects)
			out.push_back(ptr.get());
		return out;
	}

private:
	void loadObject(ptree &node) {
		string mesh = node.get<string>("<xmlattr>.mesh");
		string name = node.get<string>("<xmlattr>.name");
		bool stat = node.get<bool>("<xmlattr>.static");
		
		T *pMesh = pMeshManager->getMeshByName(mesh);
		if (!pMesh) {
			pMesh = pMeshManager->getNewMesh(mesh);
			loadFromFile(pMesh, mesh);
		}

		boost::shared_ptr<ObjectBase<T>> objectPtr;
		if (stat) {
			Affine3d transform = parseTransform(node.get_child("transform"));
			objectPtr = boost::shared_ptr<ObjectBase<T>>(new StaticObject<T>(pMesh, name, transform));
		} else {
			objectPtr = boost::shared_ptr<ObjectBase<T>>(new TFObject<T>(pMesh, name, pTfListener));
		}

		objects.push_back(objectPtr);
	}

	tf::TransformListener* pTfListener;

	MeshManager<T>* pMeshManager;
	unordered_map<string, SNamedPoint> points;
	vector<SNamedPoint> goals;
	vector<SNamedPoint> starts;

	vector<boost::shared_ptr<ObjectBase<T>>> objects;
};