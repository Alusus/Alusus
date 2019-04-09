/**
 * @file Core/Basic/SbWStr.cpp
 * Contains the implementation of class Core::Basic::SbWStr.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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

void SbWStr::assign(WChar const *str, Word n, Word bufferSize)
{
  if (bufferSize < 2) {
    throw EXCEPTION(InvalidArgumentException, S("bufferSize"), S("Buffer size too small."), bufferSize);
  }
  if (n == 0) n = getStrLen(str);
  Word size = std::min(n, bufferSize-1);
  WChar *buf = reinterpret_cast<WChar*>(this);
  copyStr(str, buf, size);
  buf[size] = STRING_TERMINATOR;
}


void SbWStr::append(WChar const *str, Word strSize, Word bufferSize)
{
  if (strSize == 0) strSize = getStrLen(str);
  Word destSize = this->size();
  if (destSize >= bufferSize-1) return;
  if (strSize+destSize > bufferSize-1) strSize = bufferSize - 1 - destSize;
  WChar *buf = reinterpret_cast<WChar*>(this);
  copyStr(str, buf+destSize, strSize);
  buf[destSize+strSize] = STRING_TERMINATOR;
}


void SbWStr::assign(Char const *str, Word n, Word bufferSize)
{
  if (n == 0) n = getStrLen(str);
  WChar *buffer = reinterpret_cast<WChar*>(SALLOC(n*sizeof(WChar)));
  Int inLength, outLength;
  convertStr(str, n, buffer, n, inLength, outLength);
  this->assign(buffer, outLength, bufferSize);
  SFREE(buffer);
}


void SbWStr::append(Char const *str, Word srcSize, Word bufferSize)
{
  if (srcSize == 0) srcSize = getStrLen(str);
  WChar *buffer = reinterpret_cast<WChar*>(SALLOC(srcSize*sizeof(WChar)));
  Int inLength, outLength;
  convertStr(str, srcSize, buffer, srcSize, inLength, outLength);
  this->append(buffer, outLength, bufferSize);
  SFREE(buffer);
}

} } // namespace
