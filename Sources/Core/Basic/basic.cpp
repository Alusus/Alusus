/**
 * @file Core/Basic/basic.cpp
 * Contains the global implementations of Basic namespace's declarations.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#include <locale>

#ifdef WINDOWS
  #include <direct.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #include <limits.h>
#endif

namespace Core { namespace Basic
{

#ifdef WINDOWS
  #define _getWorkingDirectory _getcwd
#else
  #define _getWorkingDirectory getcwd
#endif


//============================================================================
// Variables and Types

typedef std::codecvt<WChar,Char,std::mbstate_t> FacetType;

static std::locale utf8Locale("en_US.UTF8");

static const FacetType& utf8Facet = std::use_facet<FacetType>(utf8Locale);


//============================================================================
// Global Functions

void convertStr(Char const *input, int inputLength, WChar *output, int outputSize, int &processedInputLength, int &resultedOutputLength)
{
  std::mbstate_t mystate = std::mbstate_t();
  const char* fromNext;
  wchar_t* toNext;

  // translate characters:
  utf8Facet.in(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
}


void convertStr(WChar const *input, int inputLength, Char *output, int outputSize, int &processedInputLength, int &resultedOutputLength)
{
  std::mbstate_t mystate = std::mbstate_t();
  const wchar_t *fromNext;
  char *toNext;

  // translate characters:
  utf8Facet.out(mystate, input, input+inputLength, fromNext, output, output+outputSize, toNext);

  processedInputLength = fromNext-input;
  resultedOutputLength = toNext-output;
}


void* allocateOnStack(Word size)
{
  #ifdef WINDOWS
    return _malloca(size);
  #else
    return alloca(size);
  #endif
}


void freeFromStack(void *p)
{
  #ifdef WINDOWS
    return _freea(p);
  #endif
}


std::string getWorkingDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  if (!_getWorkingDirectory(currentPath.data(), currentPath.size()))
  {
    throw GeneralException(STR("Couldn't obtain the current working directory."),
                           STR("Core::Basic::getWorkingDirectory"));
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
