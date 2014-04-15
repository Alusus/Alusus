/**
 * @file Core/Data/StrReferenceSegment.cpp
 * Contains the implementation of class Core::Data::StrReferenceSegment.
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

Bool StrReferenceSegment::compare(const ReferenceSegment *r) const
{
  const StrReferenceSegment *sr = io_cast<StrReferenceSegment>(r);
  if (sr != 0 && sr->getKey() == this->getKey()) return true;
  else return false;
}


void StrReferenceSegment::set(const Provider *provider, IdentifiableObject *parent,
                              const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  MapContainer *container = parent->getInterface<MapContainer>();
  if (container == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::set"),
                                   STR("Object doesn't support the MapContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
  if (this->index < 0 || this->index >= container->getCount()) {
    this->index = container->findIndex(this->key.c_str());
  }
  if (this->index == -1) {
    this->index = container->set(this->key.c_str(), obj);
  } else {
    container->set(this->index, obj);
  }
}


Bool StrReferenceSegment::trySet(const Provider *provider, IdentifiableObject *parent,
                                 const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  MapContainer *container = parent->getInterface<MapContainer>();
  if (container == 0) return false;
  if (this->index < 0 || this->index >= container->getCount()) {
    this->index = container->findIndex(this->key.c_str());
  }
  if (this->index == -1) {
    this->index = container->set(this->key.c_str(), obj);
  } else {
    container->set(this->index, obj);
  }
  return true;
}


void StrReferenceSegment::setPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
  if (container == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::setPlain"),
                                   STR("Object doesn't support the MapPlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
  if (this->index < 0 || this->index >= container->getCount()) {
    this->index = container->findIndex(this->key.c_str());
  }
  if (this->index == -1) {
    this->index = container->set(this->key.c_str(), obj);
  } else {
    container->set(this->index, obj);
  }
}


Bool StrReferenceSegment::trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                      IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
  if (container == 0) return false;
  if (this->index < 0 || this->index >= container->getCount()) {
    this->index = container->findIndex(this->key.c_str());
  }
  if (this->index == -1) {
    this->index = container->set(this->key.c_str(), obj);
  } else {
    container->set(this->index, obj);
  }
  return true;
}


void StrReferenceSegment::remove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  MapPlainContainer *plainContainer;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) {
        throw GeneralException(STR("Key not found in the provided parent object."),
                               STR("Core::Data::StrReferenceSegment::remove"));
      }
    }
    container->remove(this->index);
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) {
      this->index = plainContainer->findIndex(this->key.c_str());
      if (this->index == -1) {
        throw GeneralException(STR("Key not found in the provided parent object."),
                               STR("Core::Data::StrReferenceSegment::remove"));
      }
    }
    plainContainer->remove(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::remove"),
                                   STR("Object doesn't support the MapContainer or MapPlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool StrReferenceSegment::tryRemove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  MapPlainContainer *plainContainer;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) return false;
    }
    container->remove(this->index);
    return true;
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) {
      this->index = plainContainer->findIndex(this->key.c_str());
      if (this->index == -1) return false;
    }
    plainContainer->remove(this->index);
    return true;
  } else {
    return false;
  }
}


const SharedPtr<IdentifiableObject>& StrReferenceSegment::get(const Provider *provider,
                                                              IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) {
        throw GeneralException(STR("Key not found in the provided parent object."),
                               STR("Core::Data::StrReferenceSegment::remove"));
      }
    }
    return container->get(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::get"),
                                   STR("Object doesn't support the MapContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool StrReferenceSegment::tryGet(const Provider *provider, IdentifiableObject *parent,
                                 SharedPtr<IdentifiableObject> &result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) return false;
    }
    result = container->get(this->index);
    return true;
  } else {
    return false;
  }
}


IdentifiableObject* StrReferenceSegment::getPlain(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  MapPlainContainer *plainContainer;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) {
        throw GeneralException(STR("Key not found in the provided parent object."),
                               STR("Core::Data::StrReferenceSegment::getPlain"));
      }
    }
    return container->get(this->index).get();
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) {
      this->index = plainContainer->findIndex(this->key.c_str());
      if (this->index == -1) {
        throw GeneralException(STR("Key not found in the provided parent object."),
                               STR("Core::Data::StrReferenceSegment::getPlain"));
      }
    }
    return plainContainer->get(this->index);
  } else {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::getPlain"),
                                   STR("Object doesn't support the MapContainer or MapPlainContainer interface."),
                                   parent->getMyTypeInfo()->getUniqueName());
  }
}


Bool StrReferenceSegment::tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                      IdentifiableObject *&result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::StrReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  MapContainer *container;
  MapPlainContainer *plainContainer;
  if ((container = parent->getInterface<MapContainer>()) != 0) {
    if (this->index < 0 || this->index >= container->getCount()) {
      this->index = container->findIndex(this->key.c_str());
      if (this->index == -1) return false;
    }
    result = container->get(this->index).get();
    return true;
  } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
    if (this->index < 0 || this->index >= plainContainer->getCount()) {
      this->index = plainContainer->findIndex(this->key.c_str());
      if (this->index == -1) return false;
    }
    result = plainContainer->get(this->index);
    return true;
  } else {
    return false;
  }
}

} } // namespace
