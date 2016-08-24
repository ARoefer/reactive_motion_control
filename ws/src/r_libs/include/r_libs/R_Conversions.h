#pragma once
#include <eigen3/Eigen/Eigen>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Transform.h>
#include <geometry_msgs/Point.h>
#include <tf/LinearMath/Transform.h>

#include <string>

geometry_msgs::PoseStamped EigenToGeometrymsgsStamped(Eigen::Affine3d poseEigen, std::string frame_id);
geometry_msgs::Pose EigenToGeometrymsgs(Eigen::Affine3d poseEigen);
geometry_msgs::Transform EigenToGeometrymsgsTF(Eigen::Affine3f poseEigen);

Eigen::Affine3d GeometrymsgsToEigenD(geometry_msgs::Pose pose);
Eigen::Matrix4f GeometrymsgsTFToEigenM44F(geometry_msgs::Transform pose);
Eigen::Affine3d GeometrymsgsTFToEigen3d(geometry_msgs::Transform pose);

Eigen::Vector3d PointToVector(geometry_msgs::Point point);
geometry_msgs::Point VectorToPoint(Eigen::Vector3d vec);

tf::Transform EigenToTfTransform(Eigen::Affine3d eTransform);