/**
 * @file Core/Data/Seeker.cpp
 * Contains the implementation of class Core::Data::Seeker.
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

Bool Seeker::getImmediateContainer(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
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
    if (!seg->tryGet(this->dataProvider, parent.get(), nextParent)) return false;
    ReferenceSegment *nextSeg = seg->getNext().get();
    Bool ret = this->getImmediateContainer(nextSeg, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = parent.s_cast<Module>();
    }
    return ret;
  }
}


Bool Seeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
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
    if (seg->tryGet(this->dataProvider, parent, nextParent)) {
      return this->getImmediateContainer(nextSeg, nextParent, retSeg, retParent, retModule);
    } else if (seg->tryGetPlain(this->dataProvider, parent, parent)) {
      return this->getImmediateContainer(nextSeg, parent, retSeg, retParent, retModule);
    } else {
      return false;
    }
  }
}


Bool Seeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
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


Bool Seeker::getImmediateContainer(ReferenceSegment *seg, IdentifiableObject *parent,
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

const SharedPtr<IdentifiableObject>& Seeker::get(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::get"));
  }
  return immSeg->get(this->dataProvider, immParent);
}


Bool Seeker::tryGet(ReferenceSegment *seg, IdentifiableObject *parent, SharedPtr<IdentifiableObject> &result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, result);
}


void Seeker::get(ReferenceSegment *seg, IdentifiableObject *parent,
                 SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::get"));
  }
  retVal = immSeg->get(this->dataProvider, immParent);
}


void Seeker::get(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                 SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::get"));
  }
  retVal = immSeg->get(this->dataProvider, immParent);
}


Bool Seeker::tryGet(ReferenceSegment *seg, IdentifiableObject *parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, retVal);
}


Bool Seeker::tryGet(ReferenceSegment *seg, const SharedPtr<IdentifiableObject> &parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Plain Data Read Functions

IdentifiableObject* Seeker::getPlain(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::get"));
  }
  return immSeg->getPlain(this->dataProvider, immParent);
}


Bool Seeker::tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, result);
}


void Seeker::getPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                      IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::get"));
  }
  retVal = immSeg->getPlain(this->dataProvider, immParent);
}


Bool Seeker::tryGetPlain(ReferenceSegment *seg, IdentifiableObject *parent,
                         IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Data Write Functions

void Seeker::set(ReferenceSegment *seg, IdentifiableObject *parent, const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::set"));
  }
  immSeg->set(this->dataProvider, immParent, val);
}


Bool Seeker::trySet(ReferenceSegment *seg, IdentifiableObject *parent,
                    const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->trySet(this->dataProvider, immParent, val);
}


void Seeker::setPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::set"));
  }
  immSeg->setPlain(this->dataProvider, immParent, val);
}


Bool Seeker::trySetPlain(ReferenceSegment *seg, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->trySetPlain(this->dataProvider, immParent, val);
}


//============================================================================
// Data Delete Functions

void Seeker::remove(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::Seeker::remove"));
  }
  immSeg->remove(this->dataProvider, immParent);
}


Bool Seeker::tryRemove(ReferenceSegment *seg, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(seg, parent, immSeg, immParent)) return false;
  return immSeg->tryRemove(this->dataProvider, immParent);
}

} } // namespace
