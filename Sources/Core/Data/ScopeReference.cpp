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
  const ScopeReference *sr = io_cast<ScopeReference>(r);
  if (sr != 0 && sr->getScope() == this->getScope()) return Reference::compare(r);
  else return false;
}


Bool ScopeReference::setValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::setValue"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListContainer *container = parent->getInterface<NamedListContainer>();
  if (container == 0) {
    index = -1;
    return false;
  }

  if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
      this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
    container->set(this->getListIndex(container, this->cachedIndex), obj);
    index = this->cachedIndex+1;
  } else {
    index = this->findScope(container, index);
    if (index == -1) return false;
    container->set(this->getListIndex(container, index), obj);
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
    ++index;
  }
  return true;
}


Bool ScopeReference::removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::removeValue"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListContainer *container;
  if ((container = parent->getInterface<NamedListContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
      container->remove(this->getListIndex(container, this->cachedIndex));
      index = this->cachedIndex;
    } else {
      index = this->findScope(container, index);
      if (index == -1) return false;
      container->remove(this->getListIndex(container, index));
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
    }
    return true;
  } else {
    index = -1;
    return false;
  }
}


Bool ScopeReference::getValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::getValue"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListContainer const *container;
  if ((container = parent->getInterface<NamedListContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
      result = container->get(this->getListIndex(container, this->cachedIndex));
      index = this->cachedIndex+1;
    } else {
      index = this->findScope(container, index);
      if (index == -1) return false;
      result = container->get(this->getListIndex(container, index));
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
      ++index;
    }
    return true;
  } else {
    index = -1;
    return false;
  }
}

} } // namespace
