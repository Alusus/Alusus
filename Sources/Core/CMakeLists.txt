# Copyright (C) 2019 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/alusus_license_1_0>.

project(AlususCore)
cmake_minimum_required(VERSION 2.8)

# Prepare compile flags.
set(AlususCore_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS RELEASE)
else()
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS USE_LOGS)
endif()

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususCore_SOURCE_DIR}")

# Add a target for Alusus global storage.
set(AlususStorage_Source_Files core_global_storage.cpp core_global_storage.h)
add_library(AlususStorage SHARED ${AlususStorage_Source_Files})
set_target_properties(AlususStorage PROPERTIES COMPILE_FLAGS "-fpic ${AlususCore_COMPILE_FLAGS}")
if(MINGW)
  target_link_libraries(AlususStorage Win32Helpers)
endif()

# Add a target for Alusus core library.
set(AlususCoreLib_Source_Subdirs Basic Data Data/Grammar Data/Ast Notices Processing Processing/Handlers Main)
set(AlususCoreLib_Source_Files core.h)
foreach (DIR ${AlususCoreLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  file(GLOB sources ${DIR}/*.cpp)
  source_group("HeaderFiles\\${DIR}" FILES ${headers})
  source_group("SourceFiles\\${DIR}" FILES ${sources})
  set(AlususCoreLib_Source_Files ${AlususCoreLib_Source_Files} ${sources} ${headers})
endforeach(DIR)
add_library(AlususCoreLib SHARED ${AlususCoreLib_Source_Files})
set_target_properties(AlususCoreLib
  PROPERTIES
  COMPILE_FLAGS "-fpic ${AlususCore_COMPILE_FLAGS}"
  COTIRE_CXX_PREFIX_HEADER_INIT "core.h"
)
if(MINGW)
  target_link_libraries(AlususCoreLib Win32Helpers)
endif()
target_link_libraries(AlususCoreLib AlususStorage "stdc++fs" "dl")
cotire(AlususCoreLib)

# Add a target for Alusus core executable.
set(AlususCore_Source_Files start.cpp)
add_executable(AlususCore ${AlususCore_Source_Files})
set_target_properties(AlususCore
  PROPERTIES
  COMPILE_FLAGS "${AlususCore_COMPILE_FLAGS}"
  COTIRE_CXX_PREFIX_HEADER_INIT "core.h"
)

# Finally, we link the executable to the libraries.
if(MINGW)
  target_link_libraries(AlususCore Win32Helpers)
endif()
target_link_libraries(AlususCore AlususCoreLib "LLVM-7" "stdc++fs")
cotire(AlususCore)

# Set library and executable output names.
# NOTE: For alusus_storage, we'll use the same filename for both debug and
#       release versions. This is important in cases where the user is running
#       the debug version of the core and is including a release version of a
#       library that also need to link to alusus_storage. We should only have
#       one alusus_storage loaded so that there is only one global storage.
#       Having two separate global storages may result in undefined behavior.
set_target_properties(AlususStorage PROPERTIES OUTPUT_NAME alusus_storage)
set_target_properties(AlususStorage PROPERTIES DEBUG_OUTPUT_NAME alusus_storage)
set_target_properties(AlususStorage PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususStorage PROPERTIES SOVERSION ${AlususSoVersion})
set_target_properties(AlususCoreLib PROPERTIES OUTPUT_NAME alusus)
set_target_properties(AlususCoreLib PROPERTIES DEBUG_OUTPUT_NAME alusus.dbg)
set_target_properties(AlususCoreLib PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususCore PROPERTIES OUTPUT_NAME alusus)
set_target_properties(AlususCore PROPERTIES DEBUG_OUTPUT_NAME alusus.dbg)
set_target_properties(AlususCore PROPERTIES VERSION ${AlususVersion})
if (NOT MINGW)
  set_target_properties(AlususCore PROPERTIES LINK_FLAGS "-Wl,-rpath=$ORIGIN -Wl,-rpath=$ORIGIN/../Lib/")
endif()

# Copy libary header files to installation directory.
install_files("/Include/Core" FILES core.h core_global_storage.h)
foreach (DIR ${AlususCoreLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  install_files("/Include/Core/${DIR}" FILES ${headers})
endforeach(DIR)

# Install library and executable files.
if(MINGW)
  install(TARGETS AlususStorage AlususCoreLib AlususCore
    RUNTIME DESTINATION Bin
    LIBRARY DESTINATION Bin
    ARCHIVE DESTINATION Bin
  )
else()
  install(TARGETS AlususStorage AlususCoreLib AlususCore
    RUNTIME DESTINATION Bin
    LIBRARY DESTINATION Lib
    ARCHIVE DESTINATION Lib
  )
endif()

# Add an install command to create the Arabic symbolic link.
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set(AlususArabicLink ${CMAKE_INSTALL_PREFIX}/Bin/الأسس)
  set(AlususArabicLinkTarget ${CMAKE_INSTALL_PREFIX}/Bin/alusus)
else()
  set(AlususArabicLink ${CMAKE_INSTALL_PREFIX}/Bin/الأسس.نقح)
  set(AlususArabicLinkTarget ${CMAKE_INSTALL_PREFIX}/Bin/alusus.dbg)
endif()
string(REPLACE "//" "/" AlususArabicLink ${AlususArabicLink})
string(REPLACE "//" "/" AlususArabicLinkTarget ${AlususArabicLinkTarget})
install(CODE "execute_process(COMMAND \"ln\" \"-sfr\" ${AlususArabicLinkTarget} ${AlususArabicLink})")
