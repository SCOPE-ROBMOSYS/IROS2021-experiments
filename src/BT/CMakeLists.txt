################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the yarp_bt_nodes library
add_library(yarp_bt_nodes STATIC)
target_sources(yarp_bt_nodes
  PRIVATE
    yarp_action.cpp
    yarp_action.h
    yarp_condition.cpp
    yarp_condition.h
    yarp_node.cpp
    yarp_node.h)
target_include_directories(yarp_bt_nodes
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(yarp_bt_nodes
  PUBLIC
    YARP::YARP_os
    YARP::YARP_init
    BehaviorTree::behaviortree_cpp_v3
    BT_protocol)

# Create the BT executable
add_executable(from_xml_example)
target_sources(from_xml_example
  PRIVATE
    from_xml_example.cpp)
target_link_libraries(from_xml_example
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    BehaviorTree::behaviortree_cpp_v3
    yarp_bt_nodes
    pthread)

file(COPY test_action_BT.xml DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
file(COPY test_battery_BT.xml DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
