# Copyright (C) 2021 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(AlususSrl)
cmake_minimum_required(VERSION 3.20.0)

# Prepare compile flags.
set(AlususSrl_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

# Make sure the compiler finds the source files.
include_directories("${AlususSrl_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususSrl_SOURCE_DIR}")

# Add a target for Alusus SRL library.
file(GLOB headers *.h)
file(GLOB sources *.cpp)
set(AlususSrlLib_Source_Files ${AlususSrlLib_Source_Files} ${sources} ${headers})
add_library(AlususSrlLib STATIC ${AlususSrlLib_Source_Files})
set_target_properties(AlususSrlLib PROPERTIES COMPILE_FLAGS "${FPIC} ${AlususSrl_COMPILE_FLAGS}")
target_precompile_headers(AlususSrlLib PRIVATE "srl.h")

# Set library and executable output names.
set_target_properties(AlususSrlLib PROPERTIES OUTPUT_NAME alusus_srl)
set_target_properties(AlususSrlLib PROPERTIES DEBUG_OUTPUT_NAME alusus_srl.dbg)
set_target_properties(AlususSrlLib PROPERTIES VERSION ${AlususVersion})

# Copy libary header files to installation directory.
install_files("/${ALUSUS_INCLUDE_DIR_NAME}/Srl" FILES srl.h)
foreach (DIR ${AlususSrlLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  install_files("/${ALUSUS_INCLUDE_DIR_NAME}/Srl/${DIR}" FILES ${headers})
endforeach(DIR)

# Install library and executable files.
install(TARGETS AlususSrlLib
  RUNTIME DESTINATION ${ALUSUS_BIN_DIR_NAME}
  LIBRARY DESTINATION ${ALUSUS_LIB_DIR_NAME}
  ARCHIVE DESTINATION ${ALUSUS_LIB_DIR_NAME}
)
