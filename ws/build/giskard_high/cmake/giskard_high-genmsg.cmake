# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(WARNING "Invoking generate_messages() without having added any message or service file before.
You should either add add_message_files() and/or add_service_files() calls or remove the invocation of generate_messages().")
message(STATUS "giskard_high: 0 messages, 0 services")

set(MSG_I_FLAGS "-Igeometry_msgs:/opt/ros/indigo/share/geometry_msgs/cmake/../msg;-Isuturo_std_msgs:/home/adrian/suturo_ws/src/suturo_msgs/suturo_std_msgs/msg;-Istd_msgs:/opt/ros/indigo/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(genjava REQUIRED)
find_package(genlisp REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(giskard_high_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



#
#  langs = gencpp;genjava;genlisp;genpy
#

### Section generating for lang: gencpp
### Generating Messages

### Generating Services

### Generating Module File
_generate_module_cpp(giskard_high
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_high
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(giskard_high_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(giskard_high_generate_messages giskard_high_generate_messages_cpp)

# add dependencies to all check dependencies targets

# target for backward compatibility
add_custom_target(giskard_high_gencpp)
add_dependencies(giskard_high_gencpp giskard_high_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_high_generate_messages_cpp)

### Section generating for lang: genjava
### Generating Messages

### Generating Services

### Generating Module File
_generate_module_java(giskard_high
  ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_high
  "${ALL_GEN_OUTPUT_FILES_java}"
)

add_custom_target(giskard_high_generate_messages_java
  DEPENDS ${ALL_GEN_OUTPUT_FILES_java}
)
add_dependencies(giskard_high_generate_messages giskard_high_generate_messages_java)

# add dependencies to all check dependencies targets

# target for backward compatibility
add_custom_target(giskard_high_genjava)
add_dependencies(giskard_high_genjava giskard_high_generate_messages_java)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_high_generate_messages_java)

### Section generating for lang: genlisp
### Generating Messages

### Generating Services

### Generating Module File
_generate_module_lisp(giskard_high
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_high
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(giskard_high_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(giskard_high_generate_messages giskard_high_generate_messages_lisp)

# add dependencies to all check dependencies targets

# target for backward compatibility
add_custom_target(giskard_high_genlisp)
add_dependencies(giskard_high_genlisp giskard_high_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_high_generate_messages_lisp)

### Section generating for lang: genpy
### Generating Messages

### Generating Services

### Generating Module File
_generate_module_py(giskard_high
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_high
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(giskard_high_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(giskard_high_generate_messages giskard_high_generate_messages_py)

# add dependencies to all check dependencies targets

# target for backward compatibility
add_custom_target(giskard_high_genpy)
add_dependencies(giskard_high_genpy giskard_high_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS giskard_high_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_high)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/giskard_high
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_high_generate_messages_cpp geometry_msgs_generate_messages_cpp)
add_dependencies(giskard_high_generate_messages_cpp suturo_std_msgs_generate_messages_cpp)

if(genjava_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_high)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genjava_INSTALL_DIR}/giskard_high
    DESTINATION ${genjava_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_high_generate_messages_java geometry_msgs_generate_messages_java)
add_dependencies(giskard_high_generate_messages_java suturo_std_msgs_generate_messages_java)

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_high)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/giskard_high
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_high_generate_messages_lisp geometry_msgs_generate_messages_lisp)
add_dependencies(giskard_high_generate_messages_lisp suturo_std_msgs_generate_messages_lisp)

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_high)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_high\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/giskard_high
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
add_dependencies(giskard_high_generate_messages_py geometry_msgs_generate_messages_py)
add_dependencies(giskard_high_generate_messages_py suturo_std_msgs_generate_messages_py)
