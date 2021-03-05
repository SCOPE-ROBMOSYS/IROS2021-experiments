################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the GoTo_protocol C++ Library
add_library(GoTo_protocol STATIC)
# yarp_add_idl(IDL_GEN_FILES GoTo.thrift)
target_sources(GoTo_protocol
  PRIVATE
    GoTo.cpp
    GoTo.h
    GoToStatus.cpp
    GoToStatus.h
)
target_include_directories(GoTo_protocol
#   INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(GoTo_protocol
  PRIVATE
    YARP::YARP_os)


yarp_prepare_plugin(GoTo
  TYPE GoToMonitorObject
  INCLUDE GoToMonitorObject.h
  CATEGORY portmonitor
  INTERNAL ON
)

yarp_add_plugin(yarp_pm_GoToMonitor)

target_sources(yarp_pm_GoToMonitor
  PRIVATE GoToMonitorObject.cpp
          GoToMonitorObject.h
)

target_link_libraries(yarp_pm_GoToMonitor
  PRIVATE YARP::YARP_os
)

target_include_directories(yarp_pm_GoToMonitor
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

yarp_install(
  TARGETS yarp_pm_GoToMonitor
  EXPORT GoToMonitor
  COMPONENT GoToMonitor
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
