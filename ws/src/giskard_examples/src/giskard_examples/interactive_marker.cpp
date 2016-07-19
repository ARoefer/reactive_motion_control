/*
* Copyright (C) 2016 Georg Bartels <georg.bartels@cs.uni-bremen.de>
*
*
* This file is part of giskard_examples.
*
* giskard_examples is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2 * of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <ros/ros.h>
#include <interactive_markers/interactive_marker_server.h>
#include <giskard_msgs/WholeBodyPositionGoal.h>
#include <memory>
#include <boost/bind.hpp>

class BodyPartSemantics
{
  public:
    BodyPartSemantics(const std::string& name, const std::string& frame_id) :
      name_( name ), frame_id_( frame_id ) {}
    ~BodyPartSemantics() {}

    const std::string& get_name() const
    {
      return name_;
    }

    void set_name(const std::string& name)
    {
      name_ = name;
    }

    const std::string& get_frame_id() const
    {
      return frame_id_;
    }

    void set_frame_id(const std::string& frame_id)
    {
      frame_id_ = frame_id;
    }

  private:
    std::string name_, frame_id_;
};

geometry_msgs::PoseStamped create_identity_goal(const BodyPartSemantics& bodypart)
{
  geometry_msgs::PoseStamped goal;
  goal.header.stamp = ros::Time::now();
  goal.header.frame_id = bodypart.get_frame_id();
  goal.pose.orientation.w = 1.0;
  return goal;
}

class WholeBodyInteractiveMarkers
{
  public:
    WholeBodyInteractiveMarkers(const ros::NodeHandle& nh, const BodyPartSemantics& left_ee_semantics,
        const BodyPartSemantics& right_ee_semantics) :
        nh_( nh ), left_ee_semantics_( left_ee_semantics ), right_ee_semantics_( right_ee_semantics )
    {
      goal_.left_ee_goal = create_identity_goal(left_ee_semantics_);
      goal_.right_ee_goal = create_identity_goal(right_ee_semantics_);
    }

    void start()
    {
      goal_pub_ = nh_.advertise<giskard_msgs::WholeBodyPositionGoal>("goal_out", 1);

      server_ = std::make_shared<interactive_markers::InteractiveMarkerServer>
          (nh_.getNamespace(), "", false);

      visualization_msgs::InteractiveMarker marker =
          create_6dof_marker(left_ee_semantics_, 0.2);
      server_->insert(marker);
      server_->setCallback(marker.name,
          boost::bind(&WholeBodyInteractiveMarkers::feedback_callback, this, _1));

      marker = create_6dof_marker(right_ee_semantics_, 0.2);
      server_->insert(marker);
      server_->setCallback(marker.name,
          boost::bind(&WholeBodyInteractiveMarkers::feedback_callback, this, _1));

      server_->applyChanges();
    }
  
    ~WholeBodyInteractiveMarkers() {}

  private:
    ros::NodeHandle nh_;
    ros::Publisher goal_pub_;
    giskard_msgs::WholeBodyPositionGoal goal_;
    std::shared_ptr<interactive_markers::InteractiveMarkerServer> server_;
    BodyPartSemantics left_ee_semantics_, right_ee_semantics_;

    void feedback_callback(const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback)
    {
      if (feedback->event_type ==
          visualization_msgs::InteractiveMarkerFeedback::MOUSE_UP)
      {
        geometry_msgs::Pose identity_pose;
        identity_pose.orientation.w = 1.0;
        server_->setPose( feedback->marker_name, identity_pose); 
        server_->applyChanges();

        if(feedback->marker_name.compare(left_ee_semantics_.get_name()) == 0)
        {
          goal_.left_ee_goal.header = feedback->header;
          goal_.left_ee_goal.pose = feedback->pose;
        }
        else if (feedback->marker_name.compare(right_ee_semantics_.get_name()) == 0)
        {
          goal_.right_ee_goal.header = feedback->header;
          goal_.right_ee_goal.pose = feedback->pose;
        }
        else
        {
          ROS_ERROR("Could not associate marker with name '%s' to any bodypart.", feedback->marker_name.c_str());
          return;
        }

        goal_pub_.publish(goal_);
      }
    }

    visualization_msgs::InteractiveMarker create_6dof_marker(const BodyPartSemantics& bodypart, 
        double marker_scale) const
    {
      visualization_msgs::InteractiveMarker result;
      result.name = bodypart.get_name();
      result.description = bodypart.get_name();
      result.header.frame_id = bodypart.get_frame_id();
      result.scale = marker_scale;

      visualization_msgs::InteractiveMarkerControl control;
      control.always_visible = true;
      control.orientation.w = 1;
      control.orientation.x = 1;
      control.orientation.y = 0;
      control.orientation.z = 0;
      control.name = "rotate_x";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
      result.controls.push_back(control);
      control.name = "move_x";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
      result.controls.push_back(control);

      control.orientation.w = 1;
      control.orientation.x = 0;
      control.orientation.y = 1;
      control.orientation.z = 0;
      control.name = "rotate_z";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
      result.controls.push_back(control);
      control.name = "move_z";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
      result.controls.push_back(control);

      control.orientation.w = 1;
      control.orientation.x = 0;
      control.orientation.y = 0;
      control.orientation.z = 1;
      control.name = "rotate_y";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::ROTATE_AXIS;
      result.controls.push_back(control);
      control.name = "move_y";
      control.interaction_mode = visualization_msgs::InteractiveMarkerControl::MOVE_AXIS;
      result.controls.push_back(control);

      return result;
    }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "interactive_marker");
  ros::NodeHandle nh("~");

  std::string left_ee_frame_id;
  if (!nh.getParam("left_ee_frame_id", left_ee_frame_id))
  {
    ROS_ERROR("Parameter 'left_ee_frame_id' not found in namespace '%s'.", nh.getNamespace().c_str());
    return 0;
  }

  std::string left_ee_name;
  if (!nh.getParam("left_ee_name", left_ee_name))
  {
    ROS_ERROR("Parameter 'left_ee_name' not found in namespace '%s'.", nh.getNamespace().c_str());
    return 0;
  }

  std::string right_ee_frame_id;
  if (!nh.getParam("right_ee_frame_id", right_ee_frame_id))
  {
    ROS_ERROR("Parameter 'right_ee_frame_id' not found in namespace '%s'.", nh.getNamespace().c_str());
    return 0;
  }

  std::string right_ee_name;
  if (!nh.getParam("right_ee_name", right_ee_name))
  {
    ROS_ERROR("Parameter 'right_ee_name' not found in namespace '%s'.", nh.getNamespace().c_str());
    return 0;
  }

  WholeBodyInteractiveMarkers wbim(nh, BodyPartSemantics(left_ee_name, left_ee_frame_id),
      BodyPartSemantics(right_ee_name, right_ee_frame_id));
  wbim.start();

  ros::spin();
}
