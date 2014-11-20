/**
 * @file Core/Data/SearchReference.cpp
 * Contains the implementation of class Core::Data::SearchReference.
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

Bool SearchReference::compare(Reference const *r) const
{
  const SearchReference *sr = io_cast<SearchReference>(r);
  if (sr != 0 &&
      this->searchValidator->compare(sr->getSearchValidator().get()) &&
      this->forward == sr->getSearchDirection() &&
      this->matchLimitationIndex == sr->getMatchLimitation()) return Reference::compare(r);
  else return false;
}


Bool SearchReference::setShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ScopeReference::set"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  SharedContainer *container = parent->getInterface<SharedContainer>();
  if (container == 0) return false;

  if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
    container->set(this->getListIndex(container, this->cachedIndex), obj);
    if (this->matchLimitationIndex == -1) index = this->cachedIndex+1;
    else index = -1;
  } else {
    if (this->matchLimitationIndex == -1) {
      index = this->findMatch(container, index);
    } else {
      index = this->findMatch(container, 0, this->matchLimitationIndex);
    }
    if (index == -1) return false;
    container->set(this->getListIndex(container, index), obj);
    this->cachedIndex = index;
    if (this->matchLimitationIndex == -1) ++index;
    else index = -1;
  }
  return true;
}


Bool SearchReference::setPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::SearchReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  PlainContainer *container = parent->getInterface<PlainContainer>();
  if (container == 0) return false;

  if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
    container->set(this->getListIndex(container, this->cachedIndex), obj);
    if (this->matchLimitationIndex == -1) index = this->cachedIndex+1;
    else index = -1;
  } else {
    if (this->matchLimitationIndex == -1) {
      index = this->findMatch(container, index);
    } else {
      index = this->findMatch(container, 0, this->matchLimitationIndex);
    }
    if (index == -1) return false;
    container->set(this->getListIndex(container, index), obj);
    this->cachedIndex = index;
    if (this->matchLimitationIndex == -1) ++index;
    else index = -1;
  }
  return true;
}


Bool SearchReference::remove(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::SearchReference::tryRemove"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  SharedContainer *sharedContainer;
  PlainContainer *plainContainer;
  if ((sharedContainer = parent->getInterface<SharedContainer>()) != 0) {
    if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < sharedContainer->getCount()) {
      sharedContainer->remove(this->getListIndex(sharedContainer, this->cachedIndex));
      if (this->matchLimitationIndex == -1) index = this->cachedIndex;
      else index = -1;
    } else {
      if (this->matchLimitationIndex == -1) {
        index = this->findMatch(sharedContainer, index);
      } else {
        index = this->findMatch(sharedContainer, 0, this->matchLimitationIndex);
      }
      if (index == -1) return false;
      sharedContainer->remove(this->getListIndex(sharedContainer, index));
      this->cachedIndex = index;
      if (this->matchLimitationIndex != -1) index = -1;
    }
    return true;
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < plainContainer->getCount()) {
      plainContainer->remove(this->getListIndex(plainContainer, this->cachedIndex));
      if (this->matchLimitationIndex == -1) index = this->cachedIndex;
      else index = -1;
    } else {
      if (this->matchLimitationIndex == -1) {
        index = this->findMatch(plainContainer, index);
      } else {
        index = this->findMatch(plainContainer, 0, this->matchLimitationIndex);
      }
      if (index == -1) return false;
      plainContainer->remove(this->getListIndex(plainContainer, index));
      this->cachedIndex = index;
      if (this->matchLimitationIndex != -1) index = -1;
    }
    return true;
  } else {
    return false;
  }
}


Bool SearchReference::getShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::SearchReference::get"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  SharedContainer const *container;
  if ((container = parent->getInterface<SharedContainer>()) != 0) {
    if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < container->getCount()) {
      result = container->get(this->getListIndex(container, this->cachedIndex));
      if (this->matchLimitationIndex == -1) index = this->cachedIndex+1;
      else index = -1;
    } else {
      if (this->matchLimitationIndex == -1) {
        index = this->findMatch(container, index);
      } else {
        index = this->findMatch(container, 0, this->matchLimitationIndex);
      }
      if (index == -1) return false;
      result = container->get(this->getListIndex(container, index));
      this->cachedIndex = index;
      if (this->matchLimitationIndex == -1) ++index;
      else index = -1;
    }
    return true;
  } else {
    return false;
  }
}


Bool SearchReference::getPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::SearchReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index < 0) return false;
  SharedContainer const *sharedContainer;
  PlainContainer const *plainContainer;
  if ((sharedContainer = parent->getInterface<SharedContainer>()) != 0) {
    if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < sharedContainer->getCount()) {
      result = sharedContainer->get(this->getListIndex(sharedContainer, this->cachedIndex)).get();
      if (this->matchLimitationIndex == -1) index = this->cachedIndex+1;
      else index = -1;
    } else {
      if (this->matchLimitationIndex == -1) {
        index = this->findMatch(sharedContainer, index);
      } else {
        index = this->findMatch(sharedContainer, 0, this->matchLimitationIndex);
      }
      if (index == -1) return false;
      result = sharedContainer->get(this->getListIndex(sharedContainer, index)).get();
      this->cachedIndex = index;
      if (this->matchLimitationIndex == -1) ++index;
      else index = -1;
    }
    return true;
  } else if ((plainContainer = parent->getInterface<NamedListPlainContainer>()) != 0) {
    if (this->isCacheUsable() && this->cachedIndex >= index && this->cachedIndex < plainContainer->getCount()) {
      result = plainContainer->get(this->getListIndex(plainContainer, this->cachedIndex));
      if (this->matchLimitationIndex == -1) index = this->cachedIndex+1;
      else index = -1;
    } else {
      if (this->matchLimitationIndex == -1) {
        index = this->findMatch(sharedContainer, index);
      } else {
        index = this->findMatch(sharedContainer, 0, this->matchLimitationIndex);
      }
      if (index == -1) return false;
      result = plainContainer->get(this->getListIndex(plainContainer, index));
      this->cachedIndex = index;
      if (this->matchLimitationIndex == -1) ++index;
      else index = -1;
    }
    return true;
  } else {
    return false;
  }
}


Int SearchReference::findMatch(SharedContainer const *container, Int index, Int skips) const
{
  ASSERT(container != 0);
  if (index >= container->getCount()) return -1;
  for (Int i = std::max(0, index); i < container->getCount(); ++i) {
    if (this->searchValidator->validate(container->get(this->getListIndex(container,i)).get())) {
      if (skips > 0) --skips;
      else return i;
    }
  }
  return -1;
}


Int SearchReference::findMatch(PlainContainer const *container, Int index, Int skips) const
{
  ASSERT(container != 0);
  if (index >= container->getCount()) return -1;
  for (Int i = std::max(0, index); i < container->getCount(); ++i) {
    if (this->searchValidator->validate(container->get(this->getListIndex(container,i)))) {
      if (skips > 0) --skips;
      else return i;
    }
  }
  return -1;
}

} } // namespace
