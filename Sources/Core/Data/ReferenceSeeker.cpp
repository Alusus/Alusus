/**
 * @file Core/Data/ReferenceSeeker.cpp
 * Contains the implementation of class Core::Data::ReferenceSeeker.
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
// Data Read Functions

IdentifiableObject* ReferenceSeeker::get(Reference const *ref, IdentifiableObject *source) const
{
  IdentifiableObject *result;
  if (!this->tryGet(ref, source, result)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
  return result;
}


IdentifiableObject* ReferenceSeeker::tryGet(Reference const *ref, IdentifiableObject *source) const
{
  IdentifiableObject *result = 0;
  this->tryGet(ref, source, result);
  return result;
}


void ReferenceSeeker::get(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                          TypeInfo const *parentTypeInfo, IdentifiableObject **retParent) const
{
  if (!this->tryGet(ref, source, retVal, parentTypeInfo, retParent)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool ReferenceSeeker::tryGet(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                             TypeInfo const *parentTypeInfo, IdentifiableObject **retParent) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }

  if (source == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("source"), STR("Cannot be null."));
  }
  if (ref->getNext() == 0) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (ref->getValue(this->dataProvider, source, tempResult, index)) {
      if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(tempResult) == false) {
        return false;
      }
      retVal = tempResult;
      if (parentTypeInfo != 0 && retParent != 0) {
        if (source->isDerivedFrom(parentTypeInfo)) {
          *retParent = source;
        } else {
          *retParent = 0;
        }
      }
      return true;
    } else {
      return false;
    }
  } else {
    Int index = 0;
    IdentifiableObject *innerSource;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (ref->getValue(this->dataProvider, source, innerSource, index)) {
        if (innerSource->isA<SharedPairedPtr>()) {
          SharedPairedPtr *pairedPtr = static_cast<SharedPairedPtr*>(innerSource);
          if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGet(ref->getNext().get(), pairedPtr->object.get(), retVal, parentTypeInfo, retParent);
            if (ret) {
              if (parentTypeInfo != 0 && retParent != 0) {
                if (*retParent == 0 && pairedPtr->parent->isDerivedFrom(parentTypeInfo)) {
                  *retParent = pairedPtr->parent.get();
                }
                break;
              }
            }
          }
        } else if (innerSource->isA<PlainPairedPtr>()) {
          PlainPairedPtr *pairedPtr = static_cast<PlainPairedPtr*>(innerSource);
          if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGet(ref->getNext().get(), pairedPtr->object, retVal, parentTypeInfo, retParent);
            if (ret) {
              if (parentTypeInfo != 0 && retParent != 0) {
                if (*retParent == 0 && pairedPtr->parent->isDerivedFrom(parentTypeInfo)) {
                  *retParent = pairedPtr->parent;
                }
              }
              break;
            }
          }
        } else {
          if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(innerSource) == true) {
            ret = this->tryGet(ref->getNext().get(), innerSource, retVal, parentTypeInfo, retParent);
            if (ret) break;
          }
        }
      }
    }
    if (ret == true && parentTypeInfo != 0 && retParent != 0 && *retParent == 0) {
      if (source->isDerivedFrom(parentTypeInfo)) *retParent = source;
    }
    return ret;
  }
}


//==============================================================================
// Data Write Functions

void ReferenceSeeker::set(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const
{
  if (!this->trySet(ref, target, val)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool ReferenceSeeker::trySet(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }
  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }
  if (ref->getNext() == 0) {
    Int index = 0;
    if (ref->getResultValidator() != 0 && ref->getResultValidator()->validate(val) == false) return false;
    if (ref->setValue(this->dataProvider, target, val, index)) return true;
    else return false;
  } else {
    Int index = 0;
    IdentifiableObject *innerTarget;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (ref->getValue(this->dataProvider, target, innerTarget, index)) {
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(innerTarget) == true) {
          ret = this->trySet(ref->getNext().get(), innerTarget, val);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}


//============================================================================
// Data Delete Functions

void ReferenceSeeker::remove(Reference const *ref, IdentifiableObject *target) const
{
  if (!this->tryRemove(ref, target)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool ReferenceSeeker::tryRemove(Reference const *ref, IdentifiableObject *target) const
{
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }
  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }
  if (ref->getNext() == 0) {
    Int index = 0;
    if (ref->getResultValidator() != 0) {
      // Validate the object before removing it.
      IdentifiableObject *obj;
      if (ref->getValue(this->dataProvider, target, obj, index)) {
        if (ref->getResultValidator()->validate(obj) == false) return false;
      }
      index = 0;
    }
    if (ref->removeValue(this->dataProvider, target, index)) return true;
    else return false;
  } else {
    Int index = 0;
    IdentifiableObject *innerTarget;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (ref->getValue(this->dataProvider, target, innerTarget, index)) {
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (ref->getResultValidator() == 0 || ref->getResultValidator()->validate(innerTarget) == true) {
          ret = this->tryRemove(ref->getNext().get(), innerTarget);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}

} } // namespace
