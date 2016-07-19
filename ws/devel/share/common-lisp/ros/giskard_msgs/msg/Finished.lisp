; Auto-generated. Do not edit!


(cl:in-package giskard_msgs-msg)


;//! \htmlinclude Finished.msg.html

(cl:defclass <Finished> (roslisp-msg-protocol:ros-message)
  ()
)

(cl:defclass Finished (<Finished>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Finished>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Finished)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_msgs-msg:<Finished> is deprecated: use giskard_msgs-msg:Finished instead.")))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Finished>) ostream)
  "Serializes a message object of type '<Finished>"
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Finished>) istream)
  "Deserializes a message object of type '<Finished>"
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Finished>)))
  "Returns string type for a message object of type '<Finished>"
  "giskard_msgs/Finished")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Finished)))
  "Returns string type for a message object of type 'Finished"
  "giskard_msgs/Finished")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Finished>)))
  "Returns md5sum for a message object of type '<Finished>"
  "d41d8cd98f00b204e9800998ecf8427e")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Finished)))
  "Returns md5sum for a message object of type 'Finished"
  "d41d8cd98f00b204e9800998ecf8427e")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Finished>)))
  "Returns full string definition for message of type '<Finished>"
  (cl:format cl:nil "~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Finished)))
  "Returns full string definition for message of type 'Finished"
  (cl:format cl:nil "~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Finished>))
  (cl:+ 0
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Finished>))
  "Converts a ROS message object to a list"
  (cl:list 'Finished
))
