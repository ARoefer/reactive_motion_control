#include <tf_conversions/tf_eigen.h>
#include "r_libs/R_Conversions.h"

geometry_msgs::PoseStamped EigenToGeometrymsgsStamped(Eigen::Affine3d poseEigen, std::string frame_id) {
  geometry_msgs::PoseStamped out;
  out.header.stamp = ros::Time::now();
  out.header.frame_id = frame_id;
  out.pose = EigenToGeometrymsgs(poseEigen);

  return out;
}

geometry_msgs::Pose EigenToGeometrymsgs(Eigen::Affine3d poseEigen){
  geometry_msgs::Pose pose;
  Eigen::Vector3d translation = poseEigen.translation();
  Eigen::Quaterniond rotation(poseEigen.rotation());
  pose.orientation.w = rotation.w();
  pose.orientation.x = rotation.x();
  pose.orientation.y = rotation.y();
  pose.orientation.z = rotation.z();
  pose.position.x = translation[0];
  pose.position.y = translation[1];
  pose.position.z = translation[2];
  return pose;
}

geometry_msgs::Transform EigenToGeometrymsgsTF(Eigen::Affine3f poseEigen){
  geometry_msgs::Transform pose;
  Eigen::Vector3f translation = poseEigen.translation();
  Eigen::Quaternionf rotation(poseEigen.rotation());
  pose.rotation.w = rotation.w();
  pose.rotation.x = rotation.x();
  pose.rotation.y = rotation.y();
  pose.rotation.z = rotation.z();
  pose.translation.x = translation[0];
  pose.translation.y = translation[1];
  pose.translation.z = translation[2];
  return pose;
}

Eigen::Affine3d GeometrymsgsToEigenD(geometry_msgs::Pose pose){
  Eigen::Affine3d poseEigen; /* = Eigen::Affine3d::Identity();
  Eigen::Quaterniond quat(pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w);
  
  Eigen::Vector3d loc(pose.position.x, pose.position.y, pose.position.z);
  
  poseEigen.rotate(quat);
  poseEigen.translate(loc); //*/
  tf::Pose temp;
  tf::poseMsgToTF(pose, temp);
  tf::transformTFToEigen(temp, poseEigen);

  return poseEigen;
}

Eigen::Affine3d GeometrymsgsTFToEigen3d(geometry_msgs::Transform pose) {
  Eigen::Affine3d out;

  out.translate(Eigen::Vector3d(pose.translation.x, pose.translation.y, pose.translation.z));
  out.rotate(Eigen::Quaterniond(pose.rotation.x, pose.rotation.y, pose.rotation.z, pose.rotation.w));

  return out;
}

Eigen::Matrix4f GeometrymsgsTFToEigenM44F (geometry_msgs::Transform pose){
  Eigen::Affine3f poseEigen = Eigen::Affine3f::Identity();
  Eigen::Quaternionf quat(pose.rotation.x, pose.rotation.y, pose.rotation.z, pose.rotation.w);
  
  Eigen::Vector3f loc(pose.translation.x, pose.translation.y, pose.translation.z);
  
  poseEigen.rotate(quat);
  poseEigen.translate(loc);
  
  return Eigen::Matrix4f(poseEigen.matrix());
}

Eigen::Vector3d PointToVector(geometry_msgs::Point point) {
  return Eigen::Vector3d(point.x, point.y, point.z);
}

geometry_msgs::Point VectorToPoint(Eigen::Vector3d vec) {
  geometry_msgs::Point out;
  out.x = vec[0];
  out.y = vec[1];
  out.z = vec[2];
  return out;
}

tf::Transform EigenToTfTransform(Eigen::Affine3d eTransform) {
  tf::Transform out;

  tf::transformEigenToTF(eTransform, out);

  return out;
}