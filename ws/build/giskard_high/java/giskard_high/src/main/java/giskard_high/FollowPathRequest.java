package giskard_high;

public interface FollowPathRequest extends org.ros.internal.message.Message {
  static final java.lang.String _TYPE = "giskard_high/FollowPathRequest";
  static final java.lang.String _DEFINITION = "geometry_msgs/PoseArray path\nsuturo_std_msgs/BodyGroup group\n";
  geometry_msgs.PoseArray getPath();
  void setPath(geometry_msgs.PoseArray value);
  suturo_std_msgs.BodyGroup getGroup();
  void setGroup(suturo_std_msgs.BodyGroup value);
}
