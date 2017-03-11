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
	Scenery(MeshManager<T>* _pMeshManager, MeshManager<shape_msgs::Mesh>* _pPhysMeshManager, tf::TransformListener* _pTfListener) 
		: pMeshManager(_pMeshManager)
		, pPhysMeshManager(_pPhysMeshManager)
		, pTfListener(_pTfListener) 
		  {};

	void loadScene(string filename) {
		points.clear();
		goals.clear();
		starts.clear();
		objects.clear();

		ptree pt;

		read_xml(filename, pt);
		string sceneDir = "";
		size_t dirSep = filename.rfind('/');
		if (dirSep != string::npos)
			sceneDir = filename.substr(0, dirSep + 1);

		cout << "Scene base directory: " << sceneDir << endl;
		ptree &sceneNode = pt.get_child("scene");

		for (const pair<string, ptree> &p: sceneNode) {
			ptree node = p.second;
			if (p.first.compare("obstacle") == 0) {
				loadObject(node, sceneDir);
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

	void getPointNames(vector<string>& names) const {
		for (auto it = points.begin(); it != points.end(); it++) {
			names.push_back(it->first);
		}
	}

	bool getPoint(const string& name, SNamedPoint &point) const {
		auto it = points.find(name);
		if (it != points.end()) {
			point = it->second;
			return true;
		}

		return false;
	};

	bool getGoal(size_t idx, SNamedPoint &point) const {
		if (goals.size() > idx) {
			point = goals[idx];
			return true;
		}

		return false;
	};

	bool getStart(size_t idx, SNamedPoint &point) const {
		if (starts.size() > idx) {
			point = starts[idx];
			return true;
		}

		return false;
	};

	vector<ObjectBase<T>*> getObjects() const {
		vector<ObjectBase<T>*> out;
		for (auto ptr: objects)
			out.push_back(ptr.get());
		return out;
	}

private:
	void loadObject(ptree &node, const string& baseDir) {
		string mesh = node.get<string>("<xmlattr>.mesh");
		string name = node.get<string>("<xmlattr>.name");
		bool stat = node.get<bool>("<xmlattr>.static");
		
		T *pMesh = pMeshManager->getMeshByName(mesh);
		shape_msgs::Mesh* pPhys = pPhysMeshManager->getMeshByName(mesh);
		if (!pMesh) {
			pMesh = pMeshManager->getNewMesh(mesh);
			loadFromFile(pMesh, baseDir + mesh);

			string physName = baseDir + mesh.substr(0, mesh.rfind('.')) + "_phys.obj";
			pPhys = pPhysMeshManager->getNewMesh(mesh);
			loadFromFile(pPhys, physName);
		}

		boost::shared_ptr<ObjectBase<T>> objectPtr;
		if (stat) {
			Affine3d transform = parseTransform(node.get_child("transform"));
			objectPtr = boost::shared_ptr<ObjectBase<T>>(new StaticObject<T>(pMesh, pPhys, name, transform));
		} else {
			objectPtr = boost::shared_ptr<ObjectBase<T>>(new TFObject<T>(pMesh, pPhys, name, pTfListener));
		}

		objects.push_back(objectPtr);
	}

	tf::TransformListener* pTfListener;

	MeshManager<T>* pMeshManager;
	MeshManager<shape_msgs::Mesh>* pPhysMeshManager;
	unordered_map<string, SNamedPoint> points;
	vector<SNamedPoint> goals;
	vector<SNamedPoint> starts;

	vector<boost::shared_ptr<ObjectBase<T>>> objects;
};