; Auto-generated. Do not edit!


(cl:in-package giskard_msgs-srv)


;//! \htmlinclude SetEnable-request.msg.html

(cl:defclass <SetEnable-request> (roslisp-msg-protocol:ros-message)
  ((enable
    :reader enable
    :initarg :enable
    :type cl:fixnum
    :initform 0))
)

(cl:defclass SetEnable-request (<SetEnable-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SetEnable-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SetEnable-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_msgs-srv:<SetEnable-request> is deprecated: use giskard_msgs-srv:SetEnable-request instead.")))

(cl:ensure-generic-function 'enable-val :lambda-list '(m))
(cl:defmethod enable-val ((m <SetEnable-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader giskard_msgs-srv:enable-val is deprecated.  Use giskard_msgs-srv:enable instead.")
  (enable m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SetEnable-request>) ostream)
  "Serializes a message object of type '<SetEnable-request>"
  (cl:let* ((signed (cl:slot-value msg 'enable)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SetEnable-request>) istream)
  "Deserializes a message object of type '<SetEnable-request>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'enable) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SetEnable-request>)))
  "Returns string type for a service object of type '<SetEnable-request>"
  "giskard_msgs/SetEnableRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetEnable-request)))
  "Returns string type for a service object of type 'SetEnable-request"
  "giskard_msgs/SetEnableRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SetEnable-request>)))
  "Returns md5sum for a message object of type '<SetEnable-request>"
  "ac6526613895843e4f8f058d9ae9c5ce")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SetEnable-request)))
  "Returns md5sum for a message object of type 'SetEnable-request"
  "ac6526613895843e4f8f058d9ae9c5ce")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SetEnable-request>)))
  "Returns full string definition for message of type '<SetEnable-request>"
  (cl:format cl:nil "int8 enable~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SetEnable-request)))
  "Returns full string definition for message of type 'SetEnable-request"
  (cl:format cl:nil "int8 enable~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SetEnable-request>))
  (cl:+ 0
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SetEnable-request>))
  "Converts a ROS message object to a list"
  (cl:list 'SetEnable-request
    (cl:cons ':enable (enable msg))
))
;//! \htmlinclude SetEnable-response.msg.html

(cl:defclass <SetEnable-response> (roslisp-msg-protocol:ros-message)
  ()
)

(cl:defclass SetEnable-response (<SetEnable-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SetEnable-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SetEnable-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name giskard_msgs-srv:<SetEnable-response> is deprecated: use giskard_msgs-srv:SetEnable-response instead.")))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SetEnable-response>) ostream)
  "Serializes a message object of type '<SetEnable-response>"
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SetEnable-response>) istream)
  "Deserializes a message object of type '<SetEnable-response>"
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SetEnable-response>)))
  "Returns string type for a service object of type '<SetEnable-response>"
  "giskard_msgs/SetEnableResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetEnable-response)))
  "Returns string type for a service object of type 'SetEnable-response"
  "giskard_msgs/SetEnableResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SetEnable-response>)))
  "Returns md5sum for a message object of type '<SetEnable-response>"
  "ac6526613895843e4f8f058d9ae9c5ce")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SetEnable-response)))
  "Returns md5sum for a message object of type 'SetEnable-response"
  "ac6526613895843e4f8f058d9ae9c5ce")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SetEnable-response>)))
  "Returns full string definition for message of type '<SetEnable-response>"
  (cl:format cl:nil "~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SetEnable-response)))
  "Returns full string definition for message of type 'SetEnable-response"
  (cl:format cl:nil "~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SetEnable-response>))
  (cl:+ 0
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SetEnable-response>))
  "Converts a ROS message object to a list"
  (cl:list 'SetEnable-response
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'SetEnable)))
  'SetEnable-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'SetEnable)))
  'SetEnable-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetEnable)))
  "Returns string type for a service object of type '<SetEnable>"
  "giskard_msgs/SetEnable")