# Source: https://gist.github.com/larsch/573926
# Some changes made by: Rafid K. Abdullah

# Macro for setting up precompiled headers. Usage:
#
#   add_precompiled_header(target header.h [FORCEINCLUDE])
#
# MSVC: A source file with the same name as the header must exist and
# be included in the target (E.g. header.cpp).
#
# MSVC: Add FORCEINCLUDE to automatically include the precompiled
# header file from every source file.
#
# GCC: The precompiled header is always automatically included from
# every header file.
#
# Copyright (C) 2009-2013 Lars Christensen <larsch@belunktum.dk>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the ‘Software’), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED ‘AS IS’, WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
 
MACRO(ADD_PRECOMPILED_HEADER _targetName _input)
  GET_FILENAME_COMPONENT(_inputWe ${_input} NAME_WE)
  SET(pch_source ${_inputWe}.cpp)
  FOREACH(arg ${ARGN})
    IF(arg STREQUAL FORCEINCLUDE)
      SET(FORCEINCLUDE ON)
    ELSE()
      SET(FORCEINCLUDE OFF)
    ENDIF()
  ENDFOREACH(arg)
 
  IF(MSVC)
    GET_TARGET_PROPERTY(sources ${_targetName} SOURCES)
    SET(_sourceFound FALSE)
    FOREACH(_source ${sources})
      SET(PCH_COMPILE_FLAGS "")
      IF(_source MATCHES \\.\(cc|cxx|cpp\)$)
        GET_FILENAME_COMPONENT(_sourceWe ${_source} NAME_WE)
        IF(_sourceWe STREQUAL ${_inputWe})
          SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /Yc${_input}")
          SET(_sourceFound TRUE)
        ELSE(_sourceWe STREQUAL ${_inputWe})
          SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /Yu${_input}")
          IF(FORCEINCLUDE)
            SET(PCH_COMPILE_FLAGS "${PCH_COMPILE_FLAGS} /FI${_input}")
          ENDIF(FORCEINCLUDE)
        ENDIF(_sourceWe STREQUAL ${_inputWe})
        SET_SOURCE_FILES_PROPERTIES(${_source} PROPERTIES COMPILE_FLAGS "${PCH_COMPILE_FLAGS}")
      ENDIF(_source MATCHES \\.\(cc|cxx|cpp\)$)
    ENDFOREACH()
    IF(NOT _sourceFound)
      MESSAGE(FATAL_ERROR "A source file for ${_input} was not found. Required for MSVC builds.")
    ENDIF(NOT _sourceFound)
  ENDIF(MSVC)
 
  IF(CMAKE_COMPILER_IS_GNUCXX)
    GET_FILENAME_COMPONENT(_name ${_input} NAME)
    SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_targetName}.dir/${_name}.gch")
    MAKE_DIRECTORY(${_outdir})
    SET(_output "${_outdir}/.c++")
    
    STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
    SET(_compiler_FLAGS ${${_flags_var_name}})
    
    GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
    FOREACH(item ${_directory_flags})
      LIST(APPEND _compiler_FLAGS "-I${item}")
    ENDFOREACH(item)
 
    GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
    LIST(APPEND _compiler_FLAGS ${_directory_flags})

    GET_TARGET_PROPERTY(_flags ${_targetName} COMPILE_FLAGS)
    SET(_commandArgs "${_compiler_FLAGS} ${_flags} -x c++-header -o ${_output} ${_source}")
    SEPARATE_ARGUMENTS(_commandArgs)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${CMAKE_CXX_COMPILER}
      ARGS ${_commandArgs}
      DEPENDS ${_source}
      VERBATIM )
    ADD_CUSTOM_TARGET(${_targetName}_gch DEPENDS ${_output})
    ADD_DEPENDENCIES(${_targetName} ${_targetName}_gch)
    SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${_flags} -include ${_name} -Winvalid-pch")
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)

  IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    GET_FILENAME_COMPONENT(_name ${_input} NAME)
    SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_targetName}/${_name}.gch")
    MAKE_DIRECTORY(${_outdir})
    SET(_output "${_outdir}")
    
    STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
    SET(_compiler_FLAGS ${${_flags_var_name}})
    
    GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
    FOREACH(item ${_directory_flags})
      LIST(APPEND _compiler_FLAGS "-I${item}")
    ENDFOREACH(item)
 
    GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
    LIST(APPEND _compiler_FLAGS ${_directory_flags})
 
    GET_TARGET_PROPERTY(_flags ${_targetName} COMPILE_FLAGS)
    SET(_commandArgs "${_compiler_FLAGS} ${_flags} -x c++-header -o ${_output} ${_source}")
    SEPARATE_ARGUMENTS(_commandArgs)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${CMAKE_CXX_COMPILER}
      ARGS ${_commandArgs}
      DEPENDS ${_source}
      VERBATIM )
    ADD_CUSTOM_TARGET(${_targetName}_gch DEPENDS ${_output})
    ADD_DEPENDENCIES(${_targetName} ${_targetName}_gch)
    SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${_flags} -include ${_name} -Winvalid-pch")
  ENDIF()
ENDMACRO()
