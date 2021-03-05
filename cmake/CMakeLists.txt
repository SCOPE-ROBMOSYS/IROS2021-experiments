################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# YARP CMake Modules
set(CMAKE_FILES SkillAutogen.cmake)
install(FILES ${CMAKE_FILES}
        DESTINATION share/bt-implementation/cmake
        COMPONENT BT_IMPLEM_conf-dev)
