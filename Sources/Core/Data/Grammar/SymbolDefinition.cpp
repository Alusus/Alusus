/**
 * @file Core/Data/Grammar/SymbolDefinition.cpp
 * Contains the implementation of class Core::Data::Grammar::SymbolDefinition.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
  RESET_OWNED_SHAREDPTR(this->term);
  RESET_OWNED_SHAREDPTR(this->varDefs);
  RESET_OWNED_SHAREDPTR(this->vars);
  RESET_OWNED_SHAREDPTR(this->attributes);
  RESET_OWNED_SHAREDPTR(this->handler);
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
  if ((this->ownership & SymbolDefinition::Element::TERM) == 0) this->term = this->base->getTerm();
  if ((this->ownership & SymbolDefinition::Element::VAR_DEFS) == 0) this->varDefs = this->base->getVarDefs();
  if ((this->ownership & SymbolDefinition::Element::VARS) == 0) this->vars = this->base->getVars();
  if ((this->ownership & SymbolDefinition::Element::HANDLER) == 0) this->handler = this->base->getBuildHandler();
  if ((this->ownership & SymbolDefinition::Element::PRIORITY) == 0) this->priority = this->base->getPriority();
  if ((this->ownership & SymbolDefinition::Element::FLAGS) == 0) this->flags = this->base->getFlags();
  if ((this->ownership & SymbolDefinition::Element::ATTRIBUTES) == 0) this->attributes = this->base->getAttributes();
}


void SymbolDefinition::removeInheritted()
{
  if ((this->ownership & SymbolDefinition::Element::TERM) == 0) this->term.reset();
  if ((this->ownership & SymbolDefinition::Element::VAR_DEFS) == 0) this->varDefs.reset();
  if ((this->ownership & SymbolDefinition::Element::VARS) == 0) this->vars.reset();
  if ((this->ownership & SymbolDefinition::Element::HANDLER) == 0) this->handler.reset();
  if ((this->ownership & SymbolDefinition::Element::PRIORITY) == 0) this->priority.reset();
  if ((this->ownership & SymbolDefinition::Element::FLAGS) == 0) this->flags.reset();
  if ((this->ownership & SymbolDefinition::Element::ATTRIBUTES) == 0) this->attributes.reset();
}


void SymbolDefinition::onParentElementChanged(SymbolDefinition *obj, SymbolDefinition::ChangeOp op, Word elmt)
{
  if (op == SymbolDefinition::ChangeOp::DESTROY) {
    this->detachFromBase();
  } else if ((elmt & this->ownership) == 0) {
    if ((elmt & SymbolDefinition::Element::TERM) == 0) this->term = this->base->getTerm();
    if ((elmt & SymbolDefinition::Element::VAR_DEFS) == 0) this->varDefs = this->base->getVarDefs();
    if ((elmt & SymbolDefinition::Element::VARS) == 0) this->vars = this->base->getVars();
    if ((elmt & SymbolDefinition::Element::HANDLER) == 0) this->handler = this->base->getBuildHandler();
    if ((elmt & SymbolDefinition::Element::PRIORITY) == 0) this->priority = this->base->getPriority();
    if ((elmt & SymbolDefinition::Element::FLAGS) == 0) this->flags = this->base->getFlags();
    if ((elmt & SymbolDefinition::Element::ATTRIBUTES) == 0) this->attributes = this->base->getAttributes();
  }
}


//==============================================================================
// Initializable Implementation

void SymbolDefinition::initialize(TiObject *context)
{
  if (this->baseRef != 0) {
    auto grammarContext = ti_cast<Context>(context);
    if (grammarContext == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("context"), STR("Must be of type Core::Data::Grammar::Context."));
    }
    TiObject *p = grammarContext->traceValue(this->baseRef.get());
    if (p == 0) {
      throw EXCEPTION(GenericException, STR("Parent reference points to missing definition."));
    }
    SymbolDefinition *psd = tio_cast<SymbolDefinition>(p);
    if (psd == 0) {
      throw EXCEPTION(GenericException, STR("Parent reference points to an object of an invalid type."));
    }
    this->setBase(psd);
  }
}


//==============================================================================
// DataHaving Implementation

void SymbolDefinition::unsetIndexes(Int from, Int to)
{
  if (this->baseRef != 0) {
    Data::unsetIndexes(this->baseRef.get(), from, to);
  }
  if (this->term != 0 && (this->ownership & SymbolDefinition::Element::TERM) != 0) {
    Data::unsetIndexes(this->term.get(), from, to);
  }
  if (this->varDefs != 0 && (this->ownership & SymbolDefinition::Element::VAR_DEFS) != 0) {
    Data::unsetIndexes(this->varDefs.get(), from, to);
  }
  if (this->vars != 0 && (this->ownership & SymbolDefinition::Element::VARS) != 0) {
    Data::unsetIndexes(this->vars.get(), from, to);
  }
  if (this->handler != 0 && (this->ownership & SymbolDefinition::Element::HANDLER) != 0) {
    Data::unsetIndexes(this->handler.get(), from, to);
  }
  if (this->attributes != 0 && (this->ownership & SymbolDefinition::Element::ATTRIBUTES) != 0) {
    Data::unsetIndexes(this->attributes.get(), from, to);
  }
}

} // namespace
