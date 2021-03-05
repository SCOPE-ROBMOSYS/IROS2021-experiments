################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the skill (state machine executable)

include(SkillAutogen)

add_executable(BatteryNotChargingSkill)
generate_skills(SOURCES_FILES LIBRARIES BatteryNotChargingSkillStateMachine.scxml)
target_sources(BatteryNotChargingSkill PRIVATE ${SOURCES_FILES})
target_link_libraries(BatteryNotChargingSkill PRIVATE ${LIBRARIES})
