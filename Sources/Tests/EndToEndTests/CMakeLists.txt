# Copyright (C) 2014 Rafid Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususE2ETests)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususE2ETests_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")

# Header files path for Boost libraries
include_directories("${BOOST_PATH}")

# Header and library files paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
link_directories("${LLVM_LIBRARY_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
include_directories("${AlususScg_SOURCE_DIR}")
include_directories("${AlususE2ETests_SOURCE_DIR}")

# Header files for CATCH.
include_directories("${CATCH_PATH}")

# Place files in folders for IDEs (though I only tried Visual Studio at the moment.)
file(GLOB AlususE2ETests_Source_Files *.cpp)
file(GLOB AlususE2ETests_Header_Files *.h)
file(GLOB AlususE2ETests_Test_Files Tests/General/*.alusus Tests/TheCProgrammingLanguage/*.alusus Tests/Arabic/*.أسس)
file(GLOB AlususE2ETests_Test_Files_Output Tests/General/*.output Tests/TheCProgrammingLanguage/*.output Tests/Arabic/*.output)
source_group("SourceFiles\\General" FILES ${AlususE2ETests_Source_Files})
source_group("HeaderFiles\\General" FILES ${AlususE2ETests_Header_Files})
source_group("TestFiles\\General" FILES ${AlususE2ETests_Test_Files} ${AlususE2ETests_Test_Files_Output})

if(MSVC)
  add_definitions("/wd4005 /wd4146 /wd4355 /wd4800 /wd4996")
endif(MSVC)

set(AlususE2ETestsSourceFiles
  ${AlususE2ETests_Source_Files}
  ${AlususE2ETests_Header_Files}
  ${AlususE2ETests_Test_Files}
  ${AlususE2ETests_Test_Files_Output}
  )

add_executable(AlususE2ETests ${AlususE2ETestsSourceFiles} ${AlususE2ETests_Header_Files})
set_target_properties(AlususE2ETests PROPERTIES COMPILE_FLAGS "${AlususE2ETests_COMPILE_FLAGS}")

# Set output names.
set_target_properties(AlususE2ETests PROPERTIES OUTPUT_NAME alusus_e2e_tests)
set_target_properties(AlususE2ETests PROPERTIES DEBUG_OUTPUT_NAME alusus_e2e_tests_d)
set_target_properties(AlususE2ETests PROPERTIES VERSION ${AlususVersion})

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core jit native)

# Finally, we link Alusus libraries to our UnitTests project.
target_link_libraries(AlususE2ETests AlususCoreLib AlususStorage "dl")

# Copy end-to-end test files to the build directory
if (NOT CUSTOM_OUTPUT_PATH STREQUAL "")
  message(STATUS "A custom path was provided so copying end-to-end test files to it: " ${CUSTOM_OUTPUT_PATH} ".")
  file(COPY "Tests/" DESTINATION "${CUSTOM_OUTPUT_PATH}/Tests")
endif()

# Copy test files to the Examples directory.
install(DIRECTORY "Tests/" DESTINATION "Examples" FILES_MATCHING PATTERN "*.alusus" PATTERN "*.أسس")

add_test(NAME AlususE2ETests
  COMMAND AlususE2ETests
  WORKING_DIRECTORY "${AlususE2ETests_SOURCE_DIR}")
set_tests_properties(AlususE2ETests PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususScg_BINARY_DIR};ALUSUS_LIBS=${AlususSrt_SOURCE_DIR}")
