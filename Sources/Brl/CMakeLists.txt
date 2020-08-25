# Copyright (C) 2020 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(AlususBrl)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususBrl_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

# Make sure the compiler finds the source files.
include_directories("${AlususBrl_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususBrl_SOURCE_DIR}")

# Add a target for Alusus BRL library.
set(AlususBrlLib_Source_Subdirs "")
set(AlususBrlLib_Source_Files brl.h brl.cpp)
foreach (DIR ${AlususBrlLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  file(GLOB sources ${DIR}/*.cpp)
  source_group("HeaderFiles\\${DIR}" FILES ${headers})
  source_group("SourceFiles\\${DIR}" FILES ${sources})
  set(AlususBrlLib_Source_Files ${AlususBrlLib_Source_Files} ${sources} ${headers})
endforeach(DIR)
add_library(AlususBrlLib STATIC ${AlususBrlLib_Source_Files})
set_target_properties(AlususBrlLib PROPERTIES COMPILE_FLAGS "-fpic ${AlususBrl_COMPILE_FLAGS}")
ADD_PRECOMPILED_HEADER(AlususBrlLib "brl.h")

# Set library and executable output names.
set_target_properties(AlususBrlLib PROPERTIES OUTPUT_NAME alusus_brl)
set_target_properties(AlususBrlLib PROPERTIES DEBUG_OUTPUT_NAME alusus_brl.dbg)
set_target_properties(AlususBrlLib PROPERTIES VERSION ${AlususVersion})

# Copy libary header files to installation directory.
install_files("/Include/Brl" FILES brl.h)
foreach (DIR ${AlususBrlLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  install_files("/Include/Brl/${DIR}" FILES ${headers})
endforeach(DIR)

# Install library and executable files.
install(TARGETS AlususBrlLib
  RUNTIME DESTINATION Bin
  LIBRARY DESTINATION Lib
  ARCHIVE DESTINATION Lib
)

# Copy .alusus files to installation directory.
file(GLOB sources ./*.alusus)
install_files("/Lib/Brl/${DIR}" FILES ${sources})
file(GLOB sources ./*.أسس)
install_files("/Lib/مـتا/${DIR}" FILES ${sources})
foreach (DIR ${AlususBrlLib_Source_Subdirs})
  file(GLOB sources ${DIR}/*.alusus)
  install_files("/Lib/Brl/${DIR}" FILES ${sources})
  file(GLOB sources ${DIR}/*.أسس)
  install_files("/Lib/مـتأ/${DIR}" FILES ${sources})
endforeach(DIR)
