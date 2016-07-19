; Auto-generated. Do not edit!


(cl:in-package giskard_high-srv)


;//! \htmlinclude FollowPath-request.msg.html

(cl:defclass <FollowPath-request> (roslisp-msg-protocol:ros-message)
  ((path
    :reader path
    :initarg :path
    :type geometry_msgs-msg:PoseArray
    :initform (cl:make-instance 'geometry_msgs-msg:PoseArray))
   (group
    :reader group
    :initarg :group
    :type suturo_std_msgs-msg:BodyGroup
    :initform (cl:make-instance 'suturo_std_msgs-msg:BodyGroup)))
)

(cl:defclass FollowPath-request (<FollowPath-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <FollowPath-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'FollowPath-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_high-srv:<FollowPath-request> is deprecated: use giskard_high-srv:FollowPath-request instead.")))

(cl:ensure-generic-function 'path-val :lambda-list '(m))
(cl:defmethod path-val ((m <FollowPath-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_high-srv:path-val is deprecated.  Use giskard_high-srv:path instead.")
  (path m))

(cl:ensure-generic-function 'group-val :lambda-list '(m))
(cl:defmethod group-val ((m <FollowPath-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_high-srv:group-val is deprecated.  Use giskard_high-srv:group instead.")
  (group m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <FollowPath-request>) ostream)
  "Serializes a message object of type '<FollowPath-request>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'path) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'group) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <FollowPath-request>) istream)
  "Deserializes a message object of type '<FollowPath-request>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'path) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'group) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<FollowPath-request>)))
  "Returns string type for a service object of type '<FollowPath-request>"
  "giskard_high/FollowPathRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'FollowPath-request)))
  "Returns string type for a service object of type 'FollowPath-request"
  "giskard_high/FollowPathRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<FollowPath-request>)))
  "Returns md5sum for a message object of type '<FollowPath-request>"
  "201a50976823ee0afe36476bd2e9a0a7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'FollowPath-request)))
  "Returns md5sum for a message object of type 'FollowPath-request"
  "201a50976823ee0afe36476bd2e9a0a7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<FollowPath-request>)))
  "Returns full string definition for message of type '<FollowPath-request>"
  (cl:format cl:nil "geometry_msgs/PoseArray path~%suturo_std_msgs/BodyGroup group~%~%================================================================================~%MSG: geometry_msgs/PoseArray~%# An array of poses with a header for global reference.~%~%Header header~%~%Pose[] poses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of postion and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%================================================================================~%MSG: suturo_std_msgs/BodyGroup~%uint8 value~%~%uint8 NONE     = 0~%uint8 RIGHTARM = 1~%uint8 LEFTARM  = 2~%uint8 ALLARMS  = 3~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'FollowPath-request)))
  "Returns full string definition for message of type 'FollowPath-request"
  (cl:format cl:nil "geometry_msgs/PoseArray path~%suturo_std_msgs/BodyGroup group~%~%================================================================================~%MSG: geometry_msgs/PoseArray~%# An array of poses with a header for global reference.~%~%Header header~%~%Pose[] poses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of postion and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%================================================================================~%MSG: suturo_std_msgs/BodyGroup~%uint8 value~%~%uint8 NONE     = 0~%uint8 RIGHTARM = 1~%uint8 LEFTARM  = 2~%uint8 ALLARMS  = 3~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <FollowPath-request>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'path))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'group))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <FollowPath-request>))
  "Converts a ROS message object to a list"
  (cl:list 'FollowPath-request
    (cl:cons ':path (path msg))
    (cl:cons ':group (group msg))
))
;//! \htmlinclude FollowPath-response.msg.html

(cl:defclass <FollowPath-response> (roslisp-msg-protocol:ros-message)
  ((feedback
    :reader feedback
    :initarg :feedback
    :type suturo_std_msgs-msg:ExecutionFeedback
    :initform (cl:make-instance 'suturo_std_msgs-msg:ExecutionFeedback)))
)

(cl:defclass FollowPath-response (<FollowPath-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <FollowPath-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'FollowPath-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_high-srv:<FollowPath-response> is deprecated: use giskard_high-srv:FollowPath-response instead.")))

(cl:ensure-generic-function 'feedback-val :lambda-list '(m))
(cl:defmethod feedback-val ((m <FollowPath-response>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_high-srv:feedback-val is deprecated.  Use giskard_high-srv:feedback instead.")
  (feedback m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <FollowPath-response>) ostream)
  "Serializes a message object of type '<FollowPath-response>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'feedback) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <FollowPath-response>) istream)
  "Deserializes a message object of type '<FollowPath-response>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'feedback) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<FollowPath-response>)))
  "Returns string type for a service object of type '<FollowPath-response>"
  "giskard_high/FollowPathResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'FollowPath-response)))
  "Returns string type for a service object of type 'FollowPath-response"
  "giskard_high/FollowPathResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<FollowPath-response>)))
  "Returns md5sum for a message object of type '<FollowPath-response>"
  "201a50976823ee0afe36476bd2e9a0a7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'FollowPath-response)))
  "Returns md5sum for a message object of type 'FollowPath-response"
  "201a50976823ee0afe36476bd2e9a0a7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<FollowPath-response>)))
  "Returns full string definition for message of type '<FollowPath-response>"
  (cl:format cl:nil "suturo_std_msgs/ExecutionFeedback feedback~%~%================================================================================~%MSG: suturo_std_msgs/ExecutionFeedback~%uint8 value~%~%uint8 FAILED = 0~%uint8 SUCCESS = 1~%uint8 ABORTED = 2~%uint8 OCCUPIED = 3~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'FollowPath-response)))
  "Returns full string definition for message of type 'FollowPath-response"
  (cl:format cl:nil "suturo_std_msgs/ExecutionFeedback feedback~%~%================================================================================~%MSG: suturo_std_msgs/ExecutionFeedback~%uint8 value~%~%uint8 FAILED = 0~%uint8 SUCCESS = 1~%uint8 ABORTED = 2~%uint8 OCCUPIED = 3~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <FollowPath-response>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'feedback))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <FollowPath-response>))
  "Converts a ROS message object to a list"
  (cl:list 'FollowPath-response
    (cl:cons ':feedback (feedback msg))
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'FollowPath)))
  'FollowPath-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'FollowPath)))
  'FollowPath-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'FollowPath)))
  "Returns string type for a service object of type '<FollowPath>"
  "giskard_high/FollowPath")