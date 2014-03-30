# Copyright (C) 2014 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususCore)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususCore_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususCore_SOURCE_DIR}")

# Add a target for Alusus global storage.
set(AlususStorage_Source_Files core_global_storage.cpp core_global_storage.h)
add_library(AlususStorage SHARED ${AlususStorage_Source_Files})
set_target_properties(AlususStorage PROPERTIES COMPILE_FLAGS "-fpic ${AlususCore_COMPILE_FLAGS}")
 
# Add a target for Alusus core library.
set(AlususCoreLib_Source_Subdirs Basic Data Common Lexer Parser Main Standard)
set(AlususCoreLib_Source_Files core.h)
foreach (DIR ${AlususCoreLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  file(GLOB sources ${DIR}/*.cpp)
  source_group("HeaderFiles\\${DIR}" FILES ${headers})
  source_group("SourceFiles\\${DIR}" FILES ${sources})
  set(AlususCoreLib_Source_Files ${AlususCoreLib_Source_Files} ${sources} ${headers})
endforeach(DIR)
add_library(AlususCoreLib STATIC ${AlususCoreLib_Source_Files})
set_target_properties(AlususCoreLib PROPERTIES COMPILE_FLAGS "-fpic ${AlususCore_COMPILE_FLAGS}")
ADD_PRECOMPILED_HEADER(AlususCoreLib "core.h")

# Add a target for Alusus core executable.
set(AlususCore_Source_Files main.cpp)
add_executable(AlususCore ${AlususCore_Source_Files})
set_target_properties(AlususCore PROPERTIES COMPILE_FLAGS "${AlususCore_COMPILE_FLAGS}")
ADD_PRECOMPILED_HEADER(AlususCore "core.h")

# Finally, we link the executable to the libraries.
target_link_libraries(AlususCore AlususCoreLib AlususStorage "dl")

# Set library and executable output names.
set_target_properties(AlususStorage PROPERTIES OUTPUT_NAME alusus_storage)
set_target_properties(AlususStorage PROPERTIES DEBUG_OUTPUT_NAME alusus_storage_d)
set_target_properties(AlususStorage PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususStorage PROPERTIES SOVERSION ${AlususSoVersion})
set_target_properties(AlususCoreLib PROPERTIES OUTPUT_NAME alusus)
set_target_properties(AlususCoreLib PROPERTIES DEBUG_OUTPUT_NAME alusus_d)
set_target_properties(AlususCoreLib PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususCore PROPERTIES OUTPUT_NAME alusus)
set_target_properties(AlususCore PROPERTIES DEBUG_OUTPUT_NAME alusus_d)
set_target_properties(AlususCore PROPERTIES VERSION ${AlususVersion})

# Copy libary header files to installation directory.
install_files("/Include/Core" FILES core.h core_global_storage.h)
foreach (DIR ${AlususCoreLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  install_files("/Include/Core/${DIR}" FILES ${headers})
endforeach(DIR)

# Install library and executable files.
install(TARGETS AlususStorage AlususCoreLib AlususCore
  RUNTIME DESTINATION Bin
  LIBRARY DESTINATION Lib
  ARCHIVE DESTINATION Lib
)

