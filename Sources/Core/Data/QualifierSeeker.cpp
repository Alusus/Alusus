/**
 * @file Core/Data/QualifierSeeker.cpp
 * Contains the implementation of class Core::Data::QualifierSeeker.
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

IdentifiableObject* QualifierSeeker::get(Char const *qualifier, IdentifiableObject *source) const
{
  IdentifiableObject *result;
  if (!this->tryGet(qualifier, source, result)) {
    throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
  }
  return result;
}


IdentifiableObject* QualifierSeeker::tryGet(Char const *qualifier, IdentifiableObject *source) const
{
  IdentifiableObject *result = 0;
  this->tryGet(qualifier, source, result);
  return result;
}


void QualifierSeeker::get(Char const *qualifier, IdentifiableObject *source, IdentifiableObject *&retVal,
                          TypeInfo const *parentTypeInfo, IdentifiableObject **retParent) const
{
  if (!this->tryGet(qualifier, source, retVal, parentTypeInfo, retParent)) {
    throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::tryGet(Char const *qualifier, IdentifiableObject *source, IdentifiableObject *&retVal,
                             TypeInfo const *parentTypeInfo, IdentifiableObject **retParent) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (source == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("source"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getValue(this->dataProvider, source, tempResult, index)) {
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult) == false) {
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
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerSource;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, source, innerSource, index)) {
        if (innerSource->isA<SharedPairedPtr>()) {
          SharedPairedPtr *pairedPtr = static_cast<SharedPairedPtr*>(innerSource);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGet(qualifier, pairedPtr->object.get(), retVal, parentTypeInfo, retParent);
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
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGet(qualifier, pairedPtr->object, retVal, parentTypeInfo, retParent);
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
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerSource) == true) {
            ret = this->tryGet(qualifier, innerSource, retVal, parentTypeInfo, retParent);
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

void QualifierSeeker::set(Char const *qualifier, IdentifiableObject *target, IdentifiableObject *val) const
{
  if (!this->trySet(qualifier, target, val)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::trySet(Char const *qualifier, IdentifiableObject *target, IdentifiableObject *val) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(val) == false) return false;
    if (seg->setValue(this->dataProvider, target, val, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerTarget;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, target, innerTarget, index)) {
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerTarget) == true) {
          ret = this->trySet(qualifier, innerTarget, val);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}


//==============================================================================
// Data Delete Functions

void QualifierSeeker::remove(Char const *qualifier, IdentifiableObject *target) const
{
  if (!this->tryRemove(qualifier, target)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::tryRemove(Char const *qualifier, IdentifiableObject *target) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (target == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("target"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0) {
      // Validate the object before removing it.
      IdentifiableObject *obj;
      if (seg->getValue(this->dataProvider, target, obj, index)) {
        if (seg->getResultValidator()->validate(obj) == false) return false;
      }
      index = 0;
    }
    if (seg->removeValue(this->dataProvider, target, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerTarget;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, target, innerTarget, index)) {
        if (innerTarget->isA<PlainPairedPtr>()) {
          innerTarget = static_cast<PlainPairedPtr*>(innerTarget)->object;
        } else if (innerTarget->isA<SharedPairedPtr>()) {
          innerTarget = static_cast<SharedPairedPtr*>(innerTarget)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerTarget) == true) {
          ret = this->tryRemove(qualifier, innerTarget);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}

} } // namespace
