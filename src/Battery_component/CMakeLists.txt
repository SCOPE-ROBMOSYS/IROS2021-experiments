################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

 # Create the component (navigation client)
add_executable(BatteryComponent)
target_sources(BatteryComponent
  PRIVATE
    BatteryComponent.cpp)
target_link_libraries(BatteryComponent
  PRIVATE
    Battery_protocol
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev)
