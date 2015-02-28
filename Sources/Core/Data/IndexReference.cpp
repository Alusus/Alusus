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


Bool IndexReference::setValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *obj, Int &index) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  Container *container = parent->getInterface<Container>();
  if (container == 0) return false;
  if (this->index >= 0 && this->index < container->getCount()) {
    container->set(this->index, obj);
    return true;
  } else if (this->index == container->getCount()) {
    ListContainer *listContainer = parent->getInterface<ListContainer>();
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


Bool IndexReference::removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  Container *container;
  if ((container = parent->getInterface<Container>()) != 0) {
    if (this->index >= 0 && this->index < container->getCount()) {
      container->remove(this->index);
      return true;
    } else if (this->index < 0 && this->index >= -container->getCount()) {
      container->remove(container->getCount() + this->index);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


Bool IndexReference::getValue(Provider *provider, IdentifiableObject *parent,
                              IdentifiableObject *&result, Int &index) const
{
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Should not be null."));
  }
  if (index == -1) return false;
  index = -1;
  Container const *container;
  if ((container = parent->getInterface<Container>()) != 0) {
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

} } // namespace
