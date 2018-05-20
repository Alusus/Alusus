/**
 * @file Core/Data/Ast/InfixOperator.cpp
 * Contains the implementation of class Core::Data::Ast::InfixOperator.
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

SharedPtr<TiObject> InfixOperator::clone() const
{
  SharedPtr<InfixOperator> newInfixOperator = std::make_shared<InfixOperator>();
  newInfixOperator->setProdId(this->getProdId());
  for (Word i = 0; i < this->getElementCount(); ++i) {
    newInfixOperator->setElement(i, this->getElement(i));
  }
  newInfixOperator->setType(this->type.get());
  newInfixOperator->setSourceLocation(this->getSourceLocation());
  return newInfixOperator;
}

} } } // namespace
