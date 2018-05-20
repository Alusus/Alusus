/**
 * @file Core/Data/Ast/Bracket.cpp
 * Contains the implementation of class Core::Data::Ast::Bracket.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data { namespace Ast
{

//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Bracket::clone() const
{
  SharedPtr<Bracket> newBracket = std::make_shared<Bracket>();
  newBracket->setProdId(this->getProdId());
  newBracket->setSourceLocation(this->getSourceLocation());
  for (Word i = 0; i < this->getElementCount(); ++i) {
    newBracket->setElement(i, this->getElement(i));
  }
  newBracket->setType(this->type);
  return newBracket;
}

} } } // namespace
