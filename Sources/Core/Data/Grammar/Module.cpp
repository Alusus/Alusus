/**
 * @file Core/Data/Grammar/Module.cpp
 * Contains the implementation of class Core::Data::Grammar::Module.
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

Module::~Module()
{
  if (this->base != 0) this->detachFromBase();
  this->destroyNotifier.emit(this);
  RESET_OWNED_SHAREDPTR(this->baseRef);
  RESET_OWNED_SHAREDPTR(this->startRef);
  RESET_OWNED_SHAREDPTR(this->lexerModuleRef);
  RESET_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef);
}


//==============================================================================
// Inheritance Functions

void Module::attachToBase(Module *b)
{
  ASSERT(this->base == 0);
  NbMap::setBase(b);

  this->base = b;
  this->base->destroyNotifier.connect(this->baseDestroySlot);
  this->base->metaChangeNotifier.connect(this->baseMetaChangeSlot);

  if ((this->ownership & Module::MetaElement::START_REF) == 0) {
    this->startRef = this->base->getStartRef();
  }
  if ((this->ownership & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = this->base->getLexerModuleRef();
  }
  if ((this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = this->base->getErrorSyncBlockPairsRef();
  }
}


void Module::detachFromBase()
{
  ASSERT(this->base != 0);

  this->base->metaChangeNotifier.disconnect(this->baseMetaChangeSlot);
  this->base->destroyNotifier.disconnect(this->baseDestroySlot);

  if ((this->ownership & Module::MetaElement::START_REF) == 0) {
    this->startRef.reset();
  }
  if ((this->ownership & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef.reset();
  }
  if ((this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef.reset();
  }

  this->base = 0;
  NbMap::setBase(0);
}


void Module::onBaseMetaChanged(Module *obj, Word element)
{
  if ((element & Module::MetaElement::START_REF) == 0) {
    this->startRef = obj->getStartRef();
  }
  if ((element & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = obj->getLexerModuleRef();
  }
  if ((element & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = obj->getErrorSyncBlockPairsRef();
  }
}


//==============================================================================
// Initializable Implementation

void Module::initialize(TiObject *context)
{
  if (this->baseRef != 0) {
    auto grammarContext = ti_cast<Context>(context);
    if (grammarContext == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("context"), STR("Must be of type Core::Data::Grammar::Context."));
    }
    TiObject *p = grammarContext->traceValue(this->baseRef.get());
    if (p == 0) {
      throw EXCEPTION(GenericException, STR("Base reference points to missing definition."));
    }
    Module *pm = tio_cast<Module>(p);
    if (pm == 0) {
      throw EXCEPTION(GenericException, STR("Base reference points to an object of an invalid type."));
    }
    this->setBase(pm);
  }
}


//==============================================================================
// DataHaving Implementation

void Module::unsetIndexes(Int from, Int to)
{
  if (this->baseRef != 0) {
    Data::unsetIndexes(this->baseRef.get(), from, to);
  }
  if (this->startRef != 0 && (this->ownership & Module::MetaElement::START_REF) != 0) {
    Data::unsetIndexes(this->startRef.get(), from, to);
  }
  if (this->lexerModuleRef != 0 && (this->ownership & Module::MetaElement::LEXER_MODULE_REF) != 0) {
    Data::unsetIndexes(this->lexerModuleRef.get(), from, to);
  }
  if (
    this->errorSyncBlockPairsRef != 0 && (this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) != 0
  ) {
    Data::unsetIndexes(this->errorSyncBlockPairsRef.get(), from, to);
  }
  NbMap::unsetIndexes(from, to);
}

} // namespace
