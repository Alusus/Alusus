/**
 * @file Core/Data/source_location.cpp
 * Contains the implementation of source location classes.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// SourceLocationStack Functions

void SourceLocationStack::push(SourceLocation *sl)
{
  if (sl == 0) {
    throw EXCEPTION(InvalidArgumentException, S("sl"), S("Cannot be null."));
  }
  auto sharedSl = getSharedPtr(sl);
  this->add(sharedSl);
}


void SourceLocationStack::pop()
{
  if (this->getCount() == 0) return;
  this->remove(this->getCount() - 1);
}

} } // namespace
