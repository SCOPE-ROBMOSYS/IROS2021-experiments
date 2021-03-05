################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the Blackboard_protocol C++ Library
add_library(Blackboard_protocol STATIC)
yarp_add_idl(IDL_GEN_FILES blackboard_commands.thrift)
target_sources(Blackboard_protocol
  PRIVATE
    ${IDL_GEN_FILES})
target_include_directories(Blackboard_protocol
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(Blackboard_protocol
  PRIVATE
    YARP::YARP_os)
