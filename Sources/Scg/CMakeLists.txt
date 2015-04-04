# Copyright (C) 2014 Rafid Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <http://alusus.net/alusus_license_1_0>.

project(AlususScg)
cmake_minimum_required(VERSION 2.8)

# Header files path for Boost libraries
include_directories("${BOOST_PATH}")

# Header and library files paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
link_directories("${LLVM_LIBRARY_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Header and library files paths for AlususCore
include_directories("${AlususCore_SOURCE_DIR}")

# Let's suppose we want to build a JIT compiler with support for
# binary code (no interpreter):
llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core jit native linker)

# Make sure the compiler finds the source files.
include_directories("${AlususScg_SOURCE_DIR}")

# Create the list of files to be added to the project.
set(AlususScg_SOURCE_SUBDIRS BuiltInFunctions CodeGeneration Containers DebuggingHelpers Functions Instructions Memory Operators Types Values ParsingHandlers)
set(AlususScg_SOURCE_FILES
  scg.h
  exceptions.h
  Expression.cpp
  Expression.h
  LibraryGateway.cpp
  LibraryGateway.h
  LlvmContainer.cpp
  LlvmContainer.h
  llvm_fwd.h
  Logger.cpp
  Logger.h
  macros.h
  main.cpp
  prerequisites.h
  typedefs.h
  )
foreach (dir ${AlususScg_SOURCE_SUBDIRS})
  file(GLOB headers ${dir}/*.h)
  file(GLOB sources ${dir}/*.cpp)
  # Place files in folders for IDEs (only tested with  Visual Studio and Eclipse at the moment.)
  source_group("HeaderFiles\\${dir}" FILES ${headers})
  source_group("SourceFiles\\${dir}" FILES ${sources})
  set(AlususScg_SOURCE_FILES ${AlususScg_SOURCE_FILES} ${sources} ${headers})
endforeach(dir)

if(MSVC)
  add_definitions("/wd4005 /wd4146 /wd4355 /wd4800 /wd4996")
endif(MSVC)

# Create AlususScg library project.
add_library(AlususScg SHARED ${AlususScg_SOURCE_FILES})
set_target_properties(AlususScg PROPERTIES COMPILE_FLAGS "${Alusus_COMPILE_FLAGS}")
ADD_PRECOMPILED_HEADER(AlususScg "precompiled_headers.h")

# Set output names.
set_target_properties(AlususScg PROPERTIES OUTPUT_NAME alusus_scg)
set_target_properties(AlususScg PROPERTIES DEBUG_OUTPUT_NAME alusus_scg.dbg)
set_target_properties(AlususScg PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususScg PROPERTIES SOVERSION ${AlususSoVersion})

# Finally, we link the LLVM and AlususCore libraries to our library.
target_link_libraries(AlususScg ${REQ_LLVM_LIBRARIES})
target_link_libraries(AlususScg AlususCoreLib)
target_link_libraries(AlususScg AlususStorage)

# Copy libary header files to installation directory.
install_files("/Include/Scg" FILES
  exceptions.h
  LlvmContainer.h
  llvm_fwd.h
  Logger.h
  macros.h
  scg.h
  prerequisites.h
  typedefs.h
  LibraryGateway.h
  )
foreach (dir ${AlususScg_SOURCE_SUBDIRS})
  file(GLOB headers ${dir}/*.h)
  install_files("/Include/Scg/${dir}" FILES ${headers})
endforeach(dir)

install(TARGETS AlususScg
  RUNTIME DESTINATION Bin
  LIBRARY DESTINATION Lib
  ARCHIVE DESTINATION Lib
)

