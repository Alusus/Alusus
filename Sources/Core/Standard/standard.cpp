/**
 * @file Core/Standard/standard.cpp
 * Contains the global implementations of Standard namespace's declarations.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
  #include <direct.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #include <limits.h>
#endif

namespace Core { namespace Standard
{

using namespace Data;

#ifdef WINDOWS
  #define _getWorkingDirectory _getcwd
#else
  #define _getWorkingDirectory getcwd
#endif


//==============================================================================
// Global Functions

std::string getWorkingDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  if (!_getWorkingDirectory(currentPath.data(), currentPath.size()))
  {
    throw EXCEPTION(GenericException, STR("Couldn't obtain the current working directory."));
  }

  std::string path(currentPath.data());
  if (path.back() != CHR('/')) path += CHR('/');
  return path;
}


std::string getModuleDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  #ifdef WINDOWS
    std::string path(currentPath.data(), GetModuleFileName(NULL, currentPath.data(), currentPath.size()));
  #else
    ssize_t count = readlink("/proc/self/exe", currentPath.data(), currentPath.size());
    std::string path(currentPath.data(), (count > 0) ? count : 0);
  #endif

  Int pos = path.rfind(CHR('/'));
  return std::string(path, 0, pos+1);
}

} } // namespace
