# Copyright (C) 2014 Rafid Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususScgTests)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususScgTests_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")

# Header files path for Boost libraries
include_directories("${BOOST_PATH}")

# Header and library files paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
link_directories("${LLVM_LIBRARY_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
include_directories("${AlususScg_SOURCE_DIR}")
include_directories("${AlususScgTests_SOURCE_DIR}")

# Header files for CATCH.
include_directories("${CATCH_PATH}")

# Place files in folders for IDEs (though I only tried Visual Studio at the moment.)
file(GLOB AlususScgTests_Source_Files *.cpp)
file(GLOB AlususScgTests_Header_Files *.h)
source_group("SourceFiles\\General" FILES ${AlususScgTests_Source_Files})
source_group("HeaderFiles\\General" FILES ${AlususScgTests_Header_Files})

if(MSVC)
  add_definitions("/wd4005 /wd4146 /wd4355 /wd4800 /wd4996")
endif(MSVC)

set(AlususScgTestsSourceFiles
  ${AlususScgTests_Source_Files}
  ${AlususScgTests_Header_Files}
  )

add_executable(AlususScgTests ${AlususScgTestsSourceFiles})
set_target_properties(AlususScgTests PROPERTIES COMPILE_FLAGS "${AlususScgTests_COMPILE_FLAGS}")

# Set output names.
set_target_properties(AlususScgTests PROPERTIES OUTPUT_NAME alusus_scg_tests)
set_target_properties(AlususScgTests PROPERTIES DEBUG_OUTPUT_NAME alusus_scg_tests_d)
set_target_properties(AlususScgTests PROPERTIES VERSION ${AlususVersion})

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core jit native)

# Finally, we link Alusus libraries to our UnitTests project.
target_link_libraries(AlususScgTests AlususCoreLib)
target_link_libraries(AlususScgTests AlususScg)
target_link_libraries(AlususScgTests ${REQ_LLVM_LIBRARIES})

add_test(NAME AlususScgTests COMMAND AlususScgTests)
set_tests_properties(AlususScgTests PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususScg_BINARY_DIR}")

