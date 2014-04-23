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

//============================================================================
// Helper Functions

Bool QualifierSeeker::getImmediateContainer(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
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
    if (!seg->tryGetShared(this->dataProvider, parent.get(), nextParent)) return false;
    Bool ret = this->getImmediateContainer(qualifier, nextParent, retSeg, retParent, retModule);
    if (ret == true && retModule == 0) {
      if (parent->isDerivedFrom<Module>()) retModule = parent.s_cast<Module>();
    }
    return ret;
  }
}


Bool QualifierSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
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
    if (seg->tryGetShared(this->dataProvider, parent, nextParent)) {
      return this->getImmediateContainer(qualifier, nextParent, retSeg, retParent, retModule);
    } else if (seg->tryGetPlain(this->dataProvider, parent, parent)) {
      return this->getImmediateContainer(qualifier, parent, retSeg, retParent, retModule);
    } else {
      return false;
    }
  }
}


Bool QualifierSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
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


Bool QualifierSeeker::getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
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

const SharedPtr<IdentifiableObject>& QualifierSeeker::getShared(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::get"));
  }
  return immSeg->getShared(this->dataProvider, immParent);
}


Bool QualifierSeeker::tryGetShared(const Char *qualifier, IdentifiableObject *parent,
                       SharedPtr<IdentifiableObject> &result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, result);
}


void QualifierSeeker::getShared(const Char *qualifier, IdentifiableObject *parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::get"));
  }
  retVal = immSeg->getShared(this->dataProvider, immParent);
}


void QualifierSeeker::getShared(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                    SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::get"));
  }
  retVal = immSeg->getShared(this->dataProvider, immParent);
}


Bool QualifierSeeker::tryGetShared(const Char *qualifier, IdentifiableObject *parent,
                       SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, retVal);
}


Bool QualifierSeeker::tryGetShared(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                       SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetShared(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Plain Data Read Functions

IdentifiableObject* QualifierSeeker::getPlain(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::get"));
  }
  return immSeg->getPlain(this->dataProvider, immParent);
}


Bool QualifierSeeker::tryGetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, result);
}


void QualifierSeeker::getPlain(const Char *qualifier, IdentifiableObject *parent,
                         IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::get"));
  }
  retVal = immSeg->getPlain(this->dataProvider, immParent);
}


Bool QualifierSeeker::tryGetPlain(const Char *qualifier, IdentifiableObject *parent,
                            IdentifiableObject *&retVal, Module *&retModule) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent, retModule)) return false;
  return immSeg->tryGetPlain(this->dataProvider, immParent, retVal);
}


//==============================================================================
// Data Write Functions

void QualifierSeeker::setShared(const Char *qualifier, IdentifiableObject *parent, const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::set"));
  }
  immSeg->setShared(this->dataProvider, immParent, val);
}


Bool QualifierSeeker::trySetShared(const Char *qualifier, IdentifiableObject *parent,
                       const SharedPtr<IdentifiableObject> &val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->trySetShared(this->dataProvider, immParent, val);
}


void QualifierSeeker::setPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::set"));
  }
  immSeg->setPlain(this->dataProvider, immParent, val);
}


Bool QualifierSeeker::trySetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->trySetPlain(this->dataProvider, immParent, val);
}


//==============================================================================
// Data Delete Functions

void QualifierSeeker::remove(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) {
    throw GeneralException(STR("Reference pointing to a missing element/tree."),
                           STR("Core::Data::QualifierSeeker::remove"));
  }
  immSeg->remove(this->dataProvider, immParent);
}


Bool QualifierSeeker::tryRemove(const Char *qualifier, IdentifiableObject *parent) const
{
  ReferenceSegment *immSeg;
  IdentifiableObject *immParent;
  if (!this->getImmediateContainer(qualifier, parent, immSeg, immParent)) return false;
  return immSeg->tryRemove(this->dataProvider, immParent);
}

} } // namespace
