/**
 * @file Core/Data/Grammar/Reference.cpp
 * Contains the implementation of class Core::Data::Reference.
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

Bool Reference::setValue(TiObject *parent, TiObject *value) const
{
  VALIDATE_NOT_NULL(parent);

  if (this->next == 0) {
    Basic::MapContaining<TiObject> *container = parent->getInterface<Basic::MapContaining<TiObject>>();
    if (container == 0) return false;

    if (this->cachedIndex < 0 || this->cachedIndex >= container->getElementCount()) {
      this->cachedIndex = container->findElementIndex(this->key.c_str());
    }

    if (this->cachedIndex == -1) {
      container->setElement(this->key.c_str(), value);
    } else {
      container->setElement(this->cachedIndex, value);
    }

    return true;
  } else {
    // Recurse into next level, if possible.
    if (!this->_getValue(parent, parent)) return false;
    return this->next->setValue(parent, value);
  }
}


Bool Reference::removeValue(TiObject *parent) const
{
  VALIDATE_NOT_NULL(parent);

  if (this->next == 0) {
    Basic::MapContaining<TiObject> *container = parent->getInterface<Basic::MapContaining<TiObject>>();
    if (container == 0) return false;

    if (this->cachedIndex < 0 || this->cachedIndex >= container->getElementCount()) {
      this->cachedIndex = container->findElementIndex(this->key.c_str());
      if (this->cachedIndex == -1) return false;
    }

    container->removeElement(this->cachedIndex);
    return true;
  } else {
    // Recurse into next level, if possible.
    if (!this->_getValue(parent, parent)) return false;
    return this->next->removeValue(parent);
  }
}


Bool Reference::getValue(TiObject *parent, TiObject *&value, Module **ownerModule) const
{
  VALIDATE_NOT_NULL(parent);

  Module *tempOwnerModule;
  if (!this->_getValue(parent, value, &tempOwnerModule)) return false;

  if (this->next == 0) {
    if (ownerModule != 0) *ownerModule = tempOwnerModule;
  } else {
    // Recurse into next level, if possible.
    if (value == 0) return false;
    if (!this->next->getValue(value, value, ownerModule)) return false;
    if (ownerModule != 0 && *ownerModule == 0) *ownerModule = tempOwnerModule;
  }

  return true;
}


Bool Reference::_getValue(TiObject *parent, TiObject *&value, Module **ownerModule) const
{
  VALIDATE_NOT_NULL(parent);

  Basic::MapContaining<TiObject> *container = parent->getInterface<Basic::MapContaining<TiObject>>();
  if (container == 0) return false;

  if (this->cachedIndex < 0 || this->cachedIndex >= container->getElementCount()) {
    this->cachedIndex = container->findElementIndex(this->key.c_str());
    if (this->cachedIndex == -1) return false;
  }

  value = container->getElement(this->cachedIndex);

  if (value != 0) {
    if (value->isA<SharedPairedPtr>()) {
      auto *pairedPtr = static_cast<SharedPairedPtr*>(value);
      value = pairedPtr->object.get();
      parent = pairedPtr->parent.get();
    } else if (value->isA<PlainPairedPtr>()) {
      auto *pairedPtr = static_cast<PlainPairedPtr*>(value);
      value = pairedPtr->object;
      parent = pairedPtr->parent;
    }
  }
  if (ownerModule != 0) *ownerModule = ti_cast<Module>(parent);

  return true;
}


Bool Reference::isEqual(Reference *ref)
{
  if (ref == 0) return false;
  if (this->key != ref->getKey()) return false;
  if (this->next == 0 && ref->getNext() == 0) return true;
  if (this->next == 0 || ref->getNext() == 0) return false;
  return this->next->isEqual(ref->getNext().get());
}

} // namespace
