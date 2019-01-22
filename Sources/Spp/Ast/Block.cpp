/**
 * @file Spp/Ast/Block.cpp
 * Contains the implementation of class Spp::Ast::Block.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Spp::Ast
{

//==============================================================================
// Inherited Functions

void Block::onAdded(Int index)
{
  this->useStatementsIndex.onAdded(index, ti_cast<UseStatement>(this->getElement(index)) != 0);
  Core::Data::Ast::Scope::onAdded(index);
}

void Block::onUpdated(Int index)
{
  this->useStatementsIndex.onUpdated(index, ti_cast<UseStatement>(this->getElement(index)) != 0);
  Core::Data::Ast::Scope::onUpdated(index);
}

void Block::onRemoved(Int index)
{
  this->useStatementsIndex.onRemoved(index);
  Core::Data::Ast::Scope::onRemoved(index);
}


//==============================================================================
// UseStatement Retrieval Functions

UseStatement* Block::getUseStatement(Int index) const
{
  if (index < 0 || index >= this->useStatementsIndex.getSize()) {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
  }
  return static_cast<UseStatement*>(this->getElement(this->useStatementsIndex.get(index)));
}

} // namespace
