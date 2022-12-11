# Copyright (C) 2021 Rafid Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(AlususTests)
cmake_minimum_required(VERSION 3.20)

# Prepare compile flags.
set(AlususTests_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")

# Header and library files paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
link_directories("${LLVM_LIBRARY_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Make sure the compiler finds the source files.
include_directories("${AlususSrl_SOURCE_DIR}")
include_directories("${AlususCore_SOURCE_DIR}")
include_directories("${AlususTests_SOURCE_DIR}")

# Place files in folders for IDEs (though I only tried Visual Studio at the moment.)
file(GLOB AlususTests_Source_Files *.cpp ../Core/interop.cpp)
file(GLOB AlususTests_Header_Files *.h)
file(GLOB AlususTests_Test_Files
  Core/*.alusus
  Spp/Parsing/*.alusus
  Spp/Building/*.alusus
  Spp/Running/*.alusus
  Arabic/*.أسس
  Srt/*.alusus
  Srt/*.أسس
  Srt/Srl/*.alusus
  Srt/مـتم/*.أسس
)
file(GLOB AlususTests_Test_Files_Output
  Core/*.output
  Spp/Parsing/*.output
  Spp/Building/*.output
  Spp/Running/*.output
  Arabic/*.output
  Srt/*.output
  Srt/Srl/*.output
  Srt/مـتم/*.output
)
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
target_link_libraries(AlususTests AlususSrlLib AlususCoreLib AlususStorage "dl")

# Build C++ interoperability test lib.
set(CppInteropTest_Source_Files cpp_interop_test.cpp)
add_library(CppInteropTest SHARED ${CppInteropTest_Source_Files})
set_target_properties(CppInteropTest PROPERTIES COMPILE_FLAGS "${FPIC} ${Alusus_COMPILE_FLAGS}")
set_target_properties(CppInteropTest PROPERTIES OUTPUT_NAME cpp_interop_test)
set_target_properties(CppInteropTest PROPERTIES DEBUG_OUTPUT_NAME cpp_interop_test)
set_target_properties(CppInteropTest PROPERTIES VERSION ${AlususVersion})
set_target_properties(CppInteropTest PROPERTIES SOVERSION ${AlususSoVersion})
target_link_libraries(CppInteropTest AlususSrlLib AlususCoreLib AlususStorage)

add_test(NAME Core
  COMMAND AlususTests "Core" ".alusus"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties(Core PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME "Spp/Parsing"
  COMMAND AlususTests "Spp/Parsing" ".alusus"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties("Spp/Parsing" PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME "Spp/Building"
  COMMAND AlususTests "Spp/Building" ".alusus"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties("Spp/Building" PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME "Spp/Running"
  COMMAND AlususTests "Spp/Running" ".alusus"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties("Spp/Running" PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME Arabic
  COMMAND AlususTests "Arabic" ".أسس" "ar"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties(Arabic PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME Srt
  COMMAND AlususTests "Srt" ".alusus"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties(Srt PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")

add_test(NAME مـتم
  COMMAND AlususTests "Srt" ".أسس" "ar"
  WORKING_DIRECTORY "${AlususTests_SOURCE_DIR}")
set_tests_properties(مـتم PROPERTIES
  ENVIRONMENT "LD_LIBRARY_PATH=${AlususCore_BINARY_DIR}:${AlususSpp_BINARY_DIR}:${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME};ALUSUS_LIBS=${CMAKE_INSTALL_PREFIX}/${ALUSUS_LIB_DIR_NAME}:${AlususSrt_SOURCE_DIR}:${AlususSpp_BINARY_DIR}:${CppInteropTest_BINARY_DIR}")
