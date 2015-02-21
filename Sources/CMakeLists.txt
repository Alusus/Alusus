# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(Alusus)
cmake_minimum_required(VERSION 2.8)

set(AlususVersion "0.1.1")
set(AlususSoVersion "0.1")
set(AlususReleaseYear "2015")
set(AlususHijriReleaseYear "1436")

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ALUSUS_VERSION="${AlususVersion}"
                                                           ALUSUS_RELEASE_YEAR="${AlususReleaseYear}"
                                                           ALUSUS_HIJRI_RELEASE_YEAR="${AlususHijriReleaseYear}")

# Boost path
set (BOOST_PATH "" CACHE PATH "Path to Boost libraries root folder.")
if (NOT EXISTS "${BOOST_PATH}/boost")
  message(SEND_ERROR "Please specify the path to boost libraries in BOOST_PATH.")
else()
  message(STATUS "The following Boost path was provided: " ${BOOST_PATH} ".")
endif()

# CATCH path
set (CATCH_PATH "" CACHE PATH "Path to CATCH library root folder.")
if (NOT EXISTS "${CATCH_PATH}")
  message(SEND_ERROR "Please specify the path to CATCH Library in CATCH_PATH.")
else()
  message(STATUS "The following CATCH path was provided: " ${CATCH_PATH} ".")
endif()

# LLVM path
set(LLVM_PATH "" CACHE PATH "Path to LLVM root folder.")
if (NOT EXISTS "${LLVM_PATH}/include/llvm")
  message(SEND_ERROR "Please specify the path to LLVM root folder in LLVM_PATH.")
else()
  message(STATUS "The following LLVM path was provided: " ${LLVM_PATH} ".")
endif()

# Prepare compile and link flags.
STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
set(Alusus_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-but-set-parameter -Wunused-but-set-variable")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++1y")
else()
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++1y")
endif()
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,'$ORIGIN',--enable-new-dtags")

# Clear CMAKE_CXX_FLAGS_* because we are going to provide our own compilation
# flags here.
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS "")

# Incorporate the CMake features provided by LLVM:
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${LLVM_PATH}/share/llvm/cmake")
include(LLVMConfig)

# Macro for generating pre-compiled headers.
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${Alusus_SOURCE_DIR}")
include(precompiled_headers.cmake)

# Enable testing
enable_testing()

set (CUSTOM_OUTPUT_PATH "" CACHE PATH "Custom path to output all executable and library files. Useful during development.")
if (NOT CUSTOM_OUTPUT_PATH STREQUAL "")
  if (NOT EXISTS "${CUSTOM_OUTPUT_PATH}")
    message(SEND_ERROR "Invalid custom output path was provided: " ${CUSTOM_OUTPUT_PATH} ".")
  else()
    message(STATUS "The following custom output path was provided: " ${CUSTOM_OUTPUT_PATH} ".")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CUSTOM_OUTPUT_PATH}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CUSTOM_OUTPUT_PATH}")
  endif()
endif()

# Add projects
add_subdirectory(Core)
add_subdirectory(Tests/CoreTests)
add_subdirectory(Scg)
add_subdirectory(Srt)
add_subdirectory(Tests/ScgTests)
add_subdirectory(Tests/EndToEndTests)
