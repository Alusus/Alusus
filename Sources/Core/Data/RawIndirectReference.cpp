/**
 * @file Core/Data/RawIndirectReference.cpp
 * Contains the implementation of class Core::Data::RawIndirectReference.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

Bool RawIndirectReference::compare(Reference const *r) const
{
  const RawIndirectReference *ir = tio_cast<RawIndirectReference>(r);
  if (ir == 0) {
    return false;
  } else if (ir->getQualifier() == 0 && this->getQualifier() == 0) {
    return Reference::compare(r);
  } else if (ir->getQualifier() == 0 || this->getQualifier() == 0) {
    return false;
  } else if (ir->getSize() == -1 && this->getSize() == -1 &&
             compareStr(ir->getQualifier(), this->getQualifier()) == 0) {
    return Reference::compare(r);
  } else if (ir->getSize() == -1 &&
             compareStr(ir->getQualifier(), this->getQualifier(), this->getSize()) == 0) {
    return Reference::compare(r);
  } else if (this->getSize() == -1 &&
             compareStr(this->getQualifier(), ir->getQualifier(), ir->getSize()) == 0) {
    return Reference::compare(r);
  } else if (this->getSize() == ir->getSize() &&
             compareStr(this->getQualifier(), ir->getQualifier(), ir->getSize()) == 0) {
    return Reference::compare(r);
  } else {
    return false;
  }
}


void RawIndirectReference::setValue(Provider *provider, TiObject *parent,
                                    ReferenceSetLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (provider == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("provider"), STR("Should not be null."));
  }
  TiObject *ref;
  if (!provider->tryGet(this->getQualifier(), ref)) return;
  if (ref == 0) return;

  if (ref->isA<TiStr>()) {
    MapContainer *container = parent->getInterface<MapContainer>();
    if (container == 0) return;
    Int index = container->findIndex(static_cast<TiStr*>(ref)->get());
    if (index == -1) {
      TiObject *obj = 0;
      if (isPerform(handler(0, obj))) {
        container->set(static_cast<TiStr*>(ref)->get(), obj);
      }
    } else {
      TiObject *obj = container->get(index);
      if (isPerform(handler(0, obj))) {
        container->set(index, obj);
      }
    }
  } else if (ref->isA<TiInt>()) {
    Int index = static_cast<TiInt*>(ref)->get();
    Container *container = parent->getInterface<Container>();
    if (container == 0) return;
    if (index >= 0 && index < container->getCount()) {
      TiObject *obj = container->get(index);
      if (isPerform(handler(0, obj))) {
        container->set(index, obj);
      }
    } else if (index == container->getCount()) {
      ListContainer *listContainer = parent->getInterface<ListContainer>();
      if (listContainer == 0) return;
      TiObject *obj = 0;
      if (isPerform(handler(0, obj))) {
        listContainer->add(obj);
      }
    } else if (index < 0 && index >= -container->getCount()) {
      TiObject *obj = container->get(container->getCount() + index);
      if (isPerform(handler(0, obj))) {
        container->set(container->getCount() + index, obj);
      }
    }
  } else if (ref->isA<Reference>()) {
    static_cast<Reference*>(ref)->setValue(provider, parent, handler);
  }
}


void RawIndirectReference::removeValue(Provider *provider, TiObject *parent,
                                       ReferenceRemoveLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (provider == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("provider"), STR("Should not be null."));
  }
  TiObject *ref;
  if (!provider->tryGet(this->getQualifier(), ref)) return;
  if (ref == 0) return;

  if (ref->isA<TiStr>()) {
    MapContainer *container;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      Int index = container->findIndex(static_cast<TiStr*>(ref)->get());
      if (index == -1) return;
      if (isPerform(handler(0, container->get(index)))) {
        container->remove(index);
      }
    }
  } else if (ref->isA<TiInt>()) {
    Int index = static_cast<TiInt*>(ref)->get();
    Container *container;
    if ((container = parent->getInterface<Container>()) != 0) {
      if (index >= 0 && index < container->getCount()) {
        if (isPerform(handler(0, container->get(index)))) {
          container->remove(index);
        }
      } else if (index < 0 && index >= -container->getCount()) {
        if (isPerform(handler(0, container->get(container->getCount() + index)))) {
          container->remove(container->getCount() + index);
        }
      }
    }
  } else if (ref->isA<Reference>()) {
    static_cast<Reference*>(ref)->removeValue(provider, parent, handler);
  }
}


void RawIndirectReference::forEachValue(Provider *provider, TiObject *parent,
                                        ReferenceForeachLambda handler) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (provider == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("provider"), STR("Should not be null."));
  }
  TiObject *ref;
  if (!provider->tryGet(this->getQualifier(), ref)) return;
  if (ref == 0) return;

  if (ref->isA<TiStr>()) {
    MapContainer const *container;
    if ((container = parent->getInterface<MapContainer>()) != 0) {
      Int index = container->findIndex(static_cast<TiStr*>(ref)->get());
      if (index == -1) return;
      handler(0, container->get(index));
    }
  } else if (ref->isA<TiInt>()) {
    Int index = static_cast<TiInt*>(ref)->get();
    Container const *container;
    if ((container = parent->getInterface<Container>()) != 0) {
      if (index >= 0 && index < container->getCount()) {
        handler(0, container->get(index));
      } else if (index < 0 && index >= -container->getCount()) {
        handler(0, container->get(container->getCount() + index));
      }
    }
  } else if (ref->isA<Reference>()) {
    static_cast<Reference*>(ref)->forEachValue(provider, parent, handler);
  }
}

} } // namespace
