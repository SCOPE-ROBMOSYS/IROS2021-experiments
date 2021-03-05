################################################################################
#                                                                              #
# Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 3.12)
project(bt_implementation
        VERSION 1.0.0
        LANGUAGES CXX)

include(GNUInstallDirs)
include(FeatureSummary)

find_package(YCM 0.11 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PATH_ROOT_TO_PACKAGE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
#set(SKILL_SCXML_FILES_DIR "${PATH_ROOT_TO_PACKAGE_ROOT_DIR}/src/skill-code-gen/input-files")

# Pick up our CMake scripts - they are all in the cmake subdirectory.
set(BT_IMPLEM_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${BT_IMPLEM_MODULE_DIR}")
message(STATUS "Using:")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")

set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "bt_implementation is always built with dynamic plugins")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build libraries as shared as opposed to static")

include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS 1)

include(AddUninstallTarget)


find_package(Qt5 COMPONENTS Widgets Qml Scxml REQUIRED)
set(CMAKE_AUTOMOC ON)

find_package(YARP COMPONENTS os idl_tools dev REQUIRED)


# BehaviorTree.CPP links Boost::coroutine, but does not look for it
find_package(Boost COMPONENTS coroutine REQUIRED)

find_package(BehaviorTree REQUIRED)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)

set(CMAKE_C_FLAGS "${YARP_C_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${YARP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

yarp_configure_plugins_installation(bt_implementation)

# Make some custom header files containing compile settings.
add_subdirectory(cmake)

# Set up the main libraries and utilities.
add_subdirectory(src)
