/**
 * @file Spp/Ast/Scope.cpp
 * Contains the implementation of class Spp::Ast::Scope.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Inherited Functions

void Block::onAdded(Int index)
{
  this->injectionsIndex.onAdded(index, this->isInjection(index));
  Scope::onAdded(index);
}

void Block::onUpdated(Int index)
{
  this->injectionsIndex.onUpdated(index, this->isInjection(index));
  Scope::onUpdated(index);
}

void Block::onRemoved(Int index)
{
  this->injectionsIndex.onRemoved(index);
  Scope::onRemoved(index);
}


//==============================================================================
// Injection Retrieval Functions

Bool Block::isInjection(Int index) const
{
  auto def = ti_cast<Core::Data::Ast::Definition>(this->getElement(index));
  if (def == 0) return false;
  return Spp::Ast::isInjection(def);
}


TiObject* Block::getInjection(Int index) const
{
  if (index < 0 || index >= this->injectionsIndex.getSize()) {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
  }
  return this->getElement(this->injectionsIndex.get(index));
}

} // namespace
