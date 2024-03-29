# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(Alusus)
cmake_minimum_required(VERSION 3.20.0)

# Read version info.
if (NOT DEFINED Python3_EXECUTABLE)
  # Only find Python executable once and cache it. Calling it twice will result in an
  # index out of bound error.
  #
  # E.g.:
  #
  # CMake Error at /usr/share/cmake-3.25/Modules/FindPython/Support.cmake:1887 (list):
  # list index: 10 out of range (-10, 9)

  find_package(Python3 COMPONENTS Interpreter)
  set(Python3_EXECUTABLE "${Python3_EXECUTABLE}" CACHE STRING
    "Python 3 executable")
endif ()
execute_process(COMMAND ${Python3_EXECUTABLE} version_info.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Tools/BuildSrc
                OUTPUT_VARIABLE VersionInfo)
string(REGEX MATCH "VERSION: ([0-9]+\\.[0-9]+\\.[0-9]+)" _dummy_ ${VersionInfo})
set(AlususVersion ${CMAKE_MATCH_1})
string(REGEX MATCH "^[0-9]+\\.[0-9]+" AlususShlibVersion ${AlususVersion})
string(REGEX MATCH "REVISION: ([A-Za-z0-9]+)" _dummy_ ${VersionInfo})
set(AlususRevision ${CMAKE_MATCH_1})
if (NOT ${AlususRevision} STREQUAL "")
  set(AlususRevision "-${AlususRevision}")
endif()
string(REGEX MATCH "DATE: ([0-9]+-[0-9]+-[0-9]+)" _dummy_ ${VersionInfo})
set(AlususReleaseDate ${CMAKE_MATCH_1})
string(REGEX MATCH "HIJRI_DATE: ([0-9]+-[0-9]+-[0-9]+)" _dummy_ ${VersionInfo})
set(AlususHijriReleaseDate ${CMAKE_MATCH_1})
message(STATUS "Alusus Version: " ${AlususVersion})
message(STATUS "Alusus Revision: " ${AlususRevision})
message(STATUS "Alusus Shlib Version: " ${AlususShlibVersion})
message(STATUS "Alusus Release Date: " ${AlususReleaseDate})
message(STATUS "Alusus Hijri Release Date: " ${AlususHijriReleaseDate})

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ALUSUS_VERSION="${AlususVersion}"
                                                           ALUSUS_REVISION="${AlususRevision}"
                                                           ALUSUS_RELEASE_DATE="${AlususReleaseDate}"
                                                           ALUSUS_HIJRI_RELEASE_DATE="${AlususHijriReleaseDate}")
STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

set(ALUSUS_BIN_DIR_NAME "bin" CACHE STRING "The Alusus \"bin\" directory name")
set(ALUSUS_LIB_DIR_NAME "lib" CACHE STRING "The Alusus \"lib\" directory name")
set(ALUSUS_INCLUDE_DIR_NAME "include" CACHE STRING "The Alusus \"include\" directory name")

if (NOT WIN32)
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(FPIC "-fPIC")
  else ()
    set(FPIC "-fpic")
  endif ()
endif ()


# Prepare compile and link flags.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS "Using Clang")
  # CLang options
  set(Alusus_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-const-variable -Wno-unused-result -fno-rtti")
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC options
  message(STATUS "Using GCC")
  set(Alusus_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-but-set-parameter -Wunused-but-set-variable -Wno-unused-result -fno-rtti")
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # macOS has a different mechanism for RPATH than Linux and requires special
  # handling. More information can be found here:
  #
  # https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
  #
  # The following article also has a good explanation:
  #
  # https://blogs.oracle.com/dipol/dynamic-libraries,-rpath,-and-mac-os
  set(CMAKE_MACOSX_RPATH TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,@executable_path")
else()
  set(CMAKE_SKIP_RPATH TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,'$ORIGIN',--enable-new-dtags")
endif()

# Clear CMAKE_CXX_FLAGS_* because we are going to provide our own compilation
# flags here.
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS "")

# Incorporate the CMake features provided by LLVM.
find_package(LLVM)

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
add_subdirectory(Srt)
add_subdirectory(Core)
add_subdirectory(Spp)
add_subdirectory(Tests)
