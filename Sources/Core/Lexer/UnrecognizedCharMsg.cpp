/**
 * @file Core/Lexer/UnrecognizedCharMsg.cpp
 * Contains the implementation of class Lexer::UnrecognizedCharMsg.
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

void UnrecognizedCharMsg::buildDescription(Str &str) const
{
  str = STR("Lexer Error [Unrecognized Character(s)]: ");
  str += this->getText();
}

} } // namespace
