################################################################################
# File:				Project Info
# Description:		Contains names of files and directories used by the project.
# 					This file is included by makefile.
# Author:			Sarmad Khalid Abdulla
# Creation Date:	2007-06-05
################################################################################


# NOTES:
# 1- All paths to files or directories should be relative to the current
#    directory, which is the directory where 'makefile' is.
# 2- Directories should not end with /


###############
# Output files

# NOTE: Output filenames should not contain the extension since the extension is
# 		system dependent.

output_dir=.
output=simple_script
debug_output=simple_script_d


###############
# Source files

# NOTE: All paths to source files or directories should start with ./

# $(source_dirs) should contain the paths to folders containing source code
# of the library.
# Any subdirectory mentioned in $(sources) or $(pchs) should be included.
source_dirs=

# The list of all source filesof the core. All files should have .cpp extension.
sources = $(wildcard ./*.cpp) $(foreach dir, $(source_dirs), $(wildcard $(dir)/*.cpp))

# The list of header files to be precompiled. They should have .h extension.
pchs=./main.h


#####################
# Intermediate files

# NOTE: These directories should NOT start with ./ even if they are
# 		subdirectories of the current directory.

objs_dir=./objs
debug_objs_dir=./dobjs
