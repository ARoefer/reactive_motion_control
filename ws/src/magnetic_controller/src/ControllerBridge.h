#pragma once

#include <ros/ros.h>
#include <eigen3/Eigen/Eigen>
#include <tf/transform_listener.h>
#include <tf_conversions/tf_eigen.h>
#include <string>

using namespace std;

struct ICommander {
	virtual void setVelocity(Eigen::Vector3d vel) = 0;
	virtual void setPosition(Eigen::Vector3d pos) = 0;
	virtual bool getCurrentEEFPose(Eigen::Affine3d &out) = 0;
};

class ROSCommander : public ICommander {
public:
	ROSCommander(ros::NodeHandle &nh);

	void setVelocity(Eigen::Vector3d vel);
	bool getCurrentEEFPose(Eigen::Affine3d &out);
	void setPosition(Eigen::Vector3d pos);

private:
	tf::TransformListener tfListener;
	ros::Publisher pubX, pubY, pubZ;
};

class GiskardCommander : public ICommander {
public:
	GiskardCommander(ros::NodeHandle &nh, string _refFrame, string _eefFrame);

	void setVelocity(Eigen::Vector3d vel);
	bool getCurrentEEFPose(Eigen::Affine3d &out);
	void setPosition(Eigen::Vector3d pos);

private:
	tf::TransformListener tfListener;
	ros::Publisher pub;

	string refFrame, eefFrame;
};


class SimCommander : public ICommander {
public:
	SimCommander(double step = 0.005);
	void setVelocity(Eigen::Vector3d vel);
	bool getCurrentEEFPose(Eigen::Affine3d &out);
	void setPosition(Eigen::Vector3d pos);

private:
	Eigen::Vector3d currentPos;
	double step;
};


class FakestCommander : public ICommander {
public:
	FakestCommander();
	FakestCommander(ros::NodeHandle &nh);

	void setVelocity(Eigen::Vector3d vel);
	bool getCurrentEEFPose(Eigen::Affine3d &out);
	void setPosition(Eigen::Vector3d pos);

private:
	void refreshPosition();

	Eigen::Vector3d currentPos;
	Eigen::Vector3d lastVelocity;
	ros::Time lastSet;
};