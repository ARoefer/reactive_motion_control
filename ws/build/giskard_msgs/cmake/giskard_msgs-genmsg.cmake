# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "giskard_msgs: 3 messages, 1 services")

set(MSG_I_FLAGS "-Igiskard_msgs:/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg;-Istd_msgs:/opt/ros/indigo/share/std_msgs/cmake/../msg;-Igeometry_msgs:/opt/ros/indigo/share/geometry_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(genjava REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(giskard_msgs_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" NAME_WE)
add_custom_target(_giskard_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "giskard_msgs" "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" ""
)

get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" NAME_WE)
add_custom_target(_giskard_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "giskard_msgs" "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" ""
)

get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" NAME_WE)
add_custom_target(_giskard_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "giskard_msgs" "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" "geometry_msgs/Point:geometry_msgs/PoseStamped:geometry_msgs/Quaternion:std_msgs/Header:geometry_msgs/Pose"
)

get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" NAME_WE)
add_custom_target(_giskard_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "giskard_msgs" "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" ""
)

#
#  langs = gencpp;genjava;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
)
_generate_msg_cpp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Point.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/PoseStamped.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Quaternion.msg;/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Pose.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
)
_generate_msg_cpp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
)

### Generating Services
_generate_srv_cpp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
)

### Generating Module File
_generate_module_cpp(giskard_msgs
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(giskard_msgs_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(giskard_msgs_generate_messages giskard_msgs_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_cpp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_cpp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_cpp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_cpp _giskard_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(giskard_msgs_gencpp)
add_dependencies(giskard_msgs_gencpp giskard_msgs_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_msgs_generate_messages_cpp)

### Section generating for lang: genjava
### Generating Messages
_generate_msg_java(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
)
_generate_msg_java(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Point.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/PoseStamped.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Quaternion.msg;/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Pose.msg"
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
)
_generate_msg_java(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
)

### Generating Services
_generate_srv_java(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
)

### Generating Module File
_generate_module_java(giskard_msgs
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
  "${ALL_GEN_OUTPUT_FILES_java}"
)

add_custom_target(giskard_msgs_generate_messages_java
  DEPENDS ${ALL_GEN_OUTPUT_FILES_java}
)
add_dependencies(giskard_msgs_generate_messages giskard_msgs_generate_messages_java)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_java _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_java _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_java _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_java _giskard_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(giskard_msgs_genjava)
add_dependencies(giskard_msgs_genjava giskard_msgs_generate_messages_java)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_msgs_generate_messages_java)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
)
_generate_msg_lisp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Point.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/PoseStamped.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Quaternion.msg;/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Pose.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
)
_generate_msg_lisp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
)

### Generating Services
_generate_srv_lisp(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
)

### Generating Module File
_generate_module_lisp(giskard_msgs
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(giskard_msgs_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(giskard_msgs_generate_messages giskard_msgs_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_lisp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_lisp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_lisp _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_lisp _giskard_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(giskard_msgs_genlisp)
add_dependencies(giskard_msgs_genlisp giskard_msgs_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_msgs_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
)
_generate_msg_py(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Point.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/PoseStamped.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Quaternion.msg;/opt/ros/indigo/share/std_msgs/cmake/../msg/Header.msg;/opt/ros/indigo/share/geometry_msgs/cmake/../msg/Pose.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
)
_generate_msg_py(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
)

### Generating Services
_generate_srv_py(giskard_msgs
  "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
)

### Generating Module File
_generate_module_py(giskard_msgs
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(giskard_msgs_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(giskard_msgs_generate_messages giskard_msgs_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Finished.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_py _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/srv/SetEnable.srv" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_py _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/WholeBodyPositionGoal.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_py _giskard_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/home/adrian/bachelor_thesis/ws/src/giskard_msgs/msg/Dummy.msg" NAME_WE)
add_dependencies(giskard_msgs_generate_messages_py _giskard_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(giskard_msgs_genpy)
add_dependencies(giskard_msgs_genpy giskard_msgs_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_msgs_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_msgs
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_msgs_generate_messages_cpp std_msgs_generate_messages_cpp)
add_dependencies(giskard_msgs_generate_messages_cpp geometry_msgs_generate_messages_cpp)

if(genjava_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_msgs
    DESTINATION ${genjava_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_msgs_generate_messages_java std_msgs_generate_messages_java)
add_dependencies(giskard_msgs_generate_messages_java geometry_msgs_generate_messages_java)

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_msgs
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_msgs_generate_messages_lisp std_msgs_generate_messages_lisp)
add_dependencies(giskard_msgs_generate_messages_lisp geometry_msgs_generate_messages_lisp)

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_msgs
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_msgs_generate_messages_py std_msgs_generate_messages_py)
add_dependencies(giskard_msgs_generate_messages_py geometry_msgs_generate_messages_py)
