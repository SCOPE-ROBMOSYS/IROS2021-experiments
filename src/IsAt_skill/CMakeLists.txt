################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the skill (state machine executable)

include(SkillAutogen)

add_executable(IsAtSkill)
generate_skills(SOURCES_FILES LIBRARIES IsAtSkillStateMachine.scxml)
target_sources(IsAtSkill PRIVATE ${SOURCES_FILES})
target_link_libraries(IsAtSkill PRIVATE ${LIBRARIES})
