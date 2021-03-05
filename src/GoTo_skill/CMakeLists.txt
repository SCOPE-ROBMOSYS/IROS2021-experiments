################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

# Create the skill (state machine executable)

include(SkillAutogen)

add_executable(GoToSkill)
generate_skills(SOURCES_FILES LIBRARIES GoToSkillStateMachine.scxml)
target_sources(GoToSkill PRIVATE ${SOURCES_FILES})
target_link_libraries(GoToSkill PRIVATE ${LIBRARIES})
