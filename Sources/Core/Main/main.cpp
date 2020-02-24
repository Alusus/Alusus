/**
 * @file Core/Main/main.cpp
 * Contains the global implementations of Main namespace's declarations.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#if __APPLE__
  #include <mach-o/dyld.h>
#endif
#ifdef WINDOWS
  #include <direct.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #include <limits.h>
#endif

namespace Core::Main
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
    throw EXCEPTION(GenericException, S("Couldn't obtain the current working directory."));
  }

  std::string path(currentPath.data());
  if (path.back() != C('/')) path += C('/');
  return path;
}


std::string getModuleDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  #ifdef WINDOWS
    std::string path(currentPath.data(), GetModuleFileName(NULL, currentPath.data(), currentPath.size()));
  #elif __APPLE__
    uint32_t size = FILENAME_MAX;
    // TODO: Check the result.
    auto res = _NSGetExecutablePath(currentPath.data(), &size);
    std::string path(currentPath.data());
  #else
    ssize_t count = readlink("/proc/self/exe", currentPath.data(), currentPath.size());
    std::string path(currentPath.data(), (count > 0) ? count : 0);
  #endif

  Int pos = path.rfind(C('/'));
  return std::string(path, 0, pos+1);
}

} // namespace
