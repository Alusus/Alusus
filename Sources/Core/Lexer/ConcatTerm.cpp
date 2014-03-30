/**
 * @file Core/Lexer/ConcatTerm.cpp
 * Contains the implementation of class Lexer::ConcatTerm.
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

//============================================================================
// Constructors

/**
 * Attach the provided list of terms to be used as the concatenated terms. This
 * constructors expects the variable list of arguments to be pointers to Term.
 *
 * @param count The count of terms provided to the function.
 */
ConcatTerm::ConcatTerm(Int count, ...)
{
  va_list terms;
  va_start(terms, count);
  if (count <= 1) {
    throw InvalidArgumentException(STR("count"),
                                   STR("Core::Lexer::ConcatTerm::ConcatTerm"),
                                   STR("Terms count should be > 1."));
  }
  for (Int i = 0; i < count; ++i) {
    this->attachChildTerm(SharedPtr<Term>(va_arg(terms, Term*)));
  }
  va_end(terms);
}

} } // namespace

