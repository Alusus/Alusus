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

IdentifiableObject* QualifierSeeker::get(Char const *qualifier, IdentifiableObject *parent) const
{
  IdentifiableObject *result;
  if (!this->tryGet(qualifier, parent, result)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
  return result;
}


IdentifiableObject* QualifierSeeker::tryGet(Char const *qualifier, IdentifiableObject *parent) const
{
  IdentifiableObject *result = 0;
  this->tryGet(qualifier, parent, result);
  return result;
}


Bool QualifierSeeker::tryGet(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getValue(this->dataProvider, parent, tempResult, index)) {
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult) == false) return false;
      result = tempResult;
      return true;
    }
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          if (this->tryGet(qualifier, innerParent, result)) return true;
        }
      }
    }
    return false;
  }
}


void QualifierSeeker::get(Char const *qualifier, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const
{
  if (!this->tryGet(qualifier, parent, retVal)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::tryGet(Char const *qualifier, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getValue(this->dataProvider, parent, tempResult, index)) {
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult) == false) {
        return false;
      }
      retVal.object = tempResult;
      if (parent->isDerivedFrom<Module>()) {
        retVal.module = static_cast<Module*>(parent);
      } else {
        retVal.module = 0;
      }
      return true;
    } else {
      return false;
    }
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = static_cast<SharedModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGet(qualifier, pairedPtr->object.get(), retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module.get();
              break;
            }
          }
        } else if (innerParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = static_cast<PlainModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGet(qualifier, pairedPtr->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              break;
            }
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
            ret = this->tryGet(qualifier, innerParent, retVal);
            if (ret) break;
          }
        }
      }
    }
    if (ret == true && retVal.module == 0) {
      if (parent->isDerivedFrom<Module>()) retVal.module = static_cast<Module*>(parent);
    }
    return ret;
  }
}


//==============================================================================
// Data Write Functions

void QualifierSeeker::set(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (!this->trySet(qualifier, parent, val)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::trySet(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(val) == false) return false;
    if (seg->setValue(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          ret = this->trySet(qualifier, innerParent, val);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}


//==============================================================================
// Data Delete Functions

void QualifierSeeker::remove(Char const *qualifier, IdentifiableObject *parent) const
{
  if (!this->tryRemove(qualifier, parent)) {
    throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
  }
}


Bool QualifierSeeker::tryRemove(Char const *qualifier, IdentifiableObject *parent) const
{
  if (qualifier == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("qualifier"), STR("Cannot be null."));
  }
  if (parent == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("parent"), STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0) {
      // Validate the object before removing it.
      IdentifiableObject *obj;
      if (seg->getValue(this->dataProvider, parent, obj, index)) {
        if (seg->getResultValidator()->validate(obj) == false) return false;
      }
      index = 0;
    }
    if (seg->removeValue(this->dataProvider, parent, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          ret = this->tryRemove(qualifier, innerParent);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}

} } // namespace
