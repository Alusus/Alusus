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


Bool IndirectReference::setValue(Provider *provider, IdentifiableObject *parent,
                                 IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::setValue"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::setValue"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGet(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) return false;
    container->set(static_cast<String*>(ref)->get(), obj);
    return true;
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    Container *container = parent->getInterface<Container>();
    if (container == 0) return false;
    if (i >= 0 && i < container->getCount()) {
      container->set(i, obj);
      return true;
    } else if (i == container->getCount()) {
      ListContainer *listContainer = parent->getInterface<ListContainer>();
      if (listContainer == 0) return false;
      listContainer->add(obj);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->setValue(provider, parent, obj, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::removeValue"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::removeValue"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGet(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapContainer *container;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      Int i = container->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      container->remove(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    Container *container;
    if ((container = parent->getInterface<Container>()) != 0) {
      if (i < 0 || i >= container->getCount()) return false;
      container->remove(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->removeValue(provider, parent, index);
  } else {
    index = -1;
    return false;
  }
}


Bool IndirectReference::getValue(Provider *provider, IdentifiableObject *parent,
                                 IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::IndirectReference::getValue"),
                                   STR("Should not be null."));
  }
  if (provider == 0) {
    throw InvalidArgumentException(STR("provider"), STR("Core::Data::IndirectReference::getValue"),
                                   STR("Should not be null."));
  }
  if (index == -1) return false;
  IdentifiableObject *ref;
  if (!provider->tryGet(this->key.get(), ref)) {
    index = -1;
    return false;
  }
  if (ref == 0) {
    index = -1;
    return false;
  }
  if (ref->isA<String>()) {
    index = -1;
    MapContainer const *container;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      Int i = container->findIndex(static_cast<String*>(ref)->get());
      if (i == -1) return false;
      result = container->get(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Integer>()) {
    index = -1;
    Int i = static_cast<Integer*>(ref)->get();
    Container const *container;
    if ((container = parent->getInterface<Container>()) != 0) {
      if (i < 0 || i >= container->getCount()) return false;
      result = container->get(i);
      return true;
    } else {
      return false;
    }
  } else if (ref->isA<Reference>()) {
    return static_cast<Reference*>(ref)->getValue(provider, parent, result, index);
  } else {
    index = -1;
    return false;
  }
}


void IndirectReference::unsetIndexes(Int from, Int to)
{
  if (this->key != 0) this->key->unsetIndexes(from, to);
  Reference::unsetIndexes(from, to);
}

} } // namespace
