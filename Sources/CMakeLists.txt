# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(Alusus)
cmake_minimum_required(VERSION 2.8)

# Read version info from version file.
execute_process(COMMAND bash version_info.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Tools/
                OUTPUT_VARIABLE VersionInfo)
string(REGEX MATCH "VERSION: ([0-9]+\\.[0-9]+\\.[0-9]+)" _dummy_ ${VersionInfo})
set(AlususVersion ${CMAKE_MATCH_1})
string(REGEX MATCH "^[0-9]+\\.[0-9]+" AlususSoVersion ${AlususVersion})
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
message(STATUS "Alusus So Version: " ${AlususSoVersion})
message(STATUS "Alusus Release Date: " ${AlususReleaseDate})
message(STATUS "Alusus Hijri Release Date: " ${AlususHijriReleaseDate})

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ALUSUS_VERSION="${AlususVersion}"
                                                           ALUSUS_REVISION="${AlususRevision}"
                                                           ALUSUS_RELEASE_DATE="${AlususReleaseDate}"
                                                           ALUSUS_HIJRI_RELEASE_DATE="${AlususHijriReleaseDate}")

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
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
else()
  set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
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
add_subdirectory(Spp)
#add_subdirectory(Srt)
add_subdirectory(Tests/EndToEndTests)
