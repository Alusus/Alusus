/**
 * @file Core/Data/Grammar/GrammarModule.cpp
 * Contains the implementation of class Core::Data::Grammar::GrammarModule.
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

GrammarModule::~GrammarModule()
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

void GrammarModule::attachToBase(GrammarModule *b)
{
  ASSERT(this->base == 0);
  NbMap::setBase(b);

  this->base = b;
  this->base->destroyNotifier.connect(this->baseDestroySlot);
  this->base->metaChangeNotifier.connect(this->baseMetaChangeSlot);

  if ((this->ownership & GrammarModule::MetaElement::START_REF) == 0) {
    this->startRef = this->base->getStartRef();
  }
  if ((this->ownership & GrammarModule::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = this->base->getLexerModuleRef();
  }
  if ((this->ownership & GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = this->base->getErrorSyncBlockPairsRef();
  }
}


void GrammarModule::detachFromBase()
{
  ASSERT(this->base != 0);

  this->base->metaChangeNotifier.disconnect(this->baseMetaChangeSlot);
  this->base->destroyNotifier.disconnect(this->baseDestroySlot);

  if ((this->ownership & GrammarModule::MetaElement::START_REF) == 0) {
    this->startRef.reset();
  }
  if ((this->ownership & GrammarModule::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef.reset();
  }
  if ((this->ownership & GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef.reset();
  }

  this->base = 0;
  NbMap::setBase(0);
}


void GrammarModule::onBaseMetaChanged(GrammarModule *obj, Word element)
{
  if ((element & GrammarModule::MetaElement::START_REF) == 0) {
    this->startRef = obj->getStartRef();
  }
  if ((element & GrammarModule::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = obj->getLexerModuleRef();
  }
  if ((element & GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = obj->getErrorSyncBlockPairsRef();
  }
}


//==============================================================================
// Initializable Implementation

void GrammarModule::initialize(TiObject *context)
{
  if (this->baseRef != 0) {
    auto grammarContext = ti_cast<Grammar::Context>(context);
    if (grammarContext == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("context"), STR("Must be of type Core::Data::Grammar::Context."));
    }
    TiObject *p = grammarContext->traceValue(this->baseRef.get());
    if (p == 0) {
      throw EXCEPTION(GenericException, STR("Base reference points to missing definition."));
    }
    GrammarModule *pm = tio_cast<GrammarModule>(p);
    if (pm == 0) {
      throw EXCEPTION(GenericException, STR("Base reference points to an object of an invalid type."));
    }
    this->setBase(pm);
  }
}


//==============================================================================
// DataOwner Implementation

void GrammarModule::unsetIndexes(Int from, Int to)
{
  if (this->baseRef != 0) {
    Data::unsetIndexes(this->baseRef.get(), from, to);
  }
  if (this->startRef != 0 && (this->ownership & GrammarModule::MetaElement::START_REF) != 0) {
    Data::unsetIndexes(this->startRef.get(), from, to);
  }
  if (this->lexerModuleRef != 0 && (this->ownership & GrammarModule::MetaElement::LEXER_MODULE_REF) != 0) {
    Data::unsetIndexes(this->lexerModuleRef.get(), from, to);
  }
  if (
    this->errorSyncBlockPairsRef != 0 && (this->ownership & GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) != 0
  ) {
    Data::unsetIndexes(this->errorSyncBlockPairsRef.get(), from, to);
  }
  NbMap::unsetIndexes(from, to);
}

} // namespace
