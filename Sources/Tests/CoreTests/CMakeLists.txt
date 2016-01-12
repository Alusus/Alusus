# Copyright (C) 2014 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususCoreTests)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususCoreTests_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

# Header files path for CATCH libraries
include_directories("${CATCH_PATH}")

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
include_directories("${AlususCoreTests_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususCoreTests_SOURCE_DIR}")

# Add a target for Alusus core executable.
set(AlususCoreTests_Source_Subdirs Test)
file(GLOB AlususCoreTests_Source_Files *.cpp)
file(GLOB AlususCoreTests_Header_Files *.h)
set(AlususCoreTests_Files ${AlususCoreTests_Source_Files} ${AlususCoreTests_Header_Files})
add_executable(AlususCoreTests ${AlususCoreTests_Files})
set_target_properties(AlususCoreTests PROPERTIES COMPILE_FLAGS "${AlususCoreTests_COMPILE_FLAGS}")
ADD_PRECOMPILED_HEADER(AlususCoreTests "core_tests_pch.h")

# Finally, we link the executable to the libraries.
target_link_libraries(AlususCoreTests AlususCoreLib AlususStorage "dl")

# Set executable output names.
set_target_properties(AlususCoreTests PROPERTIES OUTPUT_NAME alusus_tests)
set_target_properties(AlususCoreTests PROPERTIES DEBUG_OUTPUT_NAME alusus_tests.dbg)
set_target_properties(AlususCoreTests PROPERTIES VERSION ${AlususVersion})

add_test(NAME AlususCoreTests COMMAND AlususCoreTests)
set_tests_properties(AlususCoreTests PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}")
