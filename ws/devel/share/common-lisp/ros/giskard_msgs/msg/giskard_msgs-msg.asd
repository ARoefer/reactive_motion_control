
(cl:in-package :asdf)

(defsystem "giskard_msgs-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
)
  :components ((:file "_package")
    (:file "Finished" :depends-on ("_package_Finished"))
    (:file "_package_Finished" :depends-on ("_package"))
    (:file "Dummy" :depends-on ("_package_Dummy"))
    (:file "_package_Dummy" :depends-on ("_package"))
    (:file "WholeBodyPositionGoal" :depends-on ("_package_WholeBodyPositionGoal"))
    (:file "_package_WholeBodyPositionGoal" :depends-on ("_package"))
  ))