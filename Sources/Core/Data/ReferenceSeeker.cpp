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
// Helper Functions

Bool ReferenceSeeker::getImmediateContainer(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                                   ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                   SharedPtr<Module> &retModule) const
{
  if (seg->getNext() == 0) {
    retParent = parent.get();
    retSeg = seg;
    if (parent->isDerivedFrom<Module>()) {
      retModule = parent.s_cast<Module>();
    } else {
      retModule.reset();
    }
    return true;
  } else {
    SharedPtr<IdentifiableObject> nextParent;
    if (!seg->tryGetShared(this->dataProvider, parent.get(), nextParent)) return false;
    ReferenceSegment *nextSeg = seg->getNext().get();
    Bool ret = this->getImmediateContainer(nextSeg, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = parent.s_cast<Module>();
    }
    return ret;
  }
}


Bool ReferenceSeeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                   ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                   SharedPtr<Module> &retModule) const
{
  if (seg->getNext() == 0) {
    retParent = parent;
    retSeg = seg;
    retModule.reset();
    return true;
  } else {
    ReferenceSegment *nextSeg = seg->getNext().get();
    SharedPtr<IdentifiableObject> nextParent;
    if (seg->tryGetShared(this->dataProvider, parent, nextParent)) {
      return this->getImmediateContainer(nextSeg, nextParent, retSeg, retParent, retModule);
    } else if (seg->tryGetPlain(this->dataProvider, parent, parent)) {
      return this->getImmediateContainer(nextSeg, parent, retSeg, retParent, retModule);
    } else {
      return false;
    }
  }
}


Bool ReferenceSeeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                   ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                   Module *&retModule) const
{
  if (seg->getNext() == 0) {
    retParent = parent;
    retSeg = seg;
    if (parent->isDerivedFrom<Module>()) {
      retModule = static_cast<Module*>(parent);
    } else {
      retModule = 0;
    }
    return true;
  } else {
    IdentifiableObject *nextParent;
    if (!seg->tryGetPlain(this->dataProvider, parent, nextParent)) return false;
    ReferenceSegment *nextSeg = seg->getNext().get();
    Bool ret = this->getImmediateContainer(nextSeg, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = static_cast<Module*>(parent);
    }
    return ret;
  }
}


Bool ReferenceSeeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
                                   ReferenceSegment *&retSeg, IdentifiableObject *&retParent) const
{
  while (seg->getNext() != 0) {
    if (!seg->tryGetPlain(this->dataProvider, parent, parent)) return false;
    seg = seg->getNext().get();
  }
  retSeg = seg;
  retParent = parent;
  return true;
}


//==============================================================================
// Data Read Functions

const SharedPtr<IdentifiableObject>& ReferenceSeeker::getShared(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  return immSeg->getShared(this->dataProvider, immParent);
}


Bool ReferenceSeeker::tryGetShared(ReferenceSegment *seg, IdentifiableObject *parent, SharedPtr<IdentifiableObject> &result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, result);
}


void ReferenceSeeker::getShared(ReferenceSegment *seg, IdentifiableObject *parent,
                 SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  retVal = immSeg->getShared(this->dataProvider, immParent);
}


void ReferenceSeeker::getShared(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                 SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  retVal = immSeg->getShared(this->dataProvider, immParent);
}


Bool ReferenceSeeker::tryGetShared(ReferenceSegment *seg, IdentifiableObject *parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, retVal);
}


Bool ReferenceSeeker::tryGetShared(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Plain Data Read Functions

IdentifiableObject* ReferenceSeeker::getPlain(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  return immSeg->getPlain(this->dataProvider, immParent);
}


Bool ReferenceSeeker::tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, result);
}


void ReferenceSeeker::getPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                      IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::get"));
  }
  retVal = immSeg->getPlain(this->dataProvider, immParent);
}


Bool ReferenceSeeker::tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                         IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Data Write Functions

void ReferenceSeeker::setShared(ReferenceSegment *seg, IdentifiableObject *parent, const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::set"));
  }
  immSeg->setShared(this->dataProvider, immParent, val);
}


Bool ReferenceSeeker::trySetShared(ReferenceSegment *seg, IdentifiableObject *parent,
                    const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->trySetShared(this->dataProvider, immParent, val);
}


void ReferenceSeeker::setPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::set"));
  }
  immSeg->setPlain(this->dataProvider, immParent, val);
}


Bool ReferenceSeeker::trySetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->trySetPlain(this->dataProvider, immParent, val);
}


//============================================================================
// Data Delete Functions

void ReferenceSeeker::remove(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::ReferenceSeeker::remove"));
  }
  immSeg->remove(this->dataProvider, immParent);
}


Bool ReferenceSeeker::tryRemove(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryRemove(this->dataProvider, immParent);
}

} } // namespace
