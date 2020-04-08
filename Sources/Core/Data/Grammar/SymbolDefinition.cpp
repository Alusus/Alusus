/**
 * @file Core/Data/Grammar/SymbolDefinition.cpp
 * Contains the implementation of class Core::Data::Grammar::SymbolDefinition.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Destructor

SymbolDefinition::~SymbolDefinition()
{
  if (this->base != 0) this->detachFromBase();
  this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::DESTROY, 0);
  RESET_OWNED_SHAREDPTR(this->baseRef);
  RESET_OWNED_SHAREDPTR(this->flags);
  RESET_OWNED_SHAREDPTR(this->term);
  RESET_OWNED_SHAREDPTR(this->varDefs);
  RESET_OWNED_SHAREDPTR(this->vars);
  RESET_OWNED_SHAREDPTR(this->attributes);
  RESET_OWNED_SHAREDPTR(this->modifierTranslations);
}


//==============================================================================
// Member Functions

void SymbolDefinition::attachToBase(SymbolDefinition *p)
{
  ASSERT(this->base == 0);
  ASSERT(p != 0);
  this->base = p;
  this->base->changeNotifier.connect(this->parentContentChangeSlot);
  this->inheritFromParent();
}


void SymbolDefinition::detachFromBase()
{
  ASSERT(this->base != 0);
  this->removeInheritted();
  this->base->changeNotifier.disconnect(this->parentContentChangeSlot);
  this->base = 0;
}


void SymbolDefinition::inheritFromParent()
{
  ASSERT(this->base != 0);
  if ((this->ownership & SymbolDefinition::Element::TERM) == 0) {
    this->term = cloneInherited(this->base->getTerm());
    OWN_SHAREDPTR(this->term);
  }
  if ((this->ownership & SymbolDefinition::Element::VAR_DEFS) == 0) {
    this->varDefs = cloneInherited(this->base->getVarDefs());
    OWN_SHAREDPTR(this->varDefs);
  }
  if ((this->ownership & SymbolDefinition::Element::VARS) == 0) {
    this->vars = cloneInherited(this->base->getVars());
    OWN_SHAREDPTR(this->vars);
  }
  if ((this->ownership & SymbolDefinition::Element::HANDLER) == 0) {
    this->handler = this->base->getBuildHandler();
  }
  if ((this->ownership & SymbolDefinition::Element::FLAGS) == 0) {
    this->flags = cloneInherited(this->base->getFlags());
    OWN_SHAREDPTR(this->flags);
  }
  if ((this->ownership & SymbolDefinition::Element::ATTRIBUTES) == 0) {
    this->attributes = cloneInherited(this->base->getAttributes());
    OWN_SHAREDPTR(this->attributes);
  }
  if ((this->ownership & SymbolDefinition::Element::MODIFIER_TRANS) == 0) {
    this->modifierTranslations = cloneInherited(this->base->getModifierTranslations());
    OWN_SHAREDPTR(this->modifierTranslations);
  }
}


void SymbolDefinition::removeInheritted()
{
  if ((this->ownership & SymbolDefinition::Element::TERM) == 0) this->term.reset();
  if ((this->ownership & SymbolDefinition::Element::VAR_DEFS) == 0) this->varDefs.reset();
  if ((this->ownership & SymbolDefinition::Element::VARS) == 0) this->vars.reset();
  if ((this->ownership & SymbolDefinition::Element::HANDLER) == 0) this->handler.reset();
  if ((this->ownership & SymbolDefinition::Element::FLAGS) == 0) this->flags.reset();
  if ((this->ownership & SymbolDefinition::Element::ATTRIBUTES) == 0) this->attributes.reset();
  if ((this->ownership & SymbolDefinition::Element::MODIFIER_TRANS) == 0) this->modifierTranslations.reset();
}


void SymbolDefinition::onParentElementChanged(SymbolDefinition *obj, SymbolDefinition::ChangeOp op, Word elmt)
{
  if (op == SymbolDefinition::ChangeOp::DESTROY) {
    this->detachFromBase();
  } else if ((elmt & this->ownership) == 0) {
    if ((elmt & SymbolDefinition::Element::TERM) == 0) {
      this->term = cloneInherited(this->base->getTerm());
      OWN_SHAREDPTR(this->term);
    }
    if ((elmt & SymbolDefinition::Element::VAR_DEFS) == 0) {
      this->varDefs = cloneInherited(this->base->getVarDefs());
      OWN_SHAREDPTR(this->varDefs);
    }
    if ((elmt & SymbolDefinition::Element::VARS) == 0) {
      this->vars = cloneInherited(this->base->getVars());
      OWN_SHAREDPTR(this->vars);
    }
    if ((elmt & SymbolDefinition::Element::HANDLER) == 0) {
      this->handler = this->base->getBuildHandler();
    }
    if ((elmt & SymbolDefinition::Element::FLAGS) == 0) {
      this->flags = cloneInherited(this->base->getFlags());
      OWN_SHAREDPTR(this->flags);
    }
    if ((elmt & SymbolDefinition::Element::ATTRIBUTES) == 0) {
      this->attributes = cloneInherited(this->base->getAttributes());
      OWN_SHAREDPTR(this->attributes);
    }
    if ((elmt & SymbolDefinition::Element::MODIFIER_TRANS) == 0) {
      this->modifierTranslations = cloneInherited(this->base->getModifierTranslations());
      OWN_SHAREDPTR(this->modifierTranslations);
    }
  }
}


SbStr const& SymbolDefinition::getTranslatedModifierKeyword(Char const *keyword) const
{
  if (this->modifierTranslations == 0) return sbstr_cast(keyword);
  auto index = this->modifierTranslations->findIndex(keyword);
  if (index == -1) return sbstr_cast(keyword);
  else {
    auto str = this->modifierTranslations->get(index).ti_cast_get<TiStr>();
    if (str == 0) return sbstr_cast(keyword);
    else return sbstr_cast(str->get());
  }
}

} // namespace
