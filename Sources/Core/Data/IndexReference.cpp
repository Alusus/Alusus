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


Bool IndexReference::setShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndexReference::set"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  SharedContainer *container = parent->getInterface<SharedContainer>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
    return true;
  } else if (this->index == container->getCount()) {
    ListSharedContainer *listContainer = parent->getInterface<ListSharedContainer>();
    if (listContainer == 0) return false;
    listContainer->add(obj);
    return true;
  } else if (this->index < 0 && this->index >= -container->getCount()) {
    container->set(container->getCount() + this->index, obj);
    return true;
  } else {
    return false;
  }
}


Bool IndexReference::setPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndexReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  PlainContainer *container = parent->getInterface<PlainContainer>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
    return true;
  } else if (this->index == container->getCount()) {
    ListPlainContainer *listContainer = parent->getInterface<ListPlainContainer>();
    if (listContainer == 0) return false;
    listContainer->add(obj);
    return true;
  } else if (this->index < 0 && this->index >= -container->getCount()) {
    container->set(container->getCount() + this->index, obj);
    return true;
  } else {
    return false;
  }
}


Bool IndexReference::remove(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndexReference::remove"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  SharedContainer *container;
  PlainContainer *plainContainer;
  if ((container = parent->getInterface<SharedContainer>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      container->remove(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      container->remove(container->getCount() + this->index);
      return true;
    } else {
      return false;
    }
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index >= 0 && this->index < plainContainer->getCount()) {
      plainContainer->remove(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -plainContainer->getCount()) {
      plainContainer->remove(plainContainer->getCount() + this->index);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


Bool IndexReference::getShared(Provider *provider, IdentifiableObject *parent,
                               SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndexReference::get"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  SharedContainer const *container;
  if ((container = parent->getInterface<SharedContainer>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      result = container->get(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      result = container->get(container->getCount() + this->index);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


Bool IndexReference::getPlain(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndexReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  SharedContainer const *container;
  PlainContainer const *plainContainer;
  if ((container = parent->getInterface<SharedContainer>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      result = container->get(this->index).get();
      return true;
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      result = container->get(container->getCount() + this->index).get();
      return true;
    } else {
      return false;
    }
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index >= 0 && this->index < plainContainer->getCount()) {
      result = plainContainer->get(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -plainContainer->getCount()) {
      result = plainContainer->get(plainContainer->getCount() + this->index);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

} } // namespace
