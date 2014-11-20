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


Bool ScopeReference::setShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::set"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListSharedContainer *container = parent->getInterface<NamedListSharedContainer>();
  if (container == 0) return false;

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


Bool ScopeReference::setPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListPlainContainer *container = parent->getInterface<NamedListPlainContainer>();
  if (container == 0) return false;

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


Bool ScopeReference::remove(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::tryRemove"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListSharedContainer *sharedContainer;
  NamedListPlainContainer *plainContainer;
  if ((sharedContainer = parent->getInterface<NamedListSharedContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < sharedContainer->getCount()) {
      sharedContainer->remove(this->getListIndex(sharedContainer, this->cachedIndex));
      index = this->cachedIndex;
    } else {
      index = this->findScope(sharedContainer, index);
      if (index == -1) return false;
      sharedContainer->remove(this->getListIndex(sharedContainer, index));
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
    }
    return true;
  } else if ((plainContainer = parent->getInterface<NamedListPlainContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < plainContainer->getCount()) {
      plainContainer->remove(this->getListIndex(plainContainer, this->cachedIndex));
      index = this->cachedIndex;
    } else {
      index = this->findScope(plainContainer, index);
      if (index == -1) return false;
      plainContainer->remove(this->getListIndex(plainContainer, index));
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
    }
    return true;
  } else {
    return false;
  }
}


Bool ScopeReference::getShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::get"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListSharedContainer const *container;
  if ((container = parent->getInterface<NamedListSharedContainer>()) != 0) {
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
    return false;
  }
}


Bool ScopeReference::getPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  NamedListSharedContainer const *sharedContainer;
  NamedListPlainContainer const *plainContainer;
  if ((sharedContainer = parent->getInterface<NamedListSharedContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < sharedContainer->getCount()) {
      result = sharedContainer->get(this->getListIndex(sharedContainer, this->cachedIndex)).get();
      index = this->cachedIndex+1;
    } else {
      index = this->findScope(sharedContainer, index);
      if (index == -1) return false;
      result = sharedContainer->get(this->getListIndex(sharedContainer, index)).get();
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
      ++index;
    }
    return true;
  } else if ((plainContainer = parent->getInterface<NamedListPlainContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH &&
        this->cachedIndex >= index && this->cachedIndex < plainContainer->getCount()) {
      result = plainContainer->get(this->getListIndex(plainContainer, this->cachedIndex));
      index = this->cachedIndex+1;
    } else {
      index = this->findScope(plainContainer, index);
      if (index == -1) return false;
      result = plainContainer->get(this->getListIndex(plainContainer, index));
      if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) this->cachedIndex = index;
      ++index;
    }
    return true;
  } else {
    return false;
  }
}

} } // namespace
