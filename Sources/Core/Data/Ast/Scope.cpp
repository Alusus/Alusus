/**
 * @file Core/Data/Ast/Scope.cpp
 * Contains the implementation of class Core::Data::Ast::Scope.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Ast
{

//==============================================================================
// Inherited Functions

void Scope::onAdded(Int index)
{
  this->bridgesIndex.onAdded(index, ti_cast<Bridge>(this->getElement(index)) != 0);
  List::onAdded(index);
}

void Scope::onUpdated(Int index)
{
  this->bridgesIndex.onUpdated(index, ti_cast<Bridge>(this->getElement(index)) != 0);
  List::onUpdated(index);
}

void Scope::onRemoved(Int index)
{
  this->bridgesIndex.onRemoved(index);
  List::onRemoved(index);
}


//==============================================================================
// Bridge Retrieval Functions

Bridge* Scope::getBridge(Int index) const
{
  if (index < 0 || index >= this->bridgesIndex.getSize()) {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
  }
  return static_cast<Bridge*>(this->getElement(this->bridgesIndex.get(index)));
}

} // namespace
