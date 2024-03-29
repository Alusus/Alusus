# Copyright (C) 2022 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/license.html>.

project(AlususCore)
cmake_minimum_required(VERSION 3.20.0)

# Prepare compile flags.
set(AlususCore_COMPILE_FLAGS "${Alusus_COMPILE_FLAGS} -fvisibility=hidden")

# Header and library files paths for AlususSrl
include_directories("${AlususSrl_SOURCE_DIR}")

# Make sure the compiler finds the source files.
include_directories("${AlususCore_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${AlususCore_SOURCE_DIR}")

# Add a target for Alusus global storage.
set(AlususStorage_Source_Files core_global_storage.cpp core_global_storage.h)
add_library(AlususStorage SHARED ${AlususStorage_Source_Files})
set_target_properties(AlususStorage PROPERTIES COMPILE_FLAGS "${FPIC} ${AlususCore_COMPILE_FLAGS}")

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
add_library(AlususCoreLib STATIC ${AlususCoreLib_Source_Files})
set_target_properties(AlususCoreLib PROPERTIES COMPILE_FLAGS "${FPIC} ${AlususCore_COMPILE_FLAGS}")
target_precompile_headers(AlususCoreLib PRIVATE "core.h")

# Add a target for Alusus core executable.
set(AlususCore_Source_Files start.cpp interop.cpp)
add_executable(AlususCore ${AlususCore_Source_Files})
set_target_properties(AlususCore PROPERTIES COMPILE_FLAGS "${AlususCore_COMPILE_FLAGS}")
target_precompile_headers(AlususCore PRIVATE "core.h")

# Finally, we link the executable to the libraries.
target_link_libraries(AlususCore AlususSrlLib AlususCoreLib AlususStorage "dl")

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
set_target_properties(AlususStorage PROPERTIES SHLIBVERSION ${AlususShlibVersion})
set_target_properties(AlususCoreLib PROPERTIES OUTPUT_NAME alusus_core)
set_target_properties(AlususCoreLib PROPERTIES DEBUG_OUTPUT_NAME alusus_core.dbg)
set_target_properties(AlususCoreLib PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususCore PROPERTIES OUTPUT_NAME alusus)
set_target_properties(AlususCore PROPERTIES DEBUG_OUTPUT_NAME alusus.dbg)
set_target_properties(AlususCore PROPERTIES VERSION ${AlususVersion})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # macOS has a different mechanism for RPATH than Linux and requires special
  # handling. More information can be found here:
  #
  # https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
  #
  # The following article also has a good explanation:
  #
  # https://blogs.oracle.com/dipol/dynamic-libraries,-rpath,-and-mac-os
  set_target_properties(AlususCore PROPERTIES LINK_FLAGS "-Wl,-rpath,@executable_path,-rpath,@executable_path/../${ALUSUS_LIB_DIR_NAME}/")
else()
  set_target_properties(AlususCore PROPERTIES LINK_FLAGS "-Wl,-rpath,'$ORIGIN',-rpath,'$ORIGIN/../${ALUSUS_LIB_DIR_NAME}/'")
endif()

# Copy libary header files to installation directory.
install_files("/${ALUSUS_INCLUDE_DIR_NAME}/Core" FILES core.h core_global_storage.h)
foreach (DIR ${AlususCoreLib_Source_Subdirs})
  file(GLOB headers ${DIR}/*.h)
  install_files("/${ALUSUS_INCLUDE_DIR_NAME}/Core/${DIR}" FILES ${headers})
endforeach(DIR)

# Install library and executable files.
install(TARGETS AlususStorage AlususCoreLib AlususCore
  RUNTIME DESTINATION ${ALUSUS_BIN_DIR_NAME}
  LIBRARY DESTINATION ${ALUSUS_LIB_DIR_NAME}
  ARCHIVE DESTINATION ${ALUSUS_LIB_DIR_NAME}
)

# Add an install command to create the Arabic symbolic link.
if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "RELEASE")
  set(AlususArabicLink ${CMAKE_INSTALL_PREFIX}/${ALUSUS_BIN_DIR_NAME}/الأسس)
  set(AlususArabicLinkTarget alusus)
else()
  set(AlususArabicLink ${CMAKE_INSTALL_PREFIX}/${ALUSUS_BIN_DIR_NAME}/الأسس.نقح)
  set(AlususArabicLinkTarget alusus.dbg)
endif()
string(REPLACE "//" "/" AlususArabicLink ${AlususArabicLink})
string(REPLACE "//" "/" AlususArabicLinkTarget ${AlususArabicLinkTarget})
install(CODE "execute_process(COMMAND \"ln\" \"-sf\" ${AlususArabicLinkTarget} ${AlususArabicLink})")
