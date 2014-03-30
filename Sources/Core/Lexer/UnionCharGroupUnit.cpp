/**
 * @file Core/Lexer/UnionCharGroupUnit.cpp
 * Contains the implementation of class Lexer::UnionCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdarg.h>

namespace Core { namespace Lexer
{

//==============================================================================
// Constructors

/**
 * Initializes the character group units list from the provided list. This
 * constructor expects the variable list of arguments to be pointers to
 * CharGroupUnit.
 *
 * @param count The count of provided character group unit pointers.
 */
UnionCharGroupUnit::UnionCharGroupUnit(Int count, ...)
{
  va_list units;
  va_start(units, count);
  if (count <= 0) {
    throw InvalidArgumentException(STR("count"),
                                   STR("Core::Lexer::UnionCharGroupUnit::UnionCharGroupUnit"),
                                   STR("Char group unit count should be > 0."));
  }
  this->charGroupUnits.resize(count);
  for (Int i = 0; i < count; i++) {
    this->charGroupUnits.at(i) = SharedPtr<CharGroupUnit>(va_arg(units, CharGroupUnit*));
  }
  va_end(units);
}

} } // namespace
