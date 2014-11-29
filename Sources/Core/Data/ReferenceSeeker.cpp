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

IdentifiableObject* ReferenceSeeker::get(Reference const *seg, IdentifiableObject *parent) const
{
  IdentifiableObject *result;
  if (!this->tryGet(seg, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  return result;
}


IdentifiableObject* ReferenceSeeker::tryGet(Reference const *seg, IdentifiableObject *parent) const
{
  IdentifiableObject *result = 0;
  this->tryGet(seg, parent, result);
  return result;
}


Bool ReferenceSeeker::tryGet(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGet"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGet"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getValue(this->dataProvider, parent, tempResult, index)) {
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult) == false) return false;
      result = tempResult;
      return true;
    } else {
      return false;
    }
  } else {
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
          if (this->tryGet(seg->getNext().get(), innerParent, result)) return true;
        }
      }
    }
    return false;
  }
}


void ReferenceSeeker::get(Reference const *seg, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const
{
  if (!this->tryGet(seg, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
}


Bool ReferenceSeeker::tryGet(Reference const *seg, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGet"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGet"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
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
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getValue(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = static_cast<SharedModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGet(seg->getNext().get(), pairedPtr->object.get(), retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module.get();
              break;
            }
          }
        } else if (innerParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = static_cast<PlainModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGet(seg->getNext().get(), pairedPtr->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              break;
            }
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
            ret = this->tryGet(seg->getNext().get(), innerParent, retVal);
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

void ReferenceSeeker::set(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (!this->trySet(seg, parent, val)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::set"));
  }
}


Bool ReferenceSeeker::trySet(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::trySet"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::trySet"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(val) == false) return false;
    if (seg->setValue(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
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
          ret = this->trySet(seg->getNext().get(), innerParent, val);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}


//============================================================================
// Data Delete Functions

void ReferenceSeeker::remove(Reference const *seg, IdentifiableObject *parent) const
{
  if (!this->tryRemove(seg, parent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::remove"));
  }
}


Bool ReferenceSeeker::tryRemove(Reference const *seg, IdentifiableObject *parent) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::trySet"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::trySet"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
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
          ret = this->tryRemove(seg->getNext().get(), innerParent);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}

} } // namespace
