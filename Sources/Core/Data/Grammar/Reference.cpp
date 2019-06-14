/**
 * @file Core/Data/Grammar/Reference.cpp
 * Contains the implementation of class Core::Data::Reference.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

Bool Reference::setValue(TiObject *source, TiObject *value) const
{
  VALIDATE_NOT_NULL(source);

  if (this->next == 0) {
    MapContaining<TiObject> *container = source->getInterface<MapContaining<TiObject>>();
    if (container == 0) return false;

    if (this->cachedIndex < 0 || this->cachedIndex >= container->getElementCount()) {
      this->cachedIndex = container->findElementIndex(this->key.get());
    }

    if (this->cachedIndex == -1) {
      container->setElement(this->key.get(), value);
    } else {
      container->setElement(this->cachedIndex, value);
    }

    return true;
  } else {
    // Recurse into next level, if possible.
    if (!this->_getValue(source, source)) return false;
    return this->next->setValue(source, value);
  }
}


Bool Reference::removeValue(TiObject *source) const
{
  VALIDATE_NOT_NULL(source);

  if (this->next == 0) {
    DynamicMapContaining<TiObject> *container = source->getInterface<DynamicMapContaining<TiObject>>();
    if (container == 0) return false;

    if (this->cachedIndex < 0 || this->cachedIndex >= container->getElementCount()) {
      this->cachedIndex = container->findElementIndex(this->key.get());
      if (this->cachedIndex == -1) return false;
    }

    container->removeElement(this->cachedIndex);
    return true;
  } else {
    // Recurse into next level, if possible.
    if (!this->_getValue(source, source)) return false;
    return this->next->removeValue(source);
  }
}


Bool Reference::getValue(TiObject *source, TiObject *&value) const
{
  VALIDATE_NOT_NULL(source);

  TiObject *tempValue;
  if (!this->_getValue(source, tempValue)) return false;

  if (this->next == 0) {
    value = tempValue;
  } else {
    // Recurse into next level, if possible.
    if (tempValue == 0) return false;
    if (!this->next->getValue(tempValue, value)) return false;
  }

  return true;
}


Bool Reference::_getValue(TiObject *source, TiObject *&value) const
{
  VALIDATE_NOT_NULL(source);

  MapContaining<TiObject> *container = 0;

  if (this->cachedIndex == -1) {
    if (this->key == S("owner")) {
      this->cachedIndex = -2;
    } else {
      container = source->getInterface<MapContaining<TiObject>>();
      if (container == 0) return false;

       this->cachedIndex = container->findElementIndex(this->key.get());
      if (this->cachedIndex == -1) return false;
    }
  }

  if (this->cachedIndex == -2) {
    // Get the owner of the provided source.
    auto node = ti_cast<Node>(source);
    if (node == 0) return false;
    value = node->getOwner();
    return true;
  } else {
    // Get an element from the provided container.
    if (container == 0) container = source->getInterface<MapContaining<TiObject>>();

    value = container->getElement(this->cachedIndex);

    return true;
  }
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
