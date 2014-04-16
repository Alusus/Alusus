/**
 * @file Core/Data/RawSeeker.cpp
 * Contains the implementation of class Core::Data::RawSeeker.
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

//============================================================================
// Helper Functions

Bool RawSeeker::getImmediateContainer(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      SharedPtr<Module> &retModule) const
{
  ReferenceSegment *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.')) {
    retParent = parent.get();
    retSeg = seg;
    if (parent->isDerivedFrom<Module>()) {
      retModule = parent.s_cast<Module>();
    } else {
      retModule.reset();
    }
    return true;
  } else {
    ++qualifier;
    SharedPtr<IdentifiableObject> nextParent;
    if (!seg->tryGet(this->dataProvider, parent.get(), nextParent)) return false;
    Bool ret = this->getImmediateContainer(qualifier, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = parent.s_cast<Module>();
    }
    return ret;
  }
}


Bool RawSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      SharedPtr<Module> &retModule) const
{
  ReferenceSegment *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.')) {
    retParent = parent;
    retSeg = seg;
    retModule.reset();
    return true;
  } else {
    ++qualifier;
    SharedPtr<IdentifiableObject> nextParent;
    if (seg->tryGet(this->dataProvider, parent, nextParent)) {
      return this->getImmediateContainer(qualifier, nextParent, retSeg, retParent, retModule);
    } else if (seg->tryGetPlain(this->dataProvider, parent, parent)) {
      return this->getImmediateContainer(qualifier, parent, retSeg, retParent, retModule);
    } else {
      return false;
    }
  }
}


Bool RawSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      Module *&retModule) const
{
  ReferenceSegment *seg = this->parser.parseQualifierSegment(qualifier);
  if (*qualifier != CHR('.')) {
    retParent = parent;
    retSeg = seg;
    if (parent->isDerivedFrom<Module>()) {
      retModule = static_cast<Module*>(parent);
    } else {
      retModule = 0;
    }
    return true;
  } else {
    ++qualifier;
    IdentifiableObject *nextParent;
    if (!seg->tryGetPlain(this->dataProvider, parent, nextParent)) return false;
    Bool ret = this->getImmediateContainer(qualifier, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = static_cast<Module*>(parent);
    }
    return ret;
  }
}


Bool RawSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent) const
{
  ReferenceSegment *seg = this->parser.parseQualifierSegment(qualifier);
  while (*qualifier == CHR('.')) {
    if (!seg->tryGetPlain(this->dataProvider, parent, parent)) return false;
    ++qualifier;
    seg = this->parser.parseQualifierSegment(qualifier);
  }
  retSeg = seg;
  retParent = parent;
  return true;
}


//==============================================================================
// Data Read Functions

const SharedPtr<IdentifiableObject>& RawSeeker::get(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::get"));
  }
  return immSeg->get(this->dataProvider, immParent);
}


Bool RawSeeker::tryGet(const Char *qualifier, IdentifiableObject *parent,
                       SharedPtr<IdentifiableObject> &result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, result);
}


void RawSeeker::get(const Char *qualifier, IdentifiableObject *parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::get"));
  }
  retVal = immSeg->get(this->dataProvider, immParent);
}


void RawSeeker::get(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::get"));
  }
  retVal = immSeg->get(this->dataProvider, immParent);
}


Bool RawSeeker::tryGet(const Char *qualifier, IdentifiableObject *parent,
                       SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, retVal);
}


Bool RawSeeker::tryGet(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                       SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGet(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Plain Data Read Functions

IdentifiableObject* RawSeeker::getPlain(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::get"));
  }
  return immSeg->getPlain(this->dataProvider, immParent);
}


Bool RawSeeker::tryGetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, result);
}


void RawSeeker::getPlain(const Char *qualifier, IdentifiableObject *parent,
                         IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::get"));
  }
  retVal = immSeg->getPlain(this->dataProvider, immParent);
}


Bool RawSeeker::tryGetPlain(const Char *qualifier, IdentifiableObject *parent,
                            IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Data Write Functions

void RawSeeker::set(const Char *qualifier, IdentifiableObject *parent, const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::set"));
  }
  immSeg->set(this->dataProvider, immParent, val);
}


Bool RawSeeker::trySet(const Char *qualifier, IdentifiableObject *parent,
                       const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->trySet(this->dataProvider, immParent, val);
}


void RawSeeker::setPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::set"));
  }
  immSeg->setPlain(this->dataProvider, immParent, val);
}


Bool RawSeeker::trySetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->trySetPlain(this->dataProvider, immParent, val);
}


//==============================================================================
// Data Delete Functions

void RawSeeker::remove(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::RawSeeker::remove"));
  }
  immSeg->remove(this->dataProvider, immParent);
}


Bool RawSeeker::tryRemove(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryRemove(this->dataProvider, immParent);
}

} } // namespace
