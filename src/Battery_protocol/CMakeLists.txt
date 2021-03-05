################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the battery_commands C++ Library
add_library(Battery_protocol STATIC)
# yarp_add_idl(IDL_GEN_FILES battery_commands.thrift)
target_sources(Battery_protocol
  PRIVATE
    BatteryReader.cpp
    BatteryReader.h
    ChargingStatus.cpp
    ChargingStatus.h)
target_include_directories(Battery_protocol
#   INTERFACE
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(Battery_protocol
  PRIVATE
    YARP::YARP_os)




yarp_prepare_plugin(BatteryReader
  TYPE BatteryReaderMonitorObject
  INCLUDE BatteryReaderMonitorObject.h
  CATEGORY portmonitor
  INTERNAL ON
)

yarp_add_plugin(yarp_pm_BatteryReaderMonitor)

target_sources(yarp_pm_BatteryReaderMonitor
  PRIVATE BatteryReaderMonitorObject.cpp
          BatteryReaderMonitorObject.h
)

target_link_libraries(yarp_pm_BatteryReaderMonitor
  PRIVATE YARP::YARP_os
)

target_include_directories(yarp_pm_BatteryReaderMonitor
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

yarp_install(
  TARGETS yarp_pm_BatteryReaderMonitor
  EXPORT BatteryReaderMonitor
  COMPONENT BatteryReaderMonitor
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
