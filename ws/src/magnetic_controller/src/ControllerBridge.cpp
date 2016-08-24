#include "ControllerBridge.h"

#include <iostream>
#include <std_msgs/Float64.h>

using namespace Eigen;


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

SimCommander::SimCommander(double _step) {
	step = _step;
	currentPos = Vector3d::Zero();
}

void SimCommander::setVelocity(Vector3d vel) {
	currentPos += vel * step;
}

void SimCommander::setPosition(Eigen::Vector3d pos) {
	currentPos = pos;
}

bool SimCommander::getCurrentEEFPose(Affine3d &out) {
	out = Affine3d(Translation3d(currentPos));
	return true;
}