/**
 * @file Core/Basic/SbStr.cpp
 * Contains the implementation of class Core::Basic::SbStr.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

void SbStr::assign(Char const *str, Word n, Word bufferSize)
{
  if (bufferSize < 2) {
    throw EXCEPTION(InvalidArgumentException, STR("bufferSize"), STR("Buffer size too small."), bufferSize);
  }
  if (n == 0) n = getStrLen(str);
  Word size = std::min(n, bufferSize-1);
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf, size);
  buf[size] = STRING_TERMINATOR;
}


void SbStr::append(Char const *str, Word strSize, Word bufferSize)
{
  if (strSize == 0) strSize = getStrLen(str);
  Word destSize = this->size();
  if (destSize >= bufferSize-1) return;
  if (strSize+destSize > bufferSize-1) strSize = bufferSize - 1 - destSize;
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf+destSize, strSize);
  buf[destSize+strSize] = STRING_TERMINATOR;
}


void SbStr::assign(WChar const *str, Word n, Word bufferSize)
{
  if (n == 0) n = getStrLen(str);
  Char *buffer = reinterpret_cast<Char*>(SALLOC(n*4)); // A UTF8 character may take up to 4 bytes.
  Int inLength, outLength;
  convertStr(str, n, buffer, n*4, inLength, outLength);
  this->assign(buffer, outLength, bufferSize);
  SFREE(buffer);
}


void SbStr::append(WChar const *str, Word srcSize, Word bufferSize)
{
  if (srcSize == 0) srcSize = getStrLen(str);
  Char *buffer = reinterpret_cast<Char*>(SALLOC(srcSize*4)); // A UTF8 character may take up to 4 bytes.
  Int inLength, outLength;
  convertStr(str, srcSize, buffer, srcSize*4, inLength, outLength);
  this->append(buffer, outLength, bufferSize);
  SFREE(buffer);
}

} } // namespace
