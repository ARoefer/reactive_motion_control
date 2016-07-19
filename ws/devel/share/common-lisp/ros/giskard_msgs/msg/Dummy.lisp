; Auto-generated. Do not edit!


(cl:in-package giskard_msgs-msg)


;//! \htmlinclude Dummy.msg.html

(cl:defclass <Dummy> (roslisp-msg-protocol:ros-message)
  ()
)

(cl:defclass Dummy (<Dummy>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Dummy>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Dummy)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_msgs-msg:<Dummy> is deprecated: use giskard_msgs-msg:Dummy instead.")))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Dummy>) ostream)
  "Serializes a message object of type '<Dummy>"
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Dummy>) istream)
  "Deserializes a message object of type '<Dummy>"
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Dummy>)))
  "Returns string type for a message object of type '<Dummy>"
  "giskard_msgs/Dummy")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Dummy)))
  "Returns string type for a message object of type 'Dummy"
  "giskard_msgs/Dummy")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Dummy>)))
  "Returns md5sum for a message object of type '<Dummy>"
  "d41d8cd98f00b204e9800998ecf8427e")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Dummy)))
  "Returns md5sum for a message object of type 'Dummy"
  "d41d8cd98f00b204e9800998ecf8427e")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Dummy>)))
  "Returns full string definition for message of type '<Dummy>"
  (cl:format cl:nil "~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Dummy)))
  "Returns full string definition for message of type 'Dummy"
  (cl:format cl:nil "~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Dummy>))
  (cl:+ 0
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Dummy>))
  "Converts a ROS message object to a list"
  (cl:list 'Dummy
))
