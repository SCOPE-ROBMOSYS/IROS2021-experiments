################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

add_executable(BlackboardComponent)
target_sources(BlackboardComponent
  PRIVATE
    BlackboardComponent.cpp)
target_link_libraries(BlackboardComponent
  PRIVATE
    Blackboard_protocol
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev)

file(COPY entries.ini DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
