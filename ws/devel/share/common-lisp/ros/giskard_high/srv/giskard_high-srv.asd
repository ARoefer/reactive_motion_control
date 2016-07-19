
(cl:in-package :asdf)

(defsystem "giskard_high-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
               :suturo_std_msgs-msg
)
  :components ((:file "_package")
    (:file "FollowPath" :depends-on ("_package_FollowPath"))
    (:file "_package_FollowPath" :depends-on ("_package"))
  ))