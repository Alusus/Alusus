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
  if (this->getBaseModule() != 0) this->detachFromBase();
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
  ASSERT(this->getBaseModule() == 0 || this->getBaseModule() == b);
  SharedMapBase<TiObject, Node>::setBase(b);

  this->getBaseModule()->destroyNotifier.connect(this->baseDestroySlot);
  this->getBaseModule()->metaChangeNotifier.connect(this->baseMetaChangeSlot);

  if ((this->ownership & Module::MetaElement::START_REF) == 0) {
    this->startRef = this->getBaseModule()->getStartRef();
  }
  if ((this->ownership & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = this->getBaseModule()->getLexerModuleRef();
  }
  if ((this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = this->getBaseModule()->getErrorSyncBlockPairsRef();
  }
}


void Module::detachFromBase()
{
  this->baseMetaChangeSlot.disconnect();
  this->baseDestroySlot.disconnect();

  if ((this->ownership & Module::MetaElement::START_REF) == 0) {
    this->startRef.reset();
  }
  if ((this->ownership & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef.reset();
  }
  if ((this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef.reset();
  }

  SharedMapBase<TiObject, Node>::setBase(0);
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

  for (Word i = 0; i < this->getCount(); ++i) {
    TiObject *obj = this->get(i).get();
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}

} // namespace
