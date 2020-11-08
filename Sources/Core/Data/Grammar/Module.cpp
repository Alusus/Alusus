/**
 * @file Core/Data/Grammar/Module.cpp
 * Contains the implementation of class Core::Data::Grammar::Module.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Destructor

Module::~Module()
{
  this->destruct();
  RESET_OWNED_SHAREDPTR(this->baseRef);
  RESET_OWNED_SHAREDPTR(this->startRef);
  RESET_OWNED_SHAREDPTR(this->lexerModuleRef);
  RESET_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef);
}


//==============================================================================
// Abstract Function Implementations

SharedPtr<TiObject> Module::prepareForSet(
  Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
) {
  if (inherited) {
    return cloneInherited(obj.get());
  } else {
    return obj;
  }
}


void Module::finalizeSet(
  Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
) {
  if (obj != 0 && obj->isDerivedFrom<Node>()) {
    obj.s_cast_get<Node>()->setOwner(this);
    setTreeIds(obj.get());
  }
}


void Module::prepareForUnset(
  Char const *key, Int index, SharedPtr<TiObject> const &obj, Bool inherited
) {
  if (obj != 0 && obj->isDerivedFrom<Node>() && obj.s_cast_get<Node>()->getOwner() == this) {
    obj.s_cast_get<Node>()->setOwner(0);
  }
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
    this->startRef = cloneInherited(this->getBaseModule()->getStartRef().get());
    OWN_SHAREDPTR(this->startRef);
  }
  if ((this->ownership & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = cloneInherited(this->getBaseModule()->getLexerModuleRef().get());
    OWN_SHAREDPTR(this->lexerModuleRef);
  }
  if ((this->ownership & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = cloneInherited(this->getBaseModule()->getErrorSyncBlockPairsRef().get());
    OWN_SHAREDPTR(this->errorSyncBlockPairsRef);
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
    this->startRef = cloneInherited(obj->getStartRef().get());
    OWN_SHAREDPTR(this->startRef);
  }
  if ((element & Module::MetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = cloneInherited(obj->getLexerModuleRef().get());
    OWN_SHAREDPTR(this->lexerModuleRef);
  }
  if ((element & Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF) == 0) {
    this->errorSyncBlockPairsRef = cloneInherited(obj->getErrorSyncBlockPairsRef().get());
    OWN_SHAREDPTR(this->errorSyncBlockPairsRef);
  }
}

} // namespace
