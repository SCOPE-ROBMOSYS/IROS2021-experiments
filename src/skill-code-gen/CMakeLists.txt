################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

set(SOURCES main.cpp)
qt5_add_resources(SOURCES list_templates.qrc)

add_executable(skill-code-gen ${SOURCES})

target_sources(skill-code-gen
  PRIVATE
    main.cpp
    qscxmlcompiler.cpp
    qscxmlcompiler.h
    qscxmlcompiler_p.h
    SkillGenerator.cpp
    SkillGenerator.h
    SkillConfigReader.cpp
    SkillConfigReader.h
)
target_link_libraries(skill-code-gen
  PRIVATE
    Qt5::Core
    Qt5::Scxml
    Qt5::ScxmlPrivate
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/qscxmlcompiler.cpp PROPERTIES SKIP_AUTOMOC TRUE)

target_compile_definitions(skill-code-gen
  PRIVATE
    BUILD_QSCXMLC) # Used in qscxmlcompiler.cpp

target_compile_options(skill-code-gen
  PRIVATE
    -Wno-overloaded-virtual)
