# This file provides information and services to end users who want to link
# against Alusus libraries. It's not needed to build Alusus from source code.

# The variable ALUSUS_PATH is assumed to be set by the including file and it
# should point to the root folder of Alusus.
set(ALUSUS_LIBRARY_DIR "${ALUSUS_PATH}/Lib")
set(ALUSUS_INCLUDE_DIR "${ALUSUS_PATH}/Include")

STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
if (${CMAKE_BUILD_TYPE_UPPER} STREQUAL "DEBUG")
  set(ALUSUS_CORE_LIB "alusus_d")
  set(ALUSUS_STORAGE_LIB "alusus_storage_d")
else()
  set(ALUSUS_CORE_LIB "alusus")
  set(ALUSUS_STORAGE_LIB "alusus_storage")
endif()

