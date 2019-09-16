/**
 * @file Spp/CodeGen/DestructionStack.cpp
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

void DestructionStack::popScope()
{
  if (this->scopeIndexes.size() == 0) {
    throw EXCEPTION(GenericException, S("Group stack is already empty."));
  }
  Int start = this->scopeIndexes.back();
  for (Int i = this->items.getCount() - 1; i >= start; --i) this->items.remove(i);
  this->scopeIndexes.pop_back();
}


Int DestructionStack::getScopeStartIndex(Int scope) const
{
  if (scope < 0) scope += this->scopeIndexes.size();
  if (scope < 0 || scope >= this->scopeIndexes.size()) {
    throw EXCEPTION(InvalidArgumentException, S("scope"), S("Out of range."), scope);
  }
  return this->scopeIndexes[scope];
}

} // namespace
