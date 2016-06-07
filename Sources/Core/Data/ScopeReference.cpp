/**
 * @file Core/Data/ScopeReference.cpp
 * Contains the implementation of class Core::Data::ScopeReference.
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

Bool ScopeReference::compare(Reference const *r) const
{
  const ScopeReference *sr = tio_cast<ScopeReference>(r);
  if (sr != 0 && sr->getScope() == this->getScope()) return Reference::compare(r);
  else return false;
}


void ScopeReference::setValue(Provider *provider, TiObject *parent,
                              ReferenceSetLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  NamedListContainer *container = parent->getInterface<NamedListContainer>();
  if (container == 0) return;

  if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
      this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    TiObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    if (isPerform(handler(0, obj))) {
      container->set(this->getListIndex(container, this->cachedIndex), obj);
    }
  } else {
    Int i = 0;
    Int index = 0;
    while (true) {
      index = this->findScope(container, index);
      if (index == -1) break;
      TiObject *obj = container->get(this->getListIndex(container, index));
      if (isPerform(handler(i, obj))) {
        container->set(this->getListIndex(container, index), obj);
      }
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) {
        this->cachedIndex = index;
        break;
      }
      ++index;
      ++i;
    }
  }
}


void ScopeReference::removeValue(Provider *provider, TiObject *parent,
                                 ReferenceRemoveLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  NamedListContainer *container = parent->getInterface<NamedListContainer>();
  if (container == 0) return;

  if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
      this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    TiObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    if (isPerform(handler(0, obj))) {
      container->remove(this->getListIndex(container, this->cachedIndex));
    }
  } else {
    Int i = 0;
    Int index = 0;
    while (true) {
      index = this->findScope(container, index);
      if (index == -1) break;
      TiObject *obj = container->get(this->getListIndex(container, index));
      if (isPerform(handler(i, obj))) {
        container->remove(this->getListIndex(container, index));
      }
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) {
        this->cachedIndex = index;
        break;
      }
      ++i;
    }
  }
}


void ScopeReference::forEachValue(Provider *provider, TiObject *parent,
                                  ReferenceForeachLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  NamedListContainer *container = parent->getInterface<NamedListContainer>();
  if (container == 0) return;

  if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
      this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    TiObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    handler(0, obj);
  } else {
    Int i = 0;
    Int index = 0;
    while (true) {
      index = this->findScope(container, index);
      if (index == -1) break;
      TiObject *obj = container->get(this->getListIndex(container, index));
      handler(i, obj);
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) {
        this->cachedIndex = index;
        break;
      }
      ++index;
      ++i;
    }
  }
}

} } // namespace
