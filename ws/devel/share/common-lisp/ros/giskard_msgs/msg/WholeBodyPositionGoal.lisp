; Auto-generated. Do not edit!


(cl:in-package giskard_msgs-msg)


;//! \htmlinclude WholeBodyPositionGoal.msg.html

(cl:defclass <WholeBodyPositionGoal> (roslisp-msg-protocol:ros-message)
  ((left_ee_goal
    :reader left_ee_goal
    :initarg :left_ee_goal
    :type geometry_msgs-msg:PoseStamped
    :initform (cl:make-instance 'geometry_msgs-msg:PoseStamped))
   (right_ee_goal
    :reader right_ee_goal
    :initarg :right_ee_goal
    :type geometry_msgs-msg:PoseStamped
    :initform (cl:make-instance 'geometry_msgs-msg:PoseStamped)))
)

(cl:defclass WholeBodyPositionGoal (<WholeBodyPositionGoal>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <WholeBodyPositionGoal>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'WholeBodyPositionGoal)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_msgs-msg:<WholeBodyPositionGoal> is deprecated: use giskard_msgs-msg:WholeBodyPositionGoal instead.")))

(cl:ensure-generic-function 'left_ee_goal-val :lambda-list '(m))
(cl:defmethod left_ee_goal-val ((m <WholeBodyPositionGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_msgs-msg:left_ee_goal-val is deprecated.  Use giskard_msgs-msg:left_ee_goal instead.")
  (left_ee_goal m))

(cl:ensure-generic-function 'right_ee_goal-val :lambda-list '(m))
(cl:defmethod right_ee_goal-val ((m <WholeBodyPositionGoal>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_msgs-msg:right_ee_goal-val is deprecated.  Use giskard_msgs-msg:right_ee_goal instead.")
  (right_ee_goal m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <WholeBodyPositionGoal>) ostream)
  "Serializes a message object of type '<WholeBodyPositionGoal>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'left_ee_goal) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'right_ee_goal) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <WholeBodyPositionGoal>) istream)
  "Deserializes a message object of type '<WholeBodyPositionGoal>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'left_ee_goal) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'right_ee_goal) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<WholeBodyPositionGoal>)))
  "Returns string type for a message object of type '<WholeBodyPositionGoal>"
  "giskard_msgs/WholeBodyPositionGoal")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'WholeBodyPositionGoal)))
  "Returns string type for a message object of type 'WholeBodyPositionGoal"
  "giskard_msgs/WholeBodyPositionGoal")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<WholeBodyPositionGoal>)))
  "Returns md5sum for a message object of type '<WholeBodyPositionGoal>"
  "782548e8941cb7f36e7e0820685c913e")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'WholeBodyPositionGoal)))
  "Returns md5sum for a message object of type 'WholeBodyPositionGoal"
  "782548e8941cb7f36e7e0820685c913e")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<WholeBodyPositionGoal>)))
  "Returns full string definition for message of type '<WholeBodyPositionGoal>"
  (cl:format cl:nil "geometry_msgs/PoseStamped left_ee_goal~%geometry_msgs/PoseStamped right_ee_goal~%~%================================================================================~%MSG: geometry_msgs/PoseStamped~%# A Pose with reference coordinate frame and timestamp~%Header header~%Pose pose~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of postion and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'WholeBodyPositionGoal)))
  "Returns full string definition for message of type 'WholeBodyPositionGoal"
  (cl:format cl:nil "geometry_msgs/PoseStamped left_ee_goal~%geometry_msgs/PoseStamped right_ee_goal~%~%================================================================================~%MSG: geometry_msgs/PoseStamped~%# A Pose with reference coordinate frame and timestamp~%Header header~%Pose pose~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of postion and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <WholeBodyPositionGoal>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'left_ee_goal))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'right_ee_goal))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <WholeBodyPositionGoal>))
  "Converts a ROS message object to a list"
  (cl:list 'WholeBodyPositionGoal
    (cl:cons ':left_ee_goal (left_ee_goal msg))
    (cl:cons ':right_ee_goal (right_ee_goal msg))
))
