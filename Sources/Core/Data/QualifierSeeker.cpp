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

SharedPtr<IdentifiableObject> QualifierSeeker::getShared(Char const *qualifier, IdentifiableObject const *parent) const
{
  SharedPtr<IdentifiableObject> result;
  if (!this->tryGetShared(qualifier, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getShared"));
  }
  return result;
}


Bool QualifierSeeker::tryGetShared(Char const *qualifier, IdentifiableObject const *parent,
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getShared(this->dataProvider, parent, result, index)) return true;
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
        if (this->tryGetShared(qualifier, innerParent, result)) return true;
      }
    }
    return false;
  }
}


void QualifierSeeker::getShared(Char const *qualifier, IdentifiableObject const *parent,
                                SharedModulePairedPtr &retVal) const
{
  if (!this->tryGetShared(qualifier, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getShared"));
  }
}


Bool QualifierSeeker::tryGetShared(Char const *qualifier, IdentifiableObject const *parent,
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getShared(this->dataProvider, parent, retVal.object, index)) {
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
          if (this->tryGetShared(qualifier, innerSharedParent.s_cast_get<SharedModulePairedPtr>()->object, retVal)) {
            if (retVal.module == 0) retVal.module = innerSharedParent.s_cast_get<SharedModulePairedPtr>()->module;
            return true;
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          if (this->tryGetShared(qualifier, innerSharedParent.s_cast_get<PlainModulePairedPtr>()->object, retVal)) {
            return true;
          }
        } else {
          if (this->tryGetShared(qualifier, innerSharedParent, retVal)) return true;
        }
      }
    }
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerPlainParent, index)) {
        if (innerPlainParent->isA<SharedModulePairedPtr>()) {
          if (this->tryGetShared(qualifier, static_cast<SharedModulePairedPtr*>(innerPlainParent)->object, retVal)) {
            if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerPlainParent)->module;
            return true;
          }
        } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
          if (this->tryGetShared(qualifier, static_cast<PlainModulePairedPtr*>(innerPlainParent)->object, retVal)) {
            return true;
          }
        } else {
          if (this->tryGetShared(qualifier, innerPlainParent, retVal)) return true;
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getShared(this->dataProvider, parent.get(), retVal.object, index)) {
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
          ret = this->tryGetShared(qualifier, innerSharedParent.s_cast_get<SharedModulePairedPtr>()->object, retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = innerSharedParent.s_cast_get<SharedModulePairedPtr>()->module;
            break;
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          ret = this->tryGetShared(qualifier, innerSharedParent.s_cast_get<PlainModulePairedPtr>()->object, retVal);
          if (ret) break;
        } else {
          ret = this->tryGetShared(qualifier, innerSharedParent, retVal);
          if (ret) break;
        }
      }
    }
    if (!ret) {
      index = 0;
      while (index != -1) {
        if (seg->getPlain(this->dataProvider, parent.get(), innerPlainParent, index)) {
          if (innerPlainParent->isA<SharedModulePairedPtr>()) {
            ret = this->tryGetShared(qualifier, static_cast<SharedModulePairedPtr*>(innerPlainParent)->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerPlainParent)->module;
              break;
            }
          } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
            ret = this->tryGetShared(qualifier, static_cast<PlainModulePairedPtr*>(innerPlainParent)->object, retVal);
            if (ret) break;
          } else {
            ret = this->tryGetShared(qualifier, innerPlainParent, retVal);
            if (ret) break;
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

IdentifiableObject* QualifierSeeker::getPlain(Char const *qualifier, IdentifiableObject const *parent) const
{
  IdentifiableObject *result;
  if (!this->tryGetPlain(qualifier, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::getPlain"));
  }
  return result;
}


Bool QualifierSeeker::tryGetPlain(Char const *qualifier, IdentifiableObject const *parent, IdentifiableObject *&result) const
{
  if (qualifier == 0) {
    throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::QualifierSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getPlain(this->dataProvider, parent, result, index)) return true;
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
        if (this->tryGetPlain(qualifier, innerParent, result)) return true;
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
    if (seg->getPlain(this->dataProvider, parent, retVal.object, index)) {
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
          ret = this->tryGetPlain(qualifier, static_cast<SharedModulePairedPtr*>(innerParent)->object.get(), retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerParent)->module.get();
            break;
          }
        } else if (innerParent->isA<PlainModulePairedPtr>()) {
          ret = this->tryGetPlain(qualifier, static_cast<PlainModulePairedPtr*>(innerParent)->object, retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = static_cast<PlainModulePairedPtr*>(innerParent)->module;
            break;
          }
        } else {
          ret = this->tryGetPlain(qualifier, innerParent, retVal);
          if (ret) break;
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
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
        ret = this->trySetShared(qualifier, innerParent, val);
        if (ret) break;
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
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
        ret = this->trySetPlain(qualifier, innerParent, val);
        if (ret) break;
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
  Reference *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.') && *qualifier != CHR(':')) {
    Int index = 0;
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
        ret = this->tryRemove(qualifier, innerParent);
        if (ret) break;
      }
    }
    return ret;
  }
}

} } // namespace
