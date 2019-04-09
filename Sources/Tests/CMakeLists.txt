# Copyright (C) 2014 Rafid Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususTests)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususTests_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")

# Header and library files paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
link_directories("${LLVM_LIBRARY_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
include_directories("${AlususTests_SOURCE_DIR}")

# Place files in folders for IDEs (though I only tried Visual Studio at the moment.)
file(GLOB AlususTests_Source_Files *.cpp)
file(GLOB AlususTests_Header_Files *.h)
file(GLOB AlususTests_Test_Files Core/*.alusus Spp/Parsing/*.alusus Spp/Building/*.alusus)
file(GLOB AlususTests_Test_Files_Output Core/*.output Spp/Parsing/*.output Spp/Building/*.output)
source_group("SourceFiles\\General" FILES ${AlususTests_Source_Files})
source_group("HeaderFiles\\General" FILES ${AlususTests_Header_Files})
source_group("TestFiles\\General" FILES ${AlususTests_Test_Files} ${AlususTests_Test_Files_Output})

if(MSVC)
  add_definitions("/wd4005 /wd4146 /wd4355 /wd4800 /wd4996")
endif(MSVC)

set(AlususTestsSourceFiles
  ${AlususTests_Source_Files}
  ${AlususTests_Header_Files}
  ${AlususTests_Test_Files}
  ${AlususTests_Test_Files_Output}
)

add_executable(AlususTests ${AlususTestsSourceFiles} ${AlususTests_Header_Files})
set_target_properties(AlususTests PROPERTIES COMPILE_FLAGS "${AlususTests_COMPILE_FLAGS}")

# Set output names.
set_target_properties(AlususTests PROPERTIES OUTPUT_NAME alusus_tests)
set_target_properties(AlususTests PROPERTIES DEBUG_OUTPUT_NAME alusus_tests.dbg)
set_target_properties(AlususTests PROPERTIES VERSION ${AlususVersion})

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES core mcjit native)

# Finally, we link Alusus libraries to our UnitTests project.
target_link_libraries(AlususTests AlususCoreLib AlususStorage "dl")

add_test(NAME AlususTests
  COMMAND AlususTests
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties(AlususTests PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/Lib;ALUSUS_LIBS=${AlususSrt_SOURCE_DIR}")
