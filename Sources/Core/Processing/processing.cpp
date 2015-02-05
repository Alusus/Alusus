/**
 * @file Core/Basic/processing.cpp
 * Contains the global implementations of Processing namespace's declarations.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

void computeNextCharPosition(WChar ch, Int &line, Int &column)
{
  if (ch == WCHR('\r')) {
    column = 1;
  } else if (ch == WCHR('\n')) {
    column = 1;
    line++;
  } else {
    column++;
  }
}

} } // namespace

