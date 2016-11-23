#include "ControllerBridge.h"

#include <iostream>
#include <std_msgs/Float64.h>
#include <geometry_msgs/PointStamped.h>

using namespace Eigen;

// ------------------------------------------- POINT ROBOT COMMANDER
ROSCommander::ROSCommander(ros::NodeHandle &nh) {
	pubX = nh.advertise<std_msgs::Float64>("/jointX_velocity_controller/command", 1);
	pubY = nh.advertise<std_msgs::Float64>("/jointY_velocity_controller/command", 1);
	pubZ = nh.advertise<std_msgs::Float64>("/jointZ_velocity_controller/command", 1);
}

void ROSCommander::setVelocity(Vector3d vel) {
	std_msgs::Float64 velX;
	velX.data = vel[0];
	std_msgs::Float64 velY;
	velY.data = vel[1];
	std_msgs::Float64 velZ;
	velZ.data = vel[2];

	pubX.publish(velX);
	pubY.publish(velY);
	pubZ.publish(velZ);//*/
}

void ROSCommander::setPosition(Eigen::Vector3d pos) {

}

bool ROSCommander::getCurrentEEFPose(Affine3d &out) {
	try {	
		tf::StampedTransform temp;
		tfListener.waitForTransform("odom_combined", "eef_link", ros::Time(0), ros::Duration(0.5));
		tfListener.lookupTransform("odom_combined", "eef_link", ros::Time(0), temp);

		tf::poseTFToEigen(temp, out);
		return true;
	} catch (tf::TransformException ex) {
		std::cerr << "Lookup of frame 'eef_link' failed!" << std::endl;
	}
	return false;
}


// ------------------------------------------- PR2 COMMANDER
PR2Commander::PR2Commander(ros::NodeHandle &nh) {
	pub = nh.advertise<geometry_msgs::PointStamped>("/goal", 1);
}

void PR2Commander::setVelocity(Vector3d vel) {
	try {	
		tf::StampedTransform temp;
		tfListener.waitForTransform("base_link", "odom_combined", ros::Time(0), ros::Duration(0.5));
		tfListener.lookupTransform("base_link", "odom_combined", ros::Time(0), temp);

		Affine3d oIb = Affine3d::Identity();
		tf::poseTFToEigen(temp, oIb);
		Vector3d velIb = oIb.rotation() * vel;

		geometry_msgs::PointStamped msg;
		msg.header.stamp = ros::Time::now();
		msg.header.frame_id = "base_link";
		msg.point.x = velIb[0];
		msg.point.y = velIb[1];
		msg.point.z = velIb[2];

		pub.publish(msg);
	} catch (tf::TransformException ex) {
		std::cerr << "Lookup of odom_combined in base_link failed!" << std::endl;
	}

}

void PR2Commander::setPosition(Eigen::Vector3d pos) {

}

bool PR2Commander::getCurrentEEFPose(Affine3d &out) {
	try {	
		tf::StampedTransform temp;
		tfListener.waitForTransform("odom_combined", "r_gripper_tool_frame", ros::Time(0), ros::Duration(0.5));
		tfListener.lookupTransform("odom_combined", "r_gripper_tool_frame", ros::Time(0), temp);

		tf::poseTFToEigen(temp, out);
		return true;
	} catch (tf::TransformException ex) {
		std::cerr << "Lookup of frame 'r_gripper_tool_frame' failed!" << std::endl;
	}
	return false;
}

// ------------------------------------------- SIM COMMANDER
SimCommander::SimCommander(double _step) {
	step = _step;
	currentPos = Vector3d::Zero();
}

void SimCommander::setVelocity(Vector3d vel) {
	// printf("Velocity: %f, %f, %f \n", vel[0],vel[1],vel[2]);
	// printf("currentPos: %f, %f, %f \n", currentPos[0],currentPos[1],currentPos[2]);
	// printf("step: %f \n", step);
	currentPos += vel * step;

	//printf("NewPos: %f, %f, %f \n", currentPos[0],currentPos[1],currentPos[2]);
}

void SimCommander::setPosition(Eigen::Vector3d pos) {
	currentPos = pos;
}

bool SimCommander::getCurrentEEFPose(Affine3d &out) {
	out = Affine3d(Translation3d(currentPos));
	return true;
}


// ------------------------------------------- FAKEST COMMANDER
FakestCommander::FakestCommander(ros::NodeHandle &nh) 
: FakestCommander()
{}

FakestCommander::FakestCommander() {
	lastSet = ros::Time::now();
}

void FakestCommander::refreshPosition() {
	ros::Time now = ros::Time::now();
	double deltaT = now.toSec() - lastSet.toSec();

	if (deltaT > 0) {
		currentPos += lastVelocity * deltaT;
	}

	lastSet = now;
}

void FakestCommander::setVelocity(Eigen::Vector3d vel) {
	refreshPosition();
	lastVelocity = vel;
}

void FakestCommander::setPosition(Eigen::Vector3d pos) {
	lastSet = ros::Time::now();
	currentPos = pos;
	lastVelocity = Vector3d::Zero();
}

bool FakestCommander::getCurrentEEFPose(Affine3d &out) {
	refreshPosition();
	out = Affine3d(Translation3d(currentPos));
	return true;
}