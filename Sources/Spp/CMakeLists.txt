# Copyright (C) 2019 Sarmad Khalid Abdullah
#
# This file is released under Alusus Public License, Version 1.0.
# For details on usage and copying conditions read the full license in the
# accompanying license file or at <https://alusus.org/alusus_license_1_0>.

project(AlususSpp)
cmake_minimum_required(VERSION 2.8)

# Header paths for LLVM
include_directories("${LLVM_INCLUDE_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

# Header and library files paths for AlususCore
include_directories("${AlususCore_SOURCE_DIR}")

# Let's suppose we want to build a JIT compiler with support for
# binary code (no interpreter):

# Make sure the compiler finds the source files.
include_directories("${AlususSpp_SOURCE_DIR}")

# Create the list of files to be added to the project.
set(AlususSpp_SOURCE_SUBDIRS Ast Notices Handlers CodeGen LlvmCodeGen)
set(AlususSpp_SOURCE_FILES
  spp.h
  LibraryGateway.h
  LibraryGateway.cpp
  ExecutionContext.h
  SeekerExtension.h
  SeekerExtension.cpp
  RootScopeHandlerExtension.h
  RootScopeHandlerExtension.cpp
  RootManagerExtension.h
  RootManagerExtension.cpp
  GrammarFactory.h
  GrammarFactory.cpp
  main.cpp
  )
foreach (dir ${AlususSpp_SOURCE_SUBDIRS})
  file(GLOB headers ${dir}/*.h)
  file(GLOB sources ${dir}/*.cpp)
  # Place files in folders for IDEs (only tested with  Visual Studio and Eclipse at the moment.)
  source_group("HeaderFiles\\${dir}" FILES ${headers})
  source_group("SourceFiles\\${dir}" FILES ${sources})
  set(AlususSpp_SOURCE_FILES ${AlususSpp_SOURCE_FILES} ${sources} ${headers})
endforeach(dir)

if(MSVC)
  add_definitions("/wd4005 /wd4146 /wd4355 /wd4800 /wd4996")
endif(MSVC)

# Create AlususSpp library project.
add_library(AlususSpp SHARED ${AlususSpp_SOURCE_FILES})
set_target_properties(AlususSpp PROPERTIES COMPILE_FLAGS "${Alusus_COMPILE_FLAGS}")
add_precompiled_header(AlususSpp "spp.h")

# Set output names.
set_target_properties(AlususSpp PROPERTIES OUTPUT_NAME alusus_spp)
set_target_properties(AlususSpp PROPERTIES DEBUG_OUTPUT_NAME alusus_spp.dbg)
set_target_properties(AlususSpp PROPERTIES VERSION ${AlususVersion})
set_target_properties(AlususSpp PROPERTIES SOVERSION ${AlususSoVersion})

# Finally, we link the LLVM and AlususCore libraries to our library.
target_link_libraries(AlususSpp AlususCoreLib)

# Copy libary header files to installation directory.
install_files("/Include/Spp" FILES
  spp.h
  LibraryGateway.h
)
foreach (dir ${AlususSpp_SOURCE_SUBDIRS})
  file(GLOB headers ${dir}/*.h)
  install_files("/Include/Spp/${dir}" FILES ${headers})
endforeach(dir)

if(MINGW)
  install(TARGETS AlususSpp
    RUNTIME DESTINATION Bin
    LIBRARY DESTINATION Bin
    ARCHIVE DESTINATION Bin
  )
else()
  install(TARGETS AlususSpp
    RUNTIME DESTINATION Bin
    LIBRARY DESTINATION Lib
    ARCHIVE DESTINATION Lib
  )
endif()
