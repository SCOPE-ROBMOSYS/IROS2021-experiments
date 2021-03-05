################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the skill (state machine executable)

include(SkillAutogen)

add_executable(BatteryLevelSkill)
target_include_directories(BatteryLevelSkill
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_sources(BatteryLevelSkill
  PRIVATE
    main.cpp
    BatteryLevelSkill.cpp
    BatteryLevelSkill.h
    BatteryLevelSkillDataModel.cpp
    BatteryLevelSkillDataModel.h
)

qt5_add_statecharts(STATECHARTS_FILES BatteryLevelSkillStateMachine.scxml)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/BatteryLevelSkillStateMachine.cpp PROPERTIES SKIP_AUTOMOC TRUE)

target_sources(BatteryLevelSkill
  PRIVATE
    ${STATECHARTS_FILES})

target_link_libraries(BatteryLevelSkill
  PRIVATE
    Qt5::Scxml
    YARP::YARP_os
    YARP::YARP_init
    Battery_protocol
    BT_protocol)
