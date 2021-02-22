/**
 * @file Core/Processing/LexerState.cpp
 * Contains the implementation of Processing::LexerState.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing
{

//==============================================================================
// Member Functions

/**
 * Copies the values of all the members of src, including all the values in the
 * stack, into this object.
 *
 * @param src A pointer to the source LexerState object from which to copy.
 */
void LexerState::copyFrom(const LexerState *src)
{
  // Validation.
  if (src == 0) {
    throw EXCEPTION(InvalidArgumentException, S("src"), S("Cannot be null."));
  }
  // Copy from the given state.
  this->tokenLength = src->getTokenLength();
  this->tokenDefIndex = src->getTokenDefIndex();
  this->levelCount = src->getLevelCount();
  memcpy(this->levels, src->levels, this->levelCount * sizeof(LexerState::Level));
}

} // namespace
