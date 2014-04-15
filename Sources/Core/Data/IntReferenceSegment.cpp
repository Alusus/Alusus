/**
 * @file Core/Data/IntReferenceSegment.cpp
 * Contains the implementation of class Core::Data::IntReferenceSegment.
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

Bool IntReferenceSegment::compare(const ReferenceSegment *r) const
{
  const IntReferenceSegment *ir = io_cast<const IntReferenceSegment>(r);
  if (ir != 0 && ir->getIndex() == this->getIndex()) return true;
  else return false;
}


void IntReferenceSegment::set(const Provider *provider, IdentifiableObject *parent,
                              const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  ListContainer *container = parent->getInterface<ListContainer>();
  if (container == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::set"),
                                   STR("Object doesn't support the ListContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
  } else if (this->index == container->getCount()) {
    container->add(obj);
  } else {
    throw GeneralException(STR("Index is out of range for the given parent."),
                           STR("Core::Data::IntReferenceSegment::set"));
  }
}


Bool IntReferenceSegment::trySet(const Provider *provider, IdentifiableObject *parent,
                                 const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  ListContainer *container = parent->getInterface<ListContainer>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
    return true;
  } else if (this->index == container->getCount()) {
    container->add(obj);
    return true;
  } else {
    return false;
  }
}


void IntReferenceSegment::setPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  ListPlainContainer *container = parent->getInterface<ListPlainContainer>();
  if (container == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::setPlain"),
                                   STR("Object doesn't support the ListPlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
  } else if (this->index == container->getCount()) {
    container->add(obj);
  } else {
    throw GeneralException(STR("Index is out of range for the given parent."),
                           STR("Core::Data::IntReferenceSegment::setPlain"));
  }
}


Bool IntReferenceSegment::trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                      IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  ListPlainContainer *container = parent->getInterface<ListPlainContainer>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
    return true;
  } else if (this->index == container->getCount()) {
    container->add(obj);
    return true;
  } else {
    return false;
  }
}


void IntReferenceSegment::remove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  Container *container;
  PlainContainer *plainContainer;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      throw GeneralException(STR("Index is out of range for the given parent."),
                             STR("Core::Data::IntReferenceSegment::remove"));
    }
    container->remove(this->index);
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) {
      throw GeneralException(STR("Index is out of range for the given parent."),
                             STR("Core::Data::IntReferenceSegment::remove"));
    }
    plainContainer->remove(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::remove"),
                                   STR("Object doesn't support the Container or PlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool IntReferenceSegment::tryRemove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  Container *container;
  PlainContainer *plainContainer;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) return false;
    container->remove(this->index);
    return true;
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) return false;
    plainContainer->remove(this->index);
    return true;
  } else {
    return false;
  }
}


const SharedPtr<IdentifiableObject>& IntReferenceSegment::get(const Provider *provider,
                                                              IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  Container *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      throw GeneralException(STR("Index is out of range for the given parent."),
                             STR("Core::Data::IntReferenceSegment::get"));
    }
    return container->get(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::get"),
                                   STR("Object doesn't support the Container interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool IntReferenceSegment::tryGet(const Provider *provider, IdentifiableObject *parent,
                                 SharedPtr<IdentifiableObject> &result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  Container *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) return false;
    result = container->get(this->index);
    return true;
  } else {
    return false;
  }
}


IdentifiableObject* IntReferenceSegment::getPlain(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  Container *container;
  PlainContainer *plainContainer;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      throw GeneralException(STR("Index is out of range for the given parent."),
                             STR("Core::Data::IntReferenceSegment::getPlain"));
    }
    return container->get(this->index).get();
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      throw GeneralException(STR("Index is out of range for the given parent."),
                             STR("Core::Data::IntReferenceSegment::getPlain"));
    }
    return plainContainer->get(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::getPlain"),
                                   STR("Object doesn't support the Container or PlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool IntReferenceSegment::tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                      IdentifiableObject *&result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IntReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  Container *container;
  PlainContainer *plainContainer;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) return false;
    result = container->get(this->index).get();
    return true;
  } else if ((plainContainer = parent->getInterface<PlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) return false;
    result = plainContainer->get(this->index);
    return true;
  } else {
    return false;
  }
}

} } // namespace
