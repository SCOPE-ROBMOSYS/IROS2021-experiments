################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the Id_protocol C++ Library
add_library(Id_protocol STATIC)
yarp_add_idl(IDL_GEN_FILES Id.thrift)
target_sources(Id_protocol
  PRIVATE
    ${IDL_GEN_FILES})
target_include_directories(Id_protocol
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(Id_protocol
  PRIVATE
    YARP::YARP_os)
