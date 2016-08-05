#pragma once

#include <ros/ros.h>
#include <unordered_map>

using namespace std;

template <class T>
class MeshManager {
	struct SMeshMeta {
		string name;
		T mesh;
	};

public:
	T* getNewMesh(string name) {
		boost::shared_ptr<SMeshMeta> newMesh(new SMeshMeta());
		newMesh->name = name;
		meshes[&(newMesh->mesh)] = newMesh;
		return &(newMesh->mesh);
	}

	T* getMeshByName(string name) {
		for(auto it = meshes.begin(); it != meshes.end(); it++) {
			if (name.compare(it->second->name) == 0)
				return &(it->second->mesh);
		}
		return 0;
	}
	
	void removeMesh(string name) {
		for(auto it = meshes.begin(); it != meshes.end(); it++) {
			if (name.compare(it->second->name) == 0) {
				meshes.erase(it);
				break;
			}
		}
	}
	
	void removeMesh(T* pMesh) {
		meshes.erase(pMesh);
	}

private:
	unordered_map<T*, boost::shared_ptr<SMeshMeta>> meshes;
};
/*
template <class T>
T* MeshManager<T>::getNewMesh(string name) 

template <class T>
T* MeshManager<T>::getMeshByName(string name) 

template <class T>
void MeshManager<T>::removeMesh(string name) 

template <class T>
void MeshManager<T>::removeMesh(T* pMesh) 
//*/