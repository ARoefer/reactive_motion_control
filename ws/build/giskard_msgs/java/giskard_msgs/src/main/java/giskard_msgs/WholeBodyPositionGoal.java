package giskard_msgs;

public interface WholeBodyPositionGoal extends org.ros.internal.message.Message {
  static final java.lang.String _TYPE = "giskard_msgs/WholeBodyPositionGoal";
  static final java.lang.String _DEFINITION = "geometry_msgs/PoseStamped left_ee_goal\ngeometry_msgs/PoseStamped right_ee_goal\n";
  geometry_msgs.PoseStamped getLeftEeGoal();
  void setLeftEeGoal(geometry_msgs.PoseStamped value);
  geometry_msgs.PoseStamped getRightEeGoal();
  void setRightEeGoal(geometry_msgs.PoseStamped value);
}
