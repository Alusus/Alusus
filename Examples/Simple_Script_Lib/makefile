################################################################################
# Project:          Simple_Script Alusus Library
# Compiler used:    GNU GCC Compiler
# Author:			Sarmad Khalid Abdulla
# Creation Date:	2013-01-05
################################################################################


# This makefile provides two types of build, default and debug. The debug build
# contains debug information while the default build is used for the release
# compilation. Each build type has its own set of files and directories, so
# performing one type of build doesn't cause files of the other type to be
# deleted.
#
# The following are the list of rules to be called:
# default: Performs the default build.
# clean: Cleans the default build files including output files.
# clean_default: Alias for clean.
# clean_all: Clean all build files (intermediate and output) for both builds.
# clean_stackdump: Clean stackdump data, if any
# debug: Performs the debug build.
# debug_clean: Cleans the debug build files including output files.
# clean_debug: Alias for debug_clean.
# clean_intermediate: Cleans intermediate files (default and debug) without
#		removing output files.
# doc: Generate source documentation.

# CONFIGURING THE BUILDS:
# Configuring the makefile for the project requires the following:
# * Setting the project information variables in the "project_info.mk" file.
# 	this information includes file names and paths for source, object, as
# 	well as output files.
# * Setting the platform dependent variables. This includes names of tools
# 	used to perform the build.
# * Setting the $(OUTPUT_EXT) variable to the executable file extension for
# 	the operating system to build the project for.
# * Modifying the build flags.


#####################################
### Project file and directory names

include project_info.mk


###################################
### Platform dependent definitions

# build tools.
CC=gcc
CPP=g++
LD=g++
LIB=ar
RESCOMP=windres
# other system tools.
ifeq ($(OSTYPE),Windows)
  MD=mkdir
  RMFILE=del /q
  RMDIR=rmdir /s /q
  CPY=copy
else
  MD=mkdir -p
  RMFILE=rm -f
  RMDIR=rm -rf
  CPY=cp
endif
# executable files extension.
ifeq ($(OSTYPE),Windows)
  OUTPUT_EXT=.dll
else
  OUTPUT_EXT=.so
endif
# stackdump files extension.
STACKDUMP_EXT=.exe.stackdump
# compilation flags.
#CMAINFLAGS=-O2 -Wuninitialized -Winit-self -Wextra -Wparentheses -Wsequence-point -Wunused-variable -Wunused-value -Wsign-compare -Wredundant-decls -Wstrict-aliasing
CMAINFLAGS=-Wuninitialized -Winit-self -Wextra -Wparentheses -Wsequence-point -Wunused-variable -Wunused-value -Wsign-compare -Wredundant-decls -Wstrict-aliasing -fpic -fvisibility=hidden -std=c++11
# additional compilation flags.
CFLAGS=


#########################
### Variable definitions

# Variables with "debug_" prefix are used for the debug build, variables
# without "debug_" are used for the default build.

# Output files.
platform_dependent_name = $(output)$(OUTPUT_EXT)
debug_platform_dependent_name = $(debug_output)$(OUTPUT_EXT)
platform_dependent_output = $(output_dir)/$(output)$(OUTPUT_EXT)
debug_platform_dependent_output = $(output_dir)/$(debug_output)$(OUTPUT_EXT)
# The list of object files with their paths.
objs = $(patsubst %.cpp, %.o, $(patsubst ./%, $(objs_dir)/%, $(sources)))
debug_objs = $(patsubst %.cpp, %.o, $(patsubst ./%, $(debug_objs_dir)/%, $(sources)))
# The list of precompiled header object files with their paths.
pcho = $(patsubst %.h, %.h.gch, $(patsubst ./%, $(objs_dir)/%, $(pchs)))
debug_pcho = $(patsubst %.h, %.h.gch, $(patsubst ./%, $(debug_objs_dir)/%, $(pchs)))
# The list of dependency files with their paths.
deps = $(patsubst %.cpp, %.d, $(patsubst ./%, $(objs_dir)/%, $(sources)))
debug_deps = $(patsubst %.cpp, %.d, $(patsubst ./%, $(debug_objs_dir)/%, $(sources)))
# The list of precompiled header dependency files with their paths.
pch_deps = $(patsubst %.h, %.h.d, $(patsubst ./%, $(objs_dir)/%, $(pchs)))
debug_pch_deps = $(patsubst %.h, %.h.d, $(patsubst ./%, $(debug_objs_dir)/%, $(pchs)))
# The list of all object directories (used to create the object directories).
objs_dirs = "$(objs_dir)" $(patsubst ./%, "$(objs_dir)/%", $(source_dirs))
debug_objs_dirs = "$(debug_objs_dir)" $(patsubst ./%, "$(debug_objs_dir)/%", $(source_dirs))
# The list of all dependency directories with /*.d appended (used to include
# all the dependency files).
deps_dirs = $(objs_dir)/*.d $(patsubst ./%, $(objs_dir)/%/*.d, $(source_dirs))
debug_deps_dirs = $(debug_objs_dir)/*.d $(patsubst ./%, $(debug_objs_dir)/%/*.d, $(source_dirs))
# The list of compiler options used to add the object directory to the include
# path. This is needed for the precompiled headers to be detected by the
# compiler.
pch_dirs = -I$(objs_dir) -I.
debug_pch_dirs = -I$(debug_objs_dir) -I.
# The listof compiler options used to add third party dependencies to the include and lib paths.
third_party_include = -I../../Include/Core
third_party_lib = -L../../Lib -lalusus
debug_third_party_lib = -L../../Lib -lalusus_d


##########
### Rules

.PHONY: all clean_all \
	default_clean clean_default debug_clean clean_debug clean_intermediate clean_stackdump \
	default_temp_dirs debug_temp_dirs \
	default debug

all: default

clean: clean_all

# Clean all build files (intermediate and output) for both builds.
clean_all: clean_stackdump
	@echo "Cleaning all target and temp files"
	-@$(RMFILE) "$(platform_dependent_output)"
	-@$(RMFILE) "$(debug_platform_dependent_output)"
	-@$(RMDIR) "$(objs_dir)"
	-@$(RMDIR) "$(debug_objs_dir)"
	-@$(RMFILE) "../Temp/doxygen.log"
	@echo "Clean all complete..."

# Clean all intermediate files for both builds.
clean_intermediate: clean_stackdump
	@echo "Cleaning all temp files"
	-@$(RMDIR) "$(objs_dir)"
	-@$(RMDIR) "$(debug_objs_dir)"
	-@$(RMFILE) "../Temp/doxygen.log"
	@echo "Intermediate clean complete..."

# Clean stackdump data, if any
clean_stackdump:
	-@$(RMFILE) "*$(STACKDUMP_EXT)"


### default build

default: default_temp_dirs $(platform_dependent_output)

clean_default: default_clean

# Clean intermediate and output files.
default_clean:
	@echo "Cleaning default target and temp files"
	-@$(RMFILE) "$(platform_dependent_output)"
	-@$(RMDIR) "$(objs_dir)"
	@echo "Default clean complete..."

# Create object directories.
default_temp_dirs:
	-@$(MD) $(objs_dirs)

# Build the output file.
$(platform_dependent_output) : $(pch_deps) $(deps) $(pcho) $(objs)
	@echo "Linking default executable $(platform_dependent_output)"
	@$(LD) -shared -Wl,-soname,$(platform_dependent_name) -o $(platform_dependent_output) $(objs) $(third_party_lib) -fvisibility=hidden
	@echo "Default build complete..."

# Build precompiled headers.
$(objs_dir)/%.h.gch : %.h
	@echo "Compiling (default) precompiled header $<"
	@$(CPP) $(CMAINFLAGS) $(CFLAGS) -DNDEBUG -DRELEASE -MMD -fpch-deps -O2 -c $< -o $@ -iquote $(pch_dirs) $(third_party_include)

# Build the object files.
$(objs_dir)/%.o : %.cpp
	@echo "Compiling (default) $<"
	@$(CPP) $(CMAINFLAGS) $(CFLAGS) -DNDEBUG -DRELEASE -MMD -fpch-deps -O2 -c $< -o $@ -iquote $(pch_dirs) $(third_party_include)

# Remove the precompiled header object file (to force it to be rebuilt) if
# the related dependency file was not found.
$(objs_dir)/%.h.d :
	-@$(RMFILE) $(patsubst %.d, "%.gch", $@)

# Remove the object file (to force it to be rebuilt) if the related dependency
# file was not found.
$(objs_dir)/%.d :
	-@$(RMFILE) $(patsubst %.d, "%.o", $@)


### debug build

debug: debug_temp_dirs $(debug_platform_dependent_output)

clean_debug: debug_clean

# Clean intermediate and output files.
debug_clean:
	@echo "Cleaning debug target and temp files"
	-@$(RMFILE) "$(debug_platform_dependent_output)"
	-@$(RMDIR) "$(debug_objs_dir)"
	@echo "Debug clean complete..."

# Create object directories.
debug_temp_dirs:
	-@$(MD) $(debug_objs_dirs)

# Build the output file.
$(debug_platform_dependent_output) : $(debug_pch_deps) $(debug_deps) $(debug_pcho) $(debug_objs)
	@echo "Linking executable $(debug_platform_dependent_output)"
	@$(LD) -pg -shared -Wl,-soname,$(platform_dependent_name) -o $(debug_platform_dependent_output) $(debug_objs) $(debug_third_party_lib) -fvisibility=hidden
	@echo "Build complete..."

# Build precompiled headers.
$(debug_objs_dir)/%.h.gch : %.h
	@echo "Compiling (debug) precompiled header $<"
	@$(CPP) $(CMAINFLAGS) $(CFLAGS) -pg -DDEBUG -MMD -fpch-deps -g -c $< -o $@ -iquote $(debug_pch_dirs) $(third_party_include)

# Build the object files.
$(debug_objs_dir)/%.o : %.cpp
	@echo "Compiling (debug) $<"
	@$(CPP) $(CMAINFLAGS) $(CFLAGS) -pg -DDEBUG -MMD -fpch-deps -g -c $< -o $@ -iquote $(debug_pch_dirs) $(third_party_include)

# Remove the precompiled header object file (to force it to be rebuilt) if
# the related dependency file was not found.
$(debug_objs_dir)/%.h.d :
	-@$(RMFILE) $(patsubst %.d, "%.gch", $@)

# Remove the object file (to force it to be rebuilt) if the related dependency
# file was not found.
$(debug_objs_dir)/%.d :
	-@$(RMFILE) $(patsubst %.d, "%.o", $@)


### auto dependencies

# Include all dependency files (*.d), if available, in order to implement
# auto dependencies.
-include $(deps_dirs)
-include $(debug_deps_dirs)
