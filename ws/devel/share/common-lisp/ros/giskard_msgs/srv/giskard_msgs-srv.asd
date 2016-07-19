
(cl:in-package :asdf)

(defsystem "giskard_msgs-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "SetEnable" :depends-on ("_package_SetEnable"))
    (:file "_package_SetEnable" :depends-on ("_package"))
  ))