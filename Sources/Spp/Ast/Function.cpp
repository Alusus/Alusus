/**
 * @file Spp/Ast/Function.cpp
 * Contains the implementation of class Spp::Ast::Function.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Function::clone() const
{
  SharedPtr<Function> newFunction = std::make_shared<Function>();
  newFunction->setProdId(this->getProdId());
  newFunction->setSourceLocation(this->getSourceLocation());
  for (Word i = 0; i < this->getCount(); ++i) {
    newFunction->set(i, this->get(i));
  }
  newFunction->setName(this->name.get());
  newFunction->setVariadic(this->isVariadic());
  return newFunction;
}

} } // namespace
