/**
 * @file Core/Data/StrKeyReference.cpp
 * Contains the implementation of class Core::Data::StrKeyReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Member Functions

Bool StrKeyReference::compare(Reference const *r) const
{
  const StrKeyReference *sr = io_cast<const StrKeyReference>(r);
  if (sr != 0 && sr->getKey() == this->getKey()) return Reference::compare(r);
  else return false;
}


void StrKeyReference::setValue(Provider *provider, IdentifiableObject *parent,
                               ReferenceSetLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  MapContainer *container = parent->getInterface<MapContainer>();
  if (container == 0) return;

  Int index = -1;
  if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
    index = container->findIndex(this->key.c_str());
  } else {
    if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
      this->cachedIndex = container->findIndex(this->key.c_str());
    }
    index = this->cachedIndex;
  }

  IdentifiableObject *obj = 0;
  if (index == -1) {
    if (isPerform(handler(0, obj))) {
      container->set(this->key.c_str(), obj);
    }
  } else {
    obj = container->get(index);
    if (isPerform(handler(0, obj))) {
      container->set(index, obj);
    }
  }
}


void StrKeyReference::removeValue(Provider *provider, IdentifiableObject *parent,
                                  ReferenceRemoveLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  MapContainer *container = parent->getInterface<MapContainer>();
  if (container == 0) return;

  Int index;
  if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
    index = container->findIndex(this->key.c_str());
    if (index == -1) return;
  } else {
    if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
      this->cachedIndex = container->findIndex(this->key.c_str());
      if (this->cachedIndex == -1) return;
    }
    index = this->cachedIndex;
  }

  if (isPerform(handler(0, container->get(index)))) {
    container->remove(index);
  }
}


void StrKeyReference::forEachValue(Provider *provider, IdentifiableObject *parent,
                                   ReferenceForeachLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  MapContainer *container = parent->getInterface<MapContainer>();
  if (container == 0) return;

  Int index;
  if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
    index = container->findIndex(this->key.c_str());
    if (index == -1) return;
  } else {
    if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
      this->cachedIndex = container->findIndex(this->key.c_str());
      if (this->cachedIndex == -1) return;
    }
    index = this->cachedIndex;
  }

  handler(0, container->get(index));
}

} } // namespace
