/**
 * @file Core/Data/RawIndirectReferenceSegment.cpp
 * Contains the implementation of class Data::RawIndirectReferenceSegment.
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

Bool RawIndirectReferenceSegment::compare(const ReferenceSegment *r) const
{
  const RawIndirectReferenceSegment *ir = io_cast<RawIndirectReferenceSegment>(r);
  if (ir == 0) {
    return false;
  } else if (ir->getQualifier() == 0 && this->getQualifier() == 0) {
    return true;
  } else if (ir->getQualifier() == 0 || this->getQualifier() == 0) {
    return false;
  } else if (ir->getSize() == -1 && this->getSize() == -1) {
    return compareStr(ir->getQualifier(), this->getQualifier()) == 0;
  } else if (ir->getSize() == -1) {
    return compareStr(ir->getQualifier(), this->getQualifier(), this->getSize()) == 0;
  } else if (this->getSize() == -1) {
    return compareStr(this->getQualifier(), ir->getQualifier(), ir->getSize()) == 0;
  } else if (this->getSize() == ir->getSize()) {
    return compareStr(this->getQualifier(), ir->getQualifier(), ir->getSize()) == 0;
  } else {
    return false;
  }
}


void RawIndirectReferenceSegment::set(const Provider *provider, IdentifiableObject *parent,
                                      const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref = provider->getPlainValue(this->getQualifier());
  if (ref == 0) {
    Str error = STR("couldn't dereference the key. Value not found: ");
    error += Str(this->getQualifier(), this->getSize());
    throw GeneralException(error.c_str(), STR("Core::Data::RawIndirectReferenceSegment::set"));
  }
  if (ref->isA<String>()) {
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                     STR("Object doesn't support the MapContainer interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    container->set(static_cast<String*>(ref)->get(), obj);
  } else if (ref->isA<Integer>()) {
    Container *container = parent->getInterface<Container>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                     STR("Object doesn't support the Container interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    container->set(static_cast<Integer*>(ref)->get(), obj);
  } else {
    throw GeneralException(STR("Key refers to an invalid object type. Object must be String or Integer."),
                           STR("Core::Data::RawIndirectReferenceSegment::set"));
  }
}


Bool RawIndirectReferenceSegment::trySet(const Provider *provider, IdentifiableObject *parent,
                                         const SharedPtr<IdentifiableObject> &obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::set"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->getQualifier(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    Container *container = parent->getInterface<Container>();
    if (container == 0) return false;
    container->set(static_cast<Integer*>(ref)->get(), obj);
    return true;
  } else {
    return false;
  }
}


void RawIndirectReferenceSegment::setPlain(const Provider *provider, IdentifiableObject *parent,
                                           IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"),
                                   STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"),
                                   STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref = provider->getPlainValue(this->getQualifier());
  if (ref == 0) {
    Str error = STR("couldn't dereference the key. Value not found: ");
    error += Str(this->getQualifier(), this->getSize());
    throw GeneralException(error.c_str(), STR("Core::Data::RawIndirectReferenceSegment::setPlain"));
  }
  if (ref->isA<String>()) {
    MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"),
                                     STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                     STR("Object doesn't support the MapPlainContainer interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    container->set(static_cast<String*>(ref)->get(), obj);
  } else if (ref->isA<Integer>()) {
    PlainContainer *container = parent->getInterface<PlainContainer>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"),
                                     STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                     STR("Object doesn't support the PlainContainer interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    container->set(static_cast<Integer*>(ref)->get(), obj);
  } else {
    throw GeneralException(STR("Key refers to an invalid object type. Object must be String or Integer."),
                           STR("Core::Data::RawIndirectReferenceSegment::setPlain"));
  }
}


Bool RawIndirectReferenceSegment::trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                              IdentifiableObject *obj) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"),
                                   STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"),
                                   STR("Core::Data::RawIndirectReferenceSegment::setPlain"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->getQualifier(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapPlainContainer *container = parent->getInterface<MapPlainContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    PlainContainer *container = parent->getInterface<PlainContainer>();
    if (container == 0) return false;
    container->set(static_cast<Integer*>(ref)->get(), obj);
    return true;
  } else {
    return false;
  }
}


void RawIndirectReferenceSegment::remove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref = provider->getPlainValue(this->getQualifier());
  if (ref == 0) {
    Str error = STR("couldn't dereference the key. Value not found: ");
    error += Str(this->getQualifier(), this->getSize());
    throw GeneralException(error.c_str(), STR("Core::Data::RawIndirectReferenceSegment::remove"));
  }
  if (ref->isA<String>()) {
    MapContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      container->remove(static_cast<String*>(ref)->get());
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      plainContainer->remove(static_cast<String*>(ref)->get());
    } else {
      throw InvalidArgumentException(STR("parent"),
                                     STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                     STR("Object must support MapContainer or MapPlainContainer interface "
                                         "for string based keys."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
  } else if (ref->isA<Integer>()) {
    Container *container;
    MapContainer *mapContainer;
    if ((container = parent->getInterface<Container>()) != 0) {
      container->remove(static_cast<Integer*>(ref)->get());
    } else if ((mapContainer = parent->getInterface<MapContainer>()) != 0) {
      mapContainer->remove(static_cast<Integer*>(ref)->get());
    } else {
      throw InvalidArgumentException(STR("parent"),
                                     STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                     STR("Object must support Container or PlainContainer interface "
                                         "for integer based keys."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
  } else {
    throw GeneralException(STR("Key refers to an invalid object type. Object must be String or Integer."),
                           STR("Core::Data::RawIndirectReferenceSegment::remove"));
  }
}


Bool RawIndirectReferenceSegment::tryRemove(const Provider *provider, IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::remove"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->getQualifier(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      container->remove(static_cast<String*>(ref)->get());
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      plainContainer->remove(static_cast<String*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    Container *container;
    MapContainer *mapContainer;
    if ((container = parent->getInterface<Container>()) != 0) {
      container->remove(static_cast<Integer*>(ref)->get());
      return true;
    } else if ((mapContainer = parent->getInterface<MapContainer>()) != 0) {
      mapContainer->remove(static_cast<Integer*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


const SharedPtr<IdentifiableObject>& RawIndirectReferenceSegment::get(const Provider *provider,
                                                                      IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref = provider->getPlainValue(this->getQualifier());
  if (ref == 0) {
    Str error = STR("couldn't dereference the key. Value not found: ");
    error += Str(this->getQualifier(), this->getSize());
    throw GeneralException(error.c_str(), STR("Core::Data::RawIndirectReferenceSegment::get"));
  }
  if (ref->isA<String>()) {
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                     STR("Object doesn't support the MapContainer interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    return container->get(static_cast<String*>(ref)->get());
  } else if (ref->isA<Integer>()) {
    Container *container = parent->getInterface<Container>();
    if (container == 0) {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                     STR("Object doesn't support the Container interface."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
    return container->get(static_cast<Integer*>(ref)->get());
  } else {
    throw GeneralException(STR("Key refers to an invalid object type. Object must be String or Integer."),
                           STR("Core::Data::RawIndirectReferenceSegment::get"));
  }
}


Bool RawIndirectReferenceSegment::tryGet(const Provider *provider, IdentifiableObject *parent,
                                         SharedPtr<IdentifiableObject> &result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::get"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->getQualifier(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) return false;
    result = container->get(static_cast<String*>(ref)->get());
    return true;
  } else if (ref->isA<Integer>()) {
    Container *container = parent->getInterface<Container>();
    if (container == 0) return false;
    result = container->get(static_cast<Integer*>(ref)->get());
    return true;
  } else {
    return false;
  }
}


IdentifiableObject* RawIndirectReferenceSegment::getPlain(const Provider *provider,
                                                          IdentifiableObject *parent) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref = provider->getPlainValue(this->getQualifier());
  if (ref == 0) {
    Str error = STR("couldn't dereference the key. Value not found: ");
    error += Str(this->getQualifier(), this->getSize());
    throw GeneralException(error.c_str(), STR("Core::Data::RawIndirectReferenceSegment::getPlain"));
  }
  if (ref->isA<String>()) {
    MapContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      return container->get(static_cast<String*>(ref)->get()).get();
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      return plainContainer->get(static_cast<String*>(ref)->get());
    } else {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                     STR("Object must support MapContainer or MapPlainContainer interface "
                                         "for string based keys."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
  } else if (ref->isA<Integer>()) {
    Container *container;
    MapContainer *mapContainer;
    if ((container = parent->getInterface<Container>()) != 0) {
      return container->get(static_cast<Integer*>(ref)->get()).get();
    } else if ((mapContainer = parent->getInterface<MapContainer>()) != 0) {
      return mapContainer->get(static_cast<Integer*>(ref)->get()).get();
    } else {
      throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                     STR("Object must support Container or PlainContainer interface "
                                         "for integer based keys."),
                                     parent->getMyTypeInfo()->getUniqueName());
    }
  } else {
    throw GeneralException(STR("Key refers to an invalid object type. Object must be String or Integer."),
                           STR("Core::Data::RawIndirectReferenceSegment::getPlain"));
  }
}


Bool RawIndirectReferenceSegment::tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                              IdentifiableObject *&result) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::RawIndirectReferenceSegment::getPlain"),
                                   STR("Should not be null."));
  }
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->getQualifier(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      result = container->get(static_cast<String*>(ref)->get()).get();
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      result = plainContainer->get(static_cast<String*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    Container *container;
    MapContainer *mapContainer;
    if ((container = parent->getInterface<Container>()) != 0) {
      result = container->get(static_cast<Integer*>(ref)->get()).get();
      return true;
    } else if ((mapContainer = parent->getInterface<MapContainer>()) != 0) {
      result = mapContainer->get(static_cast<Integer*>(ref)->get()).get();
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

} } // namespace
