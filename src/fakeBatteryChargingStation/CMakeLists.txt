################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

add_executable(fakeBatteryChargingStation)

yarp_add_idl(IDL_GEN_FILES FakeBatteryService.thrift)

target_sources(fakeBatteryChargingStation PRIVATE fakeBatteryChargingStation.cpp
                                                  ${IDL_GEN_FILES})

target_link_libraries(fakeBatteryChargingStation PRIVATE YARP::YARP_os
                                                         YARP::YARP_sig
                                                         YARP::YARP_dev
                                                         YARP::YARP_init)
