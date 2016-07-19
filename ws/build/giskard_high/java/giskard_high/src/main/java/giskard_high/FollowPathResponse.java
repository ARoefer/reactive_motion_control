package giskard_high;

public interface FollowPathResponse extends org.ros.internal.message.Message {
  static final java.lang.String _TYPE = "giskard_high/FollowPathResponse";
  static final java.lang.String _DEFINITION = "suturo_std_msgs/ExecutionFeedback feedback";
  suturo_std_msgs.ExecutionFeedback getFeedback();
  void setFeedback(suturo_std_msgs.ExecutionFeedback value);
}
