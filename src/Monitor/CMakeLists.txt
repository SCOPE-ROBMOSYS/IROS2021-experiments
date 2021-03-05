################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

add_executable(monitor)
qt5_add_resources(QRC_GEN_FILES res.qrc)

target_sources(monitor
  PRIVATE
    main.cpp
    ${QRC_GEN_FILES}
)

target_link_libraries(monitor
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    Qt5::Widgets
    Qt5::Qml
    Qt5::Scxml
)
