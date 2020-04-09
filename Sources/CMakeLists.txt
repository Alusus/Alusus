# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/alusus_license_1_0>.

project(Alusus)
cmake_minimum_required(VERSION 3.1)

if(MINGW)
  set(InstallLibDir "${CMAKE_INSTALL_PREFIX}/Bin")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Win32Helpers")
else()
  set(InstallLibDir "${CMAKE_INSTALL_PREFIX}/Lib")
endif()
link_directories(${InstallLibDir})

# Read version info from version file.

if(PYTHON_PREFIX)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} version_info.py ${PYTHON_PREFIX}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Tools/BuildSrc
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
endif()

# LLVM path
# set(LLVM_PATH "" CACHE PATH "Path to LLVM root folder.")
# if (NOT EXISTS "${LLVM_PATH}/include/llvm")
#  message(SEND_ERROR "Please specify the path to LLVM root folder in LLVM_PATH.")
# else()
#  message(STATUS "The following LLVM path was provided: " ${LLVM_PATH} ".")
# endif()

# DLFCN-32 include directory for Win32
if(MINGW)
  include_directories(${DLFCN_WIN32_PATH}/include)
endif()

# Prepare compile and link flags.
STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)

if (NOT MINGW)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # CLang options
    set(Alusus_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-const-variable -Wno-unused-result -fno-rtti")
    set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
    set(CMAKE_SKIP_RPATH TRUE)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,'$ORIGIN',--enable-new-dtags")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC options
    set(Alusus_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-but-set-parameter -Wunused-but-set-variable -Wno-unused-result -fno-rtti")
    set(Alusus_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -std=c++17")
    set(CMAKE_SKIP_RPATH TRUE)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,'$ORIGIN',--enable-new-dtags")
  endif()
else ()
  set(CXX_STD_FLAG "-std=c++17")
  set(Alusus_COMPILE_FLAGS "${CXX_STD_FLAG} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}} -Werror=return-type -Wunused-variable -Winit-self -Wuninitialized -Wunused-but-set-parameter -Wunused-but-set-variable -Wno-unused-result -fno-rtti")
  set(CMAKE_SKIP_RPATH TRUE)
endif()


# Clear CMAKE_CXX_FLAGS_* because we are going to provide our own compilation
# flags here.
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS "")

# Incorporate the CMake features provided by LLVM:
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${LLVM_PATH}/lib/cmake/llvm")
# include(LLVMConfig)

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
if(MINGW)
  add_subdirectory(Win32Helpers)
endif()
add_subdirectory(Core)
add_subdirectory(Spp)
add_subdirectory(Srt)
add_subdirectory(Tests)
