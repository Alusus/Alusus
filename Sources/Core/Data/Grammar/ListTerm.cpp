/**
 * @file Core/Data/Grammar/ListTerm.cpp
 * Contains the implementation of class Core::Data::Grammar::ListTerm.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Member Functions

/**
 * @param index The index of the term to return. If the list contains a
 *              singular term, that term will be returned regardless of this
 *              parameter.
 */
Term* ListTerm::getTerm(Int index) const
{
  if (this->terms != 0) {
    if (static_cast<Word>(index) >= this->terms->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    Term *term = this->terms->get(index).s_cast_get<Term>();
    return term;
  } else {
    return 0;
  }
}

} // namespace
