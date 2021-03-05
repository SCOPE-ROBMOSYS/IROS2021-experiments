################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the component (navigation client)
add_executable(GoToComponent)
target_sources(GoToComponent
  PRIVATE
    GoToComponent.cpp)
target_link_libraries(GoToComponent
  PRIVATE
    GoTo_protocol
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev)
