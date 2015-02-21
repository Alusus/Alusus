# Copyright (C) 2014 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususSrt)
cmake_minimum_required(VERSION 2.8)

file(GLOB AlususSrt_Source_Files *.alusus)
source_group("SourceFiles\\General" FILES ${AlususSrt_Source_Files})
add_custom_target(AlususSrt SOURCES ${AlususSrt_Source_Files})
install_files("/Lib" FILES ${AlususSrt_Source_Files})
if (NOT CUSTOM_OUTPUT_PATH STREQUAL "")
  message(STATUS "A custom path was provided so copying SRT files to it: " ${CUSTOM_OUTPUT_PATH} ".")
  file(COPY ${AlususSrt_Source_Files} DESTINATION "${CUSTOM_OUTPUT_PATH}")
endif()
