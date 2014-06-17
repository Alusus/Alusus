/**
 * @file Core/Data/IndirectReference.cpp
 * Contains the implementation of class Core::Data::IndirectReference.
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

Bool IndirectReference::compare(Reference const *r) const
{
  const IndirectReference *ir = io_cast<IndirectReference>(r);
  if (ir == 0) return false;
  else if (ir->getKey() != 0 && this->key != 0 && this->key->compare(ir->getKey().get())) {
    return Reference::compare(r);
  } else if (ir->getKey() == 0 && this->key == 0) return Reference::compare(r);
  else return false;
}


Bool IndirectReference::setShared(Provider const *provider, IdentifiableObject *parent,
                                  SharedPtr<IdentifiableObject> const &obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::set"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::set"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapSharedContainer *container = parent->getInterface<MapSharedContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    SharedContainer *container = parent->getInterface<SharedContainer>();
    if (container == 0) return false;
    container->set(static_cast<Integer*>(ref)->get(), obj);
    return true;
  } else {
    return false;
  }
}


Bool IndirectReference::setPlain(Provider const *provider, IdentifiableObject *parent,
                                 IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::setPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) return false;
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


Bool IndirectReference::remove(Provider const *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::remove"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::remove"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapSharedContainer *container;
    MapPlainContainer *plainContainer;
    if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
      container->remove(static_cast<String*>(ref)->get());
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      plainContainer->remove(static_cast<String*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    SharedContainer *container;
    MapSharedContainer *mapContainer;
    if ((container = parent->getInterface<SharedContainer>()) != 0) {
      container->remove(static_cast<Integer*>(ref)->get());
      return true;
    } else if ((mapContainer = parent->getInterface<MapSharedContainer>()) != 0) {
      mapContainer->remove(static_cast<Integer*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


Bool IndirectReference::getShared(Provider const *provider, IdentifiableObject const *parent,
                                  SharedPtr<IdentifiableObject> &result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::get"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::get"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapSharedContainer const *container = parent->getInterface<MapSharedContainer>();
    if (container == 0) return false;
    result = container->get(static_cast<String*>(ref)->get());
    return true;
  } else if (ref->isA<Integer>()) {
    SharedContainer const *container = parent->getInterface<SharedContainer>();
    if (container == 0) return false;
    result = container->get(static_cast<Integer*>(ref)->get());
    return true;
  } else {
    return false;
  }
}


Bool IndirectReference::getPlain(Provider const *provider, IdentifiableObject const *parent,
                                 IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::getPlain"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  IdentifiableObject *ref;
  if (!provider->tryGetPlainValue(this->key.get(), ref)) return false;
  if (ref == 0) return false;
  if (ref->isA<String>()) {
    MapSharedContainer const *container;
    MapPlainContainer const *plainContainer;
    if ((container = parent->getInterface<MapSharedContainer>()) != 0) {
      result = container->get(static_cast<String*>(ref)->get()).get();
      return true;
    } else if ((plainContainer = parent->getInterface<MapPlainContainer>()) != 0) {
      result = plainContainer->get(static_cast<String*>(ref)->get());
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    SharedContainer const *container;
    MapSharedContainer const *mapContainer;
    if ((container = parent->getInterface<SharedContainer>()) != 0) {
      result = container->get(static_cast<Integer*>(ref)->get()).get();
      return true;
    } else if ((mapContainer = parent->getInterface<MapSharedContainer>()) != 0) {
      result = mapContainer->get(static_cast<Integer*>(ref)->get()).get();
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


void IndirectReference::unsetIndexes(Int from, Int to)
{
  if (this->key != 0) this->key->unsetIndexes(from, to);
  Reference::unsetIndexes(from, to);
}

} } // namespace
