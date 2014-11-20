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


Bool StrKeyReference::setShared(Provider *provider, IdentifiableObject *parent,
                                SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrKeyReference::set"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  MapSharedContainer *container = parent->getInterface<MapSharedContainer>();
  if (container == 0) return false;
  if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
    container->set(this->key.c_str(), obj);
  } else {
    if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
      this->cachedIndex = container->set(this->key.c_str(), obj);
    } else {
      container->set(this->cachedIndex, obj);
    }
  }
  return true;
}


Bool StrKeyReference::setPlain(Provider *provider, IdentifiableObject *parent,
                               IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrKeyReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
  if (container == 0) return false;
  if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
    container->set(this->key.c_str(), obj);
  } else {
    if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
      this->cachedIndex = container->set(this->key.c_str(), obj);
    } else {
      container->set(this->cachedIndex, obj);
    }
  }
  return true;
}


Bool StrKeyReference::remove(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrKeyReference::remove"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  MapSharedContainer *container;
  MapPlainContainer *plainContainer;
  if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
      Int i = container->findIndex(this->key.c_str());
      if (i == -1) return false;
      container->remove(i);
      return true;
    } else {
      if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
        this->cachedIndex = container->findIndex(this->key.c_str());
        if (this->cachedIndex == -1) return false;
      }
      container->remove(this->cachedIndex);
      return true;
    }
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
      Int i = plainContainer->findIndex(this->key.c_str());
      if (i == -1) return false;
      plainContainer->remove(i);
      return true;
    } else {
      if (this->cachedIndex < 0 || this->cachedIndex >= plainContainer->getCount()) {
        this->cachedIndex = plainContainer->findIndex(this->key.c_str());
        if (this->cachedIndex == -1) return false;
      }
      plainContainer->remove(this->cachedIndex);
      return true;
    }
  } else {
    return false;
  }
}


Bool StrKeyReference::getShared(Provider *provider, IdentifiableObject *parent,
                                SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrKeyReference::get"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  MapSharedContainer const *container;
  if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
      Int i = container->findIndex(this->key.c_str());
      if (i == -1) return false;
      result = container->get(i);
      return true;
    } else {
      if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
        this->cachedIndex = container->findIndex(this->key.c_str());
        if (this->cachedIndex == -1) return false;
      }
      result = container->get(this->cachedIndex);
      return true;
    }
  } else {
    return false;
  }
}


Bool StrKeyReference::getPlain(Provider *provider, IdentifiableObject *parent,
                               IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrKeyReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  MapSharedContainer const *container;
  MapPlainContainer const *plainContainer;
  if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
      Int i = container->findIndex(this->key.c_str());
      if (i == -1) return false;
      result = container->get(i).get();
      return true;
    } else {
      if (this->cachedIndex < 0 || this->cachedIndex >= container->getCount()) {
        this->cachedIndex = container->findIndex(this->key.c_str());
        if (this->cachedIndex == -1) return false;
      }
      result = container->get(this->cachedIndex).get();
      return true;
    }
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->usageCriteria == ReferenceUsageCriteria::MULTI_DATA) {
      Int i = plainContainer->findIndex(this->key.c_str());
      if (i == -1) return false;
      result = plainContainer->get(i);
      return true;
    } else {
      if (this->cachedIndex < 0 || this->cachedIndex >= plainContainer->getCount()) {
        this->cachedIndex = plainContainer->findIndex(this->key.c_str());
        if (this->cachedIndex == -1) return false;
      }
      result = plainContainer->get(this->cachedIndex);
      return true;
    }
  } else {
    return false;
  }
}

} } // namespace
