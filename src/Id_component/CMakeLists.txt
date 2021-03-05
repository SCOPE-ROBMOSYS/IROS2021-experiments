################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

 # Create the component (navigation client)
add_executable(IdComponent)
target_sources(IdComponent
  PRIVATE
    IdComponent.cpp)
target_link_libraries(IdComponent
  PRIVATE
    Id_protocol
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev)
