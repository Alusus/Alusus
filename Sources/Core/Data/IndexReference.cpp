/**
 * @file Core/Data/IndexReference.cpp
 * Contains the implementation of class Core::Data::IndexReference.
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

Bool IndexReference::compare(Reference const *r) const
{
  const IndexReference *ir = io_cast<const IndexReference>(r);
  if (ir != 0 && ir->getIndex() == this->getIndex()) return Reference::compare(r);
  else return false;
}


void IndexReference::setValue(Provider *provider, IdentifiableObject *parent,
                              ReferenceSetLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container = parent->getInterface<Container>();
  IdentifiableObject *obj;
  if (container == 0) return;
  if (this->index >= 0 && this->index < container->getCount()) {
    obj = container->get(this->index);
    if (isPerform(handler(0, obj))) {
      container->set(this->index, obj);
    }
  } else if (this->index == container->getCount()) {
    ListContainer *listContainer = parent->getInterface<ListContainer>();
    if (listContainer == 0) return;
    obj = 0;
    if (isPerform(handler(0, obj))) {
      listContainer->add(obj);
    }
  } else if (this->index < 0 && this->index >= -container->getCount()) {
    obj = container->get(container->getCount() + this->index);
    if (isPerform(handler(0, obj))) {
      container->set(container->getCount() + this->index, obj);
    }
  }
}


void IndexReference::removeValue(Provider *provider, IdentifiableObject *parent,
                                 ReferenceRemoveLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      if (isPerform(handler(0, container->get(this->index)))) {
        container->remove(this->index);
      }
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      if (isPerform(handler(0, container->get(container->getCount() + this->index)))) {
        container->remove(container->getCount() + this->index);
      }
    }
  }
}


void IndexReference::forEachValue(Provider *provider, IdentifiableObject *parent,
                                  ReferenceForeachLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container const *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      handler(0, container->get(this->index));
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      handler(0, container->get(container->getCount() + this->index));
    }
  }
}


Bool IndexReference::setValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container *container = parent->getInterface<Container>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
  } else if (this->index == container->getCount()) {
    ListContainer *listContainer = parent->getInterface<ListContainer>();
    if (listContainer == 0) return false;
    listContainer->add(obj);
  } else if (this->index < 0 && this->index >= -container->getCount()) {
    container->set(container->getCount() + this->index, obj);
  }
  return true;
}


Bool IndexReference::getValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  Container const *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      result = container->get(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      result = container->get(container->getCount() + this->index);
      return true;
    }
  }
  return false;
}

} } // namespace
