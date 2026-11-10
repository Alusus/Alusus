/**
 * @file Core/Ast/Scope.cpp
 * Contains the implementation of class Core::Ast::Scope.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Ast
{

//==============================================================================
// Inherited Functions

void Scope::onAdded(Int index)
{
  this->bridgesIndex.onAdded(index, ti_cast<Bridge>(this->getElement(index)) != 0);
  this->addToDefinitionsIndex(index);
  List::onAdded(index);
}

void Scope::onWillUpdate(Int index)
{
  auto def = ti_cast<Definition>(this->getElement(index));
  if (def != 0) this->removeFromDefinitionsIndex(def, def->getName().getStr());
  List::onWillUpdate(index);
}

void Scope::onUpdated(Int index)
{
  this->bridgesIndex.onUpdated(index, ti_cast<Bridge>(this->getElement(index)) != 0);
  this->addToDefinitionsIndex(index);
  List::onUpdated(index);
}

void Scope::onWillRemove(Int index)
{
  auto def = ti_cast<Definition>(this->getElement(index));
  if (def != 0) this->removeFromDefinitionsIndex(def, def->getName().getStr());
  List::onWillRemove(index);
}

void Scope::onRemoved(Int index)
{
  this->bridgesIndex.onRemoved(index);
  List::onRemoved(index);
}


//==============================================================================
// Definition Retrieval Functions

Int Scope::findDefinitionIndex(Definition const *def) const
{
  for (Int i = 0; i < this->getCount(); ++i) {
    if (this->getElement(i) == def) return i;
  }
  return -1;
}


void Scope::onDefinitionRenamed(Definition *def, Str const &oldName)
{
  this->removeFromDefinitionsIndex(def, oldName);
  auto index = this->findDefinitionIndex(def);
  if (index != -1) this->addToDefinitionsIndex(index);
}


void Scope::addToDefinitionsIndex(Int index)
{
  auto def = ti_cast<Definition>(this->getElement(index));
  if (def == 0) return;
  auto &defs = this->definitionsIndex[def->getName().getStr()];
  if (index == static_cast<Int>(this->getCount()) - 1) {
    // Appending is the common case, in which the definition comes after all other definitions of the same name.
    defs.push_back(def);
  } else {
    // Count the definitions of the same name that precede this one to keep the list in scope order.
    Word pos = 0;
    for (Int i = 0; i < index && pos < defs.size(); ++i) {
      if (this->getElement(i) == defs[pos]) ++pos;
    }
    defs.insert(defs.begin() + pos, def);
  }
}


void Scope::removeFromDefinitionsIndex(Definition *def, Str const &name)
{
  auto iter = this->definitionsIndex.find(name);
  if (iter == this->definitionsIndex.end()) return;
  auto &defs = iter->second;
  auto pos = std::find(defs.begin(), defs.end(), def);
  if (pos != defs.end()) defs.erase(pos);
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
