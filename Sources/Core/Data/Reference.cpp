/**
 * @file Core/Data/Reference.cpp
 * Contains the implementation of class Core::Data::Reference.
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

Reference* Reference::getLastNode(Reference *reference)
{
  if (reference->getNext() == 0) return reference;
  else {
    Reference *ref = reference->getNext().get();
    while (ref->getNext() != 0) ref = ref->getNext().get();
    return ref;
  }
}


const SharedPtr<Reference>& Reference::getLastNode(SharedPtr<Reference> const &reference)
{
  if (reference->getNext() == 0) return reference;
  else {
    const SharedPtr<Reference> *ref = &reference->getNext();
    while ((*ref)->getNext() != 0) ref = &(*ref)->getNext();
    return *ref;
  }
}


Bool Reference::compare(Reference const *r) const
{
  if (r == 0) return false;
  else if (r->getNext() == 0 && this->getNext() == 0) return true;
  else if (r->getNext() != 0 && this->getNext() != 0) return this->getNext()->compare(r->getNext().get());
  else return false;
}

} } // namespace
