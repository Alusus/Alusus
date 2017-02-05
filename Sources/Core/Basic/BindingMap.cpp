/**
 * @file Core/Basic/BindingMap.cpp
 * Contains the implementation of class Core::Basic::BindingMap.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

void BindingMap::setBase(BindingMap *base)
{
  if (this->getBase() != 0) {
    // Uninherit old.
    this->onBaseDetached();
    BindingMapBase::setBase(0);
    this->baseChangeSlot.disconnect();
    this->baseDestroySlot.disconnect();
  }
  if (base != 0) {
    // Inheirt new.
    base->destroyNotifier.connect(this->baseDestroySlot);
    BindingMapBase::setBase(base);
    base->changeNotifier.connect(this->baseChangeSlot);
  }
}


void BindingMap::onBaseContentChanged(BindingMapBase *src, ContentChangeOp op, Int index)
{
  if (this->isInherited(index)) return;
  if (op == ContentChangeOp::WILL_UPDATE || op == ContentChangeOp::WILL_REMOVE) {
    auto superFunc = src->get(index).ti_cast_get<TiFunctionBase>();
    auto func = this->get(index).ti_cast<TiFunctionBase>();
    if (superFunc != 0 && func != 0) {
      this->resetFunctionChain(this->getKey(index).c_str(), superFunc);
    }
  } else if (op == ContentChangeOp::ADDED || op == ContentChangeOp::UPDATED) {
    auto superFunc = src->get(index).ti_cast<TiFunctionBase>();
    auto func = this->get(index).ti_cast<TiFunctionBase>();
    if (superFunc != 0 && func != 0) {
      this->updateFunctionChain(this->getKey(index).c_str(), 0, superFunc);
    }
  }
}


void BindingMap::onBaseDetached()
{
  auto src = this->getBase();
  for (Int i = 0; i < this->getCount(); ++i) {
    if (this->isInherited(i)) continue;
    auto superFunc = src->get(i).ti_cast_get<TiFunctionBase>();
    auto func = this->get(i).ti_cast<TiFunctionBase>();
    if (superFunc != 0 && func != 0) {
      this->resetFunctionChain(this->getKey(i).c_str(), superFunc);
    }
  }
}


void BindingMap::updateFunctionChain(Char const *name, TiFunctionBase *currentTifn,
                                     SharedPtr<TiFunctionBase> const &newTifn)
{
  VALIDATE_NOT_NULL(name, newTifn);
  auto index = this->findIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Not found."), name);
  }
  auto tifn = this->get(index).ti_cast_get<TiFunctionBase>();
  if (tifn == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("No function override is defined at this key."), name);
  }
  if (tifn == currentTifn) {
    this->set(index, newTifn);
  } else {
    while (tifn != 0 && tifn->getSuper() != currentTifn) tifn = tifn->getSuper();
    if (tifn == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("name"),
                      STR("Provided funciton override is not found at the given key."), name);
    }
    tifn->setSuper(newTifn.get());
  }
}


void BindingMap::resetFunction(Char const *name, TiFunctionBase *currentTifn)
{
  VALIDATE_NOT_NULL(name, currentTifn);
  auto index = this->findIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Not found."), name);
  }
  auto tifn = this->get(index).ti_cast_get<TiFunctionBase>();
  if (tifn == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("No function override is defined at this key."), name);
  }
  if (tifn == currentTifn) {
    if (currentTifn->getSuper() == 0) {
      this->remove(index);
    } else {
      this->set(index, getSharedPtr(currentTifn->getSuper(), true));
    }
  } else {
    while (tifn != 0 && tifn->getSuper() != currentTifn) tifn = tifn->getSuper();
    if (tifn == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("name"),
                      STR("Provided funciton override is not found at the given key."), name);
    }
    tifn->setSuper(currentTifn->getSuper());
  }
}


void BindingMap::resetFunctionChain(Char const *name, TiFunctionBase *currentTifn)
{
  VALIDATE_NOT_NULL(name, currentTifn);
  auto index = this->findIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Not found."), name);
  }
  auto tifn = this->get(index).ti_cast_get<TiFunctionBase>();
  if (tifn == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("No function override is defined at this key."), name);
  }
  if (tifn == currentTifn) {
    this->remove(index);
  } else {
    while (tifn != 0 && tifn->getSuper() != currentTifn) tifn = tifn->getSuper();
    if (tifn == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("name"),
                      STR("Provided funciton override is not found at the given key."), name);
    }
    tifn->setSuper(0);
  }
}

} } // namespace
