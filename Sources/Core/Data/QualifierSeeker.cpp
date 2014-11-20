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

SharedPtr<IdentifiableObject> QualifierSeeker::getShared(Char const *qualifier, IdentifiableObject *parent) const
{
  SharedPtr<IdentifiableObject> result;
  if (!this->tryGetShared(qualifier, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getShared"));
  }
  return result;
}


SharedPtr<IdentifiableObject> QualifierSeeker::tryGetShared(Char const *qualifier, IdentifiableObject *parent) const
{
  SharedPtr<IdentifiableObject> result;
  this->tryGetShared(qualifier, parent, result);
  return result;
}


Bool QualifierSeeker::tryGetShared(Char const *qualifier, IdentifiableObject *parent,
                                   SharedPtr<IdentifiableObject> &result) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    SharedPtr<IdentifiableObject> tempResult;
    if (seg->getShared(this->dataProvider, parent, tempResult, index)) {
      // Validate the result if a validator exists.
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult.get()) == false) return false;
      result = tempResult;
      return true;
    } else {
      return false;
    }
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        // Validate the parent if the reference has a validator.
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          if (this->tryGetShared(qualifier, innerParent, result)) return true;
        }
      }
    }
    return false;
  }
}


void QualifierSeeker::getShared(Char const *qualifier, IdentifiableObject *parent,
                                SharedModulePairedPtr &retVal) const
{
  if (!this->tryGetShared(qualifier, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getShared"));
  }
}


Bool QualifierSeeker::tryGetShared(Char const *qualifier, IdentifiableObject *parent,
                                   SharedModulePairedPtr &retVal) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    SharedPtr<IdentifiableObject> tempResult;
    if (seg->getShared(this->dataProvider, parent, tempResult, index)) {
      // Validate the result if the reference has a validator.
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult.get()) == false) {
        return false;
      }
      retVal.object = tempResult;
      retVal.module.reset();
      return true;
    } else {
      return false;
    }
  } else {
    ++qualifier;
    Int index = 0;
    SharedPtr<IdentifiableObject> innerSharedParent;
    IdentifiableObject *innerPlainParent;
    while (index != -1) {
      if (seg->getShared(this->dataProvider, parent, innerSharedParent, index)) {
        if (innerSharedParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = innerSharedParent.s_cast_get<SharedModulePairedPtr>();
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            if (this->tryGetShared(qualifier, pairedPtr->object, retVal)) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              return true;
            }
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = innerSharedParent.s_cast_get<PlainModulePairedPtr>();
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            if (this->tryGetShared(qualifier, pairedPtr->object, retVal)) {
              return true;
            }
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerSharedParent.get()) == true) {
            if (this->tryGetShared(qualifier, innerSharedParent, retVal)) return true;
          }
        }
      }
    }
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerPlainParent, index)) {
        if (innerPlainParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = static_cast<SharedModulePairedPtr*>(innerPlainParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            if (this->tryGetShared(qualifier, pairedPtr->object, retVal)) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              return true;
            }
          }
        } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = static_cast<PlainModulePairedPtr*>(innerPlainParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            if (this->tryGetShared(qualifier, pairedPtr->object, retVal)) {
              return true;
            }
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerPlainParent) == true) {
            if (this->tryGetShared(qualifier, innerPlainParent, retVal)) return true;
          }
        }
      }
    }
    return false;
  }
}


void QualifierSeeker::getShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                                SharedModulePairedPtr &retVal) const
{
  if (!this->tryGetShared(qualifier, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getShared"));
  }
}


Bool QualifierSeeker::tryGetShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                                   SharedModulePairedPtr &retVal) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    SharedPtr<IdentifiableObject> tempResult;
    if (seg->getShared(this->dataProvider, parent.get(), tempResult, index)) {
      if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(tempResult.get()) == false) {
        return false;
      }
      retVal.object = tempResult;
      if (parent->isDerivedFrom<Module>()) {
        retVal.module = parent.s_cast<Module>();
      } else {
        retVal.module.reset();
      }
      return true;
    } else {
      return false;
    }
  } else {
    ++qualifier;
    Int index = 0;
    SharedPtr<IdentifiableObject> innerSharedParent;
    IdentifiableObject *innerPlainParent;
    Bool ret = false;
    while (index != -1) {
      if (seg->getShared(this->dataProvider, parent.get(), innerSharedParent, index)) {
        if (innerSharedParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = innerSharedParent.s_cast_get<SharedModulePairedPtr>();
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGetShared(qualifier, pairedPtr->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              break;
            }
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = innerSharedParent.s_cast_get<PlainModulePairedPtr>();
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGetShared(qualifier, pairedPtr->object, retVal);
            if (ret) break;
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerSharedParent.get()) == true) {
            ret = this->tryGetShared(qualifier, innerSharedParent, retVal);
            if (ret) break;
          }
        }
      }
    }
    if (!ret) {
      index = 0;
      while (index != -1) {
        if (seg->getPlain(this->dataProvider, parent.get(), innerPlainParent, index)) {
          if (innerPlainParent->isA<SharedModulePairedPtr>()) {
            SharedModulePairedPtr *pairedPtr = static_cast<SharedModulePairedPtr*>(innerPlainParent);
            if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
              ret = this->tryGetShared(qualifier, pairedPtr->object, retVal);
              if (ret) {
                if (retVal.module == 0) retVal.module = pairedPtr->module;
                break;
              }
            }
          } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
            PlainModulePairedPtr *pairedPtr = static_cast<PlainModulePairedPtr*>(innerPlainParent);
            if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
              ret = this->tryGetShared(qualifier, pairedPtr->object, retVal);
              if (ret) break;
            }
          } else {
            if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerPlainParent) == true) {
              ret = this->tryGetShared(qualifier, innerPlainParent, retVal);
              if (ret) break;
            }
          }
        }
      }
    }
    if (ret == true && retVal.module == 0) {
      if (parent->isDerivedFrom<Module>()) retVal.module = parent.s_cast<Module>();
    }
    return ret;
  }
}


//==============================================================================
// Plain Data Read Functions


IdentifiableObject* QualifierSeeker::getPlain(Char const *qualifier, IdentifiableObject *parent) const
{
  IdentifiableObject *result;
  if (!this->tryGetPlain(qualifier, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getPlain"));
  }
  return result;
}


IdentifiableObject* QualifierSeeker::tryGetPlain(Char const *qualifier, IdentifiableObject *parent) const
{
  IdentifiableObject *result = 0;
  this->tryGetPlain(qualifier, parent, result);
  return result;
}


Bool QualifierSeeker::tryGetPlain(Char const *qualifier, IdentifiableObject *parent,
                                  IdentifiableObject *&result) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getPlain(this->dataProvider, parent, tempResult, index)) {
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
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          if (this->tryGetPlain(qualifier, innerParent, result)) return true;
        }
      }
    }
    return false;
  }
}


void QualifierSeeker::getPlain(Char const *qualifier, IdentifiableObject *parent,
                               PlainModulePairedPtr &retVal) const
{
  if (!this->tryGetPlain(qualifier, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getPlain"));
  }
}


Bool QualifierSeeker::tryGetPlain(Char const *qualifier, IdentifiableObject *parent,
                                  PlainModulePairedPtr &retVal) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    IdentifiableObject *tempResult;
    if (seg->getPlain(this->dataProvider, parent, tempResult, index)) {
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
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<SharedModulePairedPtr>()) {
          SharedModulePairedPtr *pairedPtr = static_cast<SharedModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object.get()) == true) {
            ret = this->tryGetPlain(qualifier, pairedPtr->object.get(), retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module.get();
              break;
            }
          }
        } else if (innerParent->isA<PlainModulePairedPtr>()) {
          PlainModulePairedPtr *pairedPtr = static_cast<PlainModulePairedPtr*>(innerParent);
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(pairedPtr->object) == true) {
            ret = this->tryGetPlain(qualifier, pairedPtr->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = pairedPtr->module;
              break;
            }
          }
        } else {
          if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
            ret = this->tryGetPlain(qualifier, innerParent, retVal);
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

void QualifierSeeker::setShared(Char const *qualifier, IdentifiableObject *parent, SharedPtr<IdentifiableObject> const &val) const
{
  if (!this->trySetShared(qualifier, parent, val)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::setShared"));
  }
}


Bool QualifierSeeker::trySetShared(Char const *qualifier, IdentifiableObject *parent,
                                   SharedPtr<IdentifiableObject> const &val) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::trySetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::trySetShared"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(val.get()) == false) return false;
    if (seg->setShared(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          ret = this->trySetShared(qualifier, innerParent, val);
          if (ret) break;
        }
      }
    }
    return ret;
  }
}


void QualifierSeeker::setPlain(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (!this->trySetPlain(qualifier, parent, val)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::setPlain"));
  }
}


Bool QualifierSeeker::trySetPlain(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0 && seg->getResultValidator()->validate(val) == false) return false;
    if (seg->setPlain(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (seg->getResultValidator() == 0 || seg->getResultValidator()->validate(innerParent) == true) {
          ret = this->trySetPlain(qualifier, innerParent, val);
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
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::remove"));
  }
}


Bool QualifierSeeker::tryRemove(Char const *qualifier, IdentifiableObject *parent) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  Reference const *seg = &this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getResultValidator() != 0) {
      // Validate the object before removing it.
      IdentifiableObject *obj;
      if (seg->getPlain(this->dataProvider, parent, obj, index)) {
        if (seg->getResultValidator()->validate(obj) == false) return false;
      }
      index = 0;
    }
    if (seg->remove(this->dataProvider, parent, index)) return true;
    else return false;
  } else {
    ++qualifier;
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
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
