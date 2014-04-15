/**
 * @file Core/Basic/SbStr.cpp
 * Contains the implementation of class Core::Basic::SbStr.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

void SbStr::assign(const Char *str, Word n, Word bufferSize)
{
  if (bufferSize < 2) {
    throw InvalidArgumentException(STR("bufferSize"), STR("Core::Basic::SbStr::assign"),
                                   STR("Buffer size too small."));
  }
  Word size = std::min(n, bufferSize-1);
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf, size);
  buf[size] = STRING_TERMINATOR;
}


void SbStr::assign(const Char *str, Word bufferSize)
{
  if (bufferSize < 2) {
    throw InvalidArgumentException(STR("bufferSize"), STR("Core::Basic::SbStr::assign"),
                                   STR("Buffer size too small."));
  }
  Word size = std::min(getStrLen(str), bufferSize-1);
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf, size);
  buf[size] = STRING_TERMINATOR;
}


void SbStr::append(const Char *str, Word src_size, Word bufferSize)
{
  Word dest_size = this->size();
  if (dest_size >= bufferSize-1) return;
  if (src_size+dest_size > bufferSize-1) src_size = bufferSize - 1 - dest_size;
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf+dest_size, src_size);
  buf[dest_size+src_size] = STRING_TERMINATOR;
}


void SbStr::append(const Char *str, Word bufferSize)
{
  Word dest_size = this->size();
  if (dest_size >= bufferSize-1) return;
  Word src_size = getStrLen(str);
  if (src_size+dest_size > bufferSize-1) src_size = bufferSize - 1 - dest_size;
  Char *buf = reinterpret_cast<Char*>(this);
  copyStr(str, buf+dest_size, src_size);
  buf[dest_size+src_size] = STRING_TERMINATOR;
}

} } // namespace
