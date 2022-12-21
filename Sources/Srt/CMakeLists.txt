# Copyright (C) 2021 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(AlususSrt)
cmake_minimum_required(VERSION 3.20.0)

add_subdirectory(Srl)

# Copy .alusus files to installation directory.
file(GLOB AlususSrt_Source_Files *.alusus *.أسس Srl/*.alusus مـتم/*.أسس)
source_group("SourceFiles\\General" FILES ${AlususSrt_Source_Files})
add_custom_target(AlususSrt SOURCES ${AlususSrt_Source_Files})
install(DIRECTORY "./" DESTINATION "${ALUSUS_LIB_DIR_NAME}" FILES_MATCHING PATTERN "*.alusus" PATTERN "*.أسس")
if (NOT CUSTOM_OUTPUT_PATH STREQUAL "")
  message(STATUS "A custom path was provided so copying SRT files to it: " ${CUSTOM_OUTPUT_PATH} ".")
  install(DIRECTORY "./" DESTINATION "${CUSTOM_OUTPUT_PATH}" FILES_MATCHING PATTERN "*.alusus" PATTERN "*.أسس")
endif()
install(PROGRAMS "./Apm/apm" DESTINATION "${ALUSUS_BIN_DIR_NAME}")
install(PROGRAMS "./Apm/محا" DESTINATION "${ALUSUS_BIN_DIR_NAME}")
