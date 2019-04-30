# Copyright (C) 2019 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususSrt)
cmake_minimum_required(VERSION 2.8)

file(GLOB AlususSrt_Source_Files *.alusus *.أسس Srl/*.alusus مـتم/*.أسس Glib/*.alusus جـلب/*.أسس Gtk/*.alusus جـتك/*.أسس)
source_group("SourceFiles\\General" FILES ${AlususSrt_Source_Files})
add_custom_target(AlususSrt SOURCES ${AlususSrt_Source_Files})
install(DIRECTORY "./" DESTINATION "Lib" FILES_MATCHING PATTERN "*.alusus" PATTERN "*.أسس")
if (NOT CUSTOM_OUTPUT_PATH STREQUAL "")
  message(STATUS "A custom path was provided so copying SRT files to it: " ${CUSTOM_OUTPUT_PATH} ".")
  install(DIRECTORY "./" DESTINATION "${CUSTOM_OUTPUT_PATH}" FILES_MATCHING PATTERN "*.alusus" PATTERN "*.أسس")
endif()
install(PROGRAMS "./apm" DESTINATION "Bin")
install(PROGRAMS "./محا" DESTINATION "Bin")