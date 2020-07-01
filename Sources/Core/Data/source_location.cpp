/**
 * @file Core/Data/source_location.cpp
 * Contains the implementation of source location classes.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
  if (sl->isA<SourceLocationRecord>()) {
    auto sharedSl = getSharedPtr(static_cast<SourceLocationRecord*>(sl));
    if (sharedSl == 0) {
      sharedSl = std::make_shared<SourceLocationRecord>(*static_cast<SourceLocationRecord*>(sl));
    }
    this->add(sharedSl);
  } else {
    auto stack = static_cast<SourceLocationStack*>(sl);
    this->reserve(this->getCount() + stack->getCount());
    for (Int i = 0; i < stack->getCount(); ++i) {
      this->add(stack->get(i));
    }
  }
}


void SourceLocationStack::pop(Word count)
{
  Word c = std::min(count, (Word)this->getCount());
  for (Word i = 0; i < c; ++i) this->remove(this->getCount() - 1);
}

} } // namespace
