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


void SearchReference::setValue(Provider *provider, IdentifiableObject *parent,
                               ReferenceSetLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container = parent->getInterface<Container>();
  if (container == 0) return;

  if (this->isCacheUsable() && this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    IdentifiableObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    if (isPerform(handler(0, obj))) {
      container->set(this->getListIndex(container, this->cachedIndex), obj);
    }
  } else {
    Int i = 0;
    Int index = 0;
    if (this->matchLimitationIndex == -1) {
      while (true) {
        index = this->findMatch(container, index);
        if (index == -1) break;
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
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
    } else {
      index = this->findMatch(container, 0, this->matchLimitationIndex);
      if (index != -1) {
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
        if (isPerform(handler(i, obj))) {
          container->set(this->getListIndex(container, index), obj);
        }
        if (this->isCacheUsable()) {
          this->cachedIndex = index;
        }
      }
    }
  }
}


void SearchReference::removeValue(Provider *provider, IdentifiableObject *parent,
                                  ReferenceRemoveLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container = parent->getInterface<Container>();
  if (container == 0) return;

  if (this->isCacheUsable() && this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    IdentifiableObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    if (isPerform(handler(0, obj))) {
      container->remove(this->getListIndex(container, this->cachedIndex));
    }
  } else {
    Int i = 0;
    Int index = 0;
    if (this->matchLimitationIndex == -1) {
      while (true) {
        index = this->findMatch(container, index);
        if (index == -1) break;
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
        if (isPerform(handler(i, obj))) {
          container->remove(this->getListIndex(container, index));
        }
        if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) {
          this->cachedIndex = index;
          break;
        }
        ++i;
      }
    } else {
      index = this->findMatch(container, 0, this->matchLimitationIndex);
      if (index != -1) {
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
        if (isPerform(handler(i, obj))) {
          container->remove(this->getListIndex(container, index));
        }
        if (this->isCacheUsable()) {
          this->cachedIndex = index;
        }
      }
    }
  }
}


void SearchReference::forEachValue(Provider *provider, IdentifiableObject *parent,
                                   ReferenceForeachLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container = parent->getInterface<Container>();
  if (container == 0) return;

  if (this->isCacheUsable() && this->cachedIndex >= 0 && this->cachedIndex < container->getCount()) {
    IdentifiableObject *obj = container->get(this->getListIndex(container, this->cachedIndex));
    handler(0, obj);
  } else {
    Int i = 0;
    Int index = 0;
    if (this->matchLimitationIndex == -1) {
      while (true) {
        index = this->findMatch(container, index);
        if (index == -1) break;
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
        handler(i, obj);
        if (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) {
          this->cachedIndex = index;
          break;
        }
        ++index;
        ++i;
      }
    } else {
      index = this->findMatch(container, 0, this->matchLimitationIndex);
      if (index != -1) {
        IdentifiableObject *obj = container->get(this->getListIndex(container, index));
        handler(i, obj);
        if (this->isCacheUsable()) {
          this->cachedIndex = index;
        }
      }
    }
  }
}


Int SearchReference::findMatch(Container const *container, Int index, Int skips) const
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
