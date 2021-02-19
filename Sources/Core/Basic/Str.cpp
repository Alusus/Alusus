/**
 * @file Core/Basic/Str.cpp
 * Contains the implementation of class Core::Basic::Str.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Basic
{

void Str::assign(Char const *buf, LongInt pos, LongInt n)
{
  while (pos-- > 0 && *buf != 0) ++buf;
  if (pos > 0) {
    this->clear();
  } else {
    if (n == 0) n = getStrLen(buf);
    this->assign(buf, n);
  }
}


void Str::assign(WChar const *buf, LongInt pos, LongInt n)
{
  while (pos-- > 0 && *buf != 0) ++buf;
  if (pos > 0) {
    this->clear();
  } else {
    if (n == 0) n = getStrLen(buf);
    this->assign(buf, n);
  }
}

} // namespace
