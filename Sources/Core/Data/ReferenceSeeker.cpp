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

SharedPtr<IdentifiableObject> ReferenceSeeker::getShared(Reference const *seg, IdentifiableObject const *parent) const
{
  SharedPtr<IdentifiableObject> result;
  if (!this->tryGetShared(seg, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::getShared"));
  }
  return result;
}


Bool ReferenceSeeker::tryGetShared(Reference const *seg, IdentifiableObject const *parent, SharedPtr<IdentifiableObject> &result) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->getShared(this->dataProvider, parent, result, index)) return true;
    else return false;
  } else {
    Int index = 0;
    IdentifiableObject *innerParent;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (this->tryGetShared(seg->getNext().get(), innerParent, result)) return true;
      }
    }
    return false;
  }
}


void ReferenceSeeker::getShared(Reference const *seg, IdentifiableObject const *parent,
                                SharedModulePairedPtr &retVal) const
{
  if (!this->tryGetShared(seg, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::getShared"));
  }
}


Bool ReferenceSeeker::tryGetShared(Reference const *seg, IdentifiableObject const *parent,
                                   SharedModulePairedPtr &retVal) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->getShared(this->dataProvider, parent, retVal.object, index)) {
      retVal.module.reset();
      return true;
    } else {
      return false;
    }
  } else {
    Int index = 0;
    SharedPtr<IdentifiableObject> innerSharedParent;
    IdentifiableObject *innerPlainParent;
    while (index != -1) {
      if (seg->getShared(this->dataProvider, parent, innerSharedParent, index)) {
        if (innerSharedParent->isA<SharedModulePairedPtr>()) {
          if (this->tryGetShared(seg->getNext().get(), innerSharedParent.s_cast_get<SharedModulePairedPtr>()->object, retVal)) {
            if (retVal.module == 0) retVal.module = innerSharedParent.s_cast_get<SharedModulePairedPtr>()->module;
            return true;
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          if (this->tryGetShared(seg->getNext().get(), innerSharedParent.s_cast_get<PlainModulePairedPtr>()->object, retVal)) {
            return true;
          }
        } else {
          if (this->tryGetShared(seg->getNext().get(), innerSharedParent, retVal)) return true;
        }
      }
    }
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerPlainParent, index)) {
        if (innerPlainParent->isA<SharedModulePairedPtr>()) {
          if (this->tryGetShared(seg->getNext().get(), static_cast<SharedModulePairedPtr*>(innerPlainParent)->object, retVal)) {
            if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerPlainParent)->module;
            return true;
          }
        } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
          if (this->tryGetShared(seg->getNext().get(), static_cast<PlainModulePairedPtr*>(innerPlainParent)->object, retVal)) {
            return true;
          }
        } else {
          if (this->tryGetShared(seg->getNext().get(), innerPlainParent, retVal)) return true;
        }
      }
    }
    return false;
  }
}


void ReferenceSeeker::getShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                                SharedModulePairedPtr &retVal) const
{
  if (!this->tryGetShared(seg, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::getShared"));
  }
}


Bool ReferenceSeeker::tryGetShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                                   SharedModulePairedPtr &retVal) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGetShared"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
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
    Int index = 0;
    SharedPtr<IdentifiableObject> innerSharedParent;
    IdentifiableObject *innerPlainParent;
    Bool ret = false;
    while (index != -1) {
      if (seg->getShared(this->dataProvider, parent.get(), innerSharedParent, index)) {
        if (innerSharedParent->isA<SharedModulePairedPtr>()) {
          ret = this->tryGetShared(seg->getNext().get(), innerSharedParent.s_cast_get<SharedModulePairedPtr>()->object, retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = innerSharedParent.s_cast_get<SharedModulePairedPtr>()->module;
            break;
          }
        } else if (innerSharedParent->isA<PlainModulePairedPtr>()) {
          ret = this->tryGetShared(seg->getNext().get(), innerSharedParent.s_cast_get<PlainModulePairedPtr>()->object, retVal);
          if (ret) break;
        } else {
          ret = this->tryGetShared(seg->getNext().get(), innerSharedParent, retVal);
          if (ret) break;
        }
      }
    }
    if (!ret) {
      index = 0;
      while (index != -1) {
        if (seg->getPlain(this->dataProvider, parent.get(), innerPlainParent, index)) {
          if (innerPlainParent->isA<SharedModulePairedPtr>()) {
            ret = this->tryGetShared(seg->getNext().get(), static_cast<SharedModulePairedPtr*>(innerPlainParent)->object, retVal);
            if (ret) {
              if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerPlainParent)->module;
              break;
            }
          } else if (innerPlainParent->isA<PlainModulePairedPtr>()) {
            ret = this->tryGetShared(seg->getNext().get(), static_cast<PlainModulePairedPtr*>(innerPlainParent)->object, retVal);
            if (ret) break;
          } else {
            ret = this->tryGetShared(seg->getNext().get(), innerPlainParent, retVal);
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

IdentifiableObject* ReferenceSeeker::getPlain(Reference const *seg, IdentifiableObject const *parent) const
{
  IdentifiableObject *result;
  if (!this->tryGetPlain(seg, parent, result)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::getPlain"));
  }
  return result;
}


Bool ReferenceSeeker::tryGetPlain(Reference const *seg, IdentifiableObject const *parent, IdentifiableObject *&result) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->getPlain(this->dataProvider, parent, result, index)) return true;
    else return false;
  } else {
    Int index = 0;
    IdentifiableObject *innerParent;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<PlainModulePairedPtr>()) {
          innerParent = static_cast<PlainModulePairedPtr*>(innerParent)->object;
        } else if (innerParent->isA<SharedModulePairedPtr>()) {
          innerParent = static_cast<SharedModulePairedPtr*>(innerParent)->object.get();
        }
        if (this->tryGetPlain(seg->getNext().get(), innerParent, result)) return true;
      }
    }
    return false;
  }
}


void ReferenceSeeker::getPlain(Reference const *seg, IdentifiableObject *parent,
                               PlainModulePairedPtr &retVal) const
{
  if (!this->tryGetPlain(seg, parent, retVal)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::getPlain"));
  }
}


Bool ReferenceSeeker::tryGetPlain(Reference const *seg, IdentifiableObject *parent,
                                  PlainModulePairedPtr &retVal) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::tryGetPlain"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
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
    Int index = 0;
    IdentifiableObject *innerParent;
    Bool ret = false;
    index = 0;
    while (index != -1) {
      if (seg->getPlain(this->dataProvider, parent, innerParent, index)) {
        if (innerParent->isA<SharedModulePairedPtr>()) {
          ret = this->tryGetPlain(seg->getNext().get(), static_cast<SharedModulePairedPtr*>(innerParent)->object.get(), retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = static_cast<SharedModulePairedPtr*>(innerParent)->module.get();
            break;
          }
        } else if (innerParent->isA<PlainModulePairedPtr>()) {
          ret = this->tryGetPlain(seg->getNext().get(), static_cast<PlainModulePairedPtr*>(innerParent)->object, retVal);
          if (ret) {
            if (retVal.module == 0) retVal.module = static_cast<PlainModulePairedPtr*>(innerParent)->module;
            break;
          }
        } else {
          ret = this->tryGetPlain(seg->getNext().get(), innerParent, retVal);
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

void ReferenceSeeker::setShared(Reference const *seg, IdentifiableObject *parent,
                                SharedPtr<IdentifiableObject> const &val) const
{
  if (!this->trySetShared(seg, parent, val)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::setShared"));
  }
}


Bool ReferenceSeeker::trySetShared(Reference const *seg, IdentifiableObject *parent,
                                   SharedPtr<IdentifiableObject> const &val) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::trySetShared"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::trySetShared"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->setShared(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
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
        ret = this->trySetShared(seg->getNext().get(), innerParent, val);
        if (ret) break;
      }
    }
    return ret;
  }
}


void ReferenceSeeker::setPlain(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (!this->trySetPlain(seg, parent, val)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::setPlain"));
  }
}


Bool ReferenceSeeker::trySetPlain(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  if (seg == 0) {
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->setPlain(this->dataProvider, parent, val, index)) return true;
    else return false;
  } else {
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
        ret = this->trySetPlain(seg->getNext().get(), innerParent, val);
        if (ret) break;
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
    throw InvalidArgumentException(STR("seg"), STR("Core::Data::ReferenceSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (parent == 0) {
    throw InvalidArgumentException(STR("parent"), STR("Core::Data::ReferenceSeeker::trySetPlain"),
                                   STR("Cannot be null."));
  }
  if (seg->getNext() == 0) {
    Int index = 0;
    if (seg->remove(this->dataProvider, parent, index)) return true;
    else return false;
  } else {
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
        ret = this->tryRemove(seg->getNext().get(), innerParent);
        if (ret) break;
      }
    }
    return ret;
  }
}

} } // namespace
