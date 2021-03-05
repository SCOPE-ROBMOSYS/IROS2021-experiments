################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the BT_protocol C++ Library
add_library(BT_protocol STATIC)
# yarp_add_idl(IDL_GEN_FILES BT_wrappers.thrift)
target_sources(BT_protocol
  PRIVATE
    Skill_request.cpp
    Skill_request.h
    SkillAck.cpp
    SkillAck.h
)
target_include_directories(BT_protocol
#   INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(BT_protocol
  PRIVATE
    YARP::YARP_os)


yarp_prepare_plugin(Skill_request
  TYPE Skill_requestMonitorObject
  INCLUDE Skill_requestMonitorObject.h
  CATEGORY portmonitor
  INTERNAL ON
)

yarp_add_plugin(yarp_pm_Skill_requestMonitor)

target_sources(yarp_pm_Skill_requestMonitor
  PRIVATE Skill_requestMonitorObject.cpp
          Skill_requestMonitorObject.h
)

target_link_libraries(yarp_pm_Skill_requestMonitor
  PRIVATE YARP::YARP_os
)

target_include_directories(yarp_pm_Skill_requestMonitor
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

yarp_install(
  TARGETS yarp_pm_Skill_requestMonitor
  EXPORT Skill_requestMonitor
  COMPONENT Skill_requestMonitor
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
