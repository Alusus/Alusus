/**
 * @file Spp/Ast/Scope.cpp
 * Contains the implementation of class Spp::Ast::Scope.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

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
  auto modifiers = def->getModifiers().get();
  if (modifiers != 0) {
    for (Int i = 0; i < modifiers->getElementCount(); ++i) {
      auto identifier = ti_cast<Core::Data::Ast::Identifier>(modifiers->getElement(i));
      if (identifier != 0 && identifier->getValue() == S("injection")) return true;
    }
  }
  return false;
}


TiObject* Block::getInjection(Int index) const
{
  if (index < 0 || index >= this->injectionsIndex.getSize()) {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
  }
  return this->getElement(this->injectionsIndex.get(index));
}

} // namespace
