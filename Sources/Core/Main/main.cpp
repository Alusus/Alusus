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
#ifdef _WIN32
#include "Win32Helpers.h"
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

namespace Core::Main
{

using namespace Data;

#ifdef _WIN32
#define _wgetWorkingDirectory _wgetcwd
#else
#define _getWorkingDirectory getcwd
#endif


//==============================================================================
// Global Functions

std::string getWorkingDirectory()
{
  #ifdef _WIN32
  thread_local static std::array<WChar,FILENAME_MAX> currentPath;
  if (!_wgetWorkingDirectory(currentPath.data(), currentPath.size()))
  #else
  thread_local static std::array<Char,FILENAME_MAX> currentPath;
  if (!_getWorkingDirectory(currentPath.data(), currentPath.size()))
  #endif
  {
    throw EXCEPTION(GenericException, S("Couldn't obtain the current working directory."));
  }

  #ifdef _WIN32
  std::string path(utf8Encode(std::wstring(currentPath.data())));
  #else
  std::string path(currentPath.data());
  #endif
  if (path.back() != fs::path::preferred_separator) path += fs::path::preferred_separator;
  return fs::u8path(path).string();
}


std::string getModuleDirectory()
{
#ifdef _WIN32
  thread_local static std::array<WChar,FILENAME_MAX> currentPath;
  int count = GetModuleFileNameW(NULL, currentPath.data(), currentPath.size());
  // TODO: Add count testing here.
  std::string path(utf8Encode(std::wstring(currentPath.data())));
#elif __APPLE__
  thread_local static std::array<Char,FILENAME_MAX> currentPath;
  uint32_t size = FILENAME_MAX;
  // TODO: Check the result.
  auto res = _NSGetExecutablePath(currentPath.data(), &size);
  std::string path(currentPath.data());
#else
  thread_local static std::array<Char,FILENAME_MAX> currentPath;
  ssize_t count = readlink("/proc/self/exe", currentPath.data(), currentPath.size());
  std::string path(currentPath.data(), (count > 0) ? count : 0);
  #endif

  Int pos = path.rfind(fs::path::preferred_separator);
  return fs::u8path(std::string(path, 0, pos+1)).string();
}

} // namespace
