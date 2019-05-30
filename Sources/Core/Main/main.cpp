/**
 * @file Core/Main/main.cpp
 * Contains the global implementations of Main namespace's declarations.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#include <filesystem>
#if defined(__MINGW32__) || defined(__MINGW64__)
#include "win32api.h"
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace Core::Main
{

using namespace Data;

#if defined(__MINGW32__) || defined(__MINGW64__)
#define _wgetWorkingDirectory _wgetcwd
#else
#define _getWorkingDirectory getcwd
#endif


//==============================================================================
// Global Functions

std::string getWorkingDirectory()
{
  #if defined(__MINGW32__) || defined(__MINGW64__)
  thread_local static std::array<WChar,FILENAME_MAX> currentPath;
  if (!_wgetWorkingDirectory(currentPath.data(), currentPath.size()))
  #else
  thread_local static std::array<Char,FILENAME_MAX> currentPath;
  if (!_getWorkingDirectory(currentPath.data(), currentPath.size()))
  #endif
  {
    throw EXCEPTION(GenericException, S("Couldn't obtain the current working directory."));
  }

  #if defined(__MINGW32__) || defined(__MINGW64__)
  std::string path(utf8Encode(std::wstring(currentPath.data())));
  #else
  std::string path(currentPath.data());
  #endif
  if (path.back() != std::filesystem::path::preferred_separator) path += std::filesystem::path::preferred_separator;
  return path;
}


std::string getModuleDirectory()
{
  #if defined(__MINGW32__) || defined(__MINGW64__)
  thread_local static std::array<WChar,FILENAME_MAX> currentPath;
  int count = GetModuleFileNameW(NULL, currentPath.data(), currentPath.size());
  // TODO: Add count testing here.
  std::string path(utf8Encode(std::wstring(currentPath.data())));
  #else
  thread_local static std::array<Char,FILENAME_MAX> currentPath;
  ssize_t count = readlink("/proc/self/exe", currentPath.data(), currentPath.size());
  std::string path(currentPath.data(), (count > 0) ? count : 0);
  #endif

  Int pos = path.rfind(std::filesystem::path::preferred_separator);
  return std::string(path, 0, pos+1);
}

} // namespace
