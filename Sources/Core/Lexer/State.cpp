/**
 * @file Core/Lexer/State.cpp
 * Contains the implementation of Lexer::State.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Lexer
{

//==============================================================================
// Member Functions

/**
 * Copies the values of all the members of src, including all the values in the
 * stack, into this object.
 *
 * @param src A pointer to the source State object from which to copy.
 */
void State::copyFrom(const State *src)
{
  // Validation.
  if (src == 0) {
    throw InvalidArgumentException(STR("src"), STR("Core::Lexer::State::copyFrom"));
  }
  // Copy from the given state.
  this->tokenLength = src->getTokenLength();
  this->indexStack.resize(src->getIndexStackSize());
  for (Word i = 0; i < this->indexStack.size(); i++) {
    this->indexStack[i] = src->getIndexStackEntry(i);
  }
}

} } // namespace
