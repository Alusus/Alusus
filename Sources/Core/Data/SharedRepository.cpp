/**
 * @file Core/Data/SharedRepository.cpp
 * Contains the implementation of class Core::Data::SharedRepository.
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
// Data Functions

void SharedRepository::popLevel()
{
  if (this->stack.getCount() > 0) {
    this->stack.remove(this->stack.getCount()-1);
  } else {
    if (this->trunkIndex >= 0) {
      this->trunkIndex--;
    } else {
      // This should never be reached.
      throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::pop"));
    }
  }
}


void SharedRepository::setLevel(SharedPtr<IdentifiableObject> const &obj, Int index)
{
  if (this->getLevelCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::set"));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::set"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::set"),
                                     STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      this->trunkRepo->setLevel(obj, index);
    } else {
      this->stack.set(index-(this->trunkIndex+1), obj);
    }
  } else {
    this->stack.set(index, obj);
  }
}


void SharedRepository::setLevel(Char const *scope, SharedPtr<IdentifiableObject> const &obj, Int index)
{
  if (this->getLevelCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::set"));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::set"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::set"),
                                     STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      this->trunkRepo->setLevel(scope, obj, index);
    } else {
      this->stack.set(index-(this->trunkIndex+1), scope, obj);
    }
  } else {
    this->stack.set(index, scope, obj);
  }
}


SharedPtr<IdentifiableObject> const& SharedRepository::getLevelData(Int index) const
{
  if (this->getLevelCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::get"));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::get"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::get"),
                                     STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      return this->trunkRepo->getLevelData(index);
    } else {
      return this->stack.getShared(index-(this->trunkIndex+1));
    }
  } else {
    return this->stack.getShared(index);
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return this->stack.getShared(index);
}


const SbStr& SharedRepository::getLevelScope(Int index) const
{
  if (this->getLevelCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::get"));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::get"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::get"),
                                     STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      return this->trunkRepo->getLevelScope(index);
    } else {
      return this->stack.getName(index-(this->trunkIndex+1));
    }
  } else {
    return this->stack.getName(index);
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return this->stack.getName(index);
}


void SharedRepository::copyFrom(SharedRepository const *src)
{
  if (src == 0) {
    throw InvalidArgumentException(STR("src"), STR("Core::Data::SharedRepository::copyFrom"),
                                   STR("Argument cannot be null."));
  }
  this->clear();
  for (Int i = 0; i < src->getLevelCount(); ++i) {
    this->pushLevel(src->getLevelScope(i).c_str(), src->getLevelData(i));
  }
}


Bool SharedRepository::isShared(Int index) const
{
  if (this->getLevelCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::SharedRepository::isShared"));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::isShared"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedRepository::isShared"),
                                     STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      // This level is shared with the trunk state.
      return true;
    } else {
      return !this->stack.getShared(index-(this->trunkIndex+1)).unique();
    }
  } else {
    return !this->stack.getShared(index).unique();
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return !this->stack.getShared(index).unique();
}


//==============================================================================
// Branching Functions

void SharedRepository::setBranchingInfo(SharedRepository *ds, Int ti)
{
  if (ds == 0) ti = -1;
  else if (ti < -1 || ti >= static_cast<Int>(ds->getLevelCount())) {
    throw InvalidArgumentException(STR("ti"), STR("Core::Data::SharedRepository::setBranchingInfo"),
                                   STR("Must be between -1 and ds->getLevelCount()-1 when ds is not null."));
  }
  this->clear();
  this->trunkRepo = ds;
  this->trunkIndex = ti;
}


void SharedRepository::ownTopLevel()
{
  ASSERT(this->getLevelCount() > 0);
  if (this->stack.getCount() > 0) return;
  ASSERT(this->trunkRepo != 0);
  ASSERT(this->trunkIndex > -1);
  if (static_cast<Int>(this->trunkRepo->getLevelCount()) <= this->trunkIndex) {
    throw GeneralException(STR("Trunk stack has been modified."),
                           STR("Core::Data::SharedRepository::ownTopLevel"));
  }
  auto srcData = this->trunkRepo->getLevelData(this->trunkIndex);
  Str scope = this->trunkRepo->getLevelScope(this->trunkIndex);
  this->trunkIndex--;
  this->stack.add(scope.c_str(), srcData);
}


//==============================================================================
// Provider Implementation

void SharedRepository::set(Reference const *ref, IdentifiableObject *val)
{
  // Set the value first.
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.set(ref, &this->stack, val);
  } else {
    Bool ret = false;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      ret = this->referenceSeeker.trySet(ref, parent, val);
      if (ret) break;
    }
    if (!ret) {
      throw GeneralException(STR("Couldn't set value. Reference doesn't point to an existing element."),
                             STR("Core::Data::SharedRepository::set"));
    }
  }

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  if (this->owner) {
    Str qualifierStr;
    if (ref->isA<ScopeReference>()) qualifierStr = ReferenceParser::getQualifier(ref->getNext().get());
    else qualifierStr = ReferenceParser::getQualifier(ref);
    Char const *qualifier = qualifierStr.c_str();
    setTreeIds(val, qualifier);
  }
}


void SharedRepository::set(Char const *qualifier, IdentifiableObject *val)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  // Set the value first.
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    Bool ret = false;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::set"),
                                       STR("Invalid scope value."), qualifier);
      }
      ret = this->qualifierSeeker.trySet(qualifier2, parent, val);
      if (ret) break;
    }
    if (!ret) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::set"),
                                     STR("Qualifier doesn't point to an existing element."), qualifier);
    }
  } else {
    qualifier2 = qualifier;
    Bool ret = false;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      ret = this->qualifierSeeker.trySet(qualifier2, parent, val);
      if (ret) break;
    }
    if (!ret) {
      throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::set"),
                                     STR("Qualifier doesn't point to an existing element."), qualifier);
    }
  }

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  if (this->owner) {
    setTreeIds(val, qualifier2);
  }
}


Bool SharedRepository::trySet(Reference const *ref, IdentifiableObject *val)
{
  // Set the value first.
  if (ref->isA<ScopeReference>()) {
    if (!this->referenceSeeker.trySet(ref, &this->stack, val)) return false;
  } else {
    Bool ret = false;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      ret = this->referenceSeeker.trySet(ref, parent, val);
      if (ret) break;
    }
    if (!ret) return false;
  }

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  if (this->owner) {
    Str qualifierStr;
    if (ref->isA<ScopeReference>()) qualifierStr = ReferenceParser::getQualifier(ref->getNext().get());
    else qualifierStr = ReferenceParser::getQualifier(ref);
    Char const *qualifier = qualifierStr.c_str();
    setTreeIds(val, qualifier);
  }

  return true;
}


Bool SharedRepository::trySet(Char const *qualifier, IdentifiableObject *val)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  // Set the value first.
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    Bool ret = false;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) return false;
      ret = this->qualifierSeeker.trySet(qualifier2, parent, val);
      if (ret) break;
    }
    if (!ret) return false;
  } else {
    qualifier2 = qualifier;
    Bool ret = false;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      ret = this->qualifierSeeker.trySet(qualifier2, parent, val);
      if (ret) break;
    }
    if (!ret) return false;
  }

  // TODO: We can remove this part if/when we start handling IDs in definition signals.
  // Set the id for the new object.
  if (this->owner) {
    setTreeIds(val, qualifier2);
  }

  return true;
}


void SharedRepository::remove(Reference const *ref)
{
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.remove(ref, &this->stack);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryRemove(ref, parent)) return;
    }
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::SharedRepository::remove")
                                   STR("Doesn't refer to an existing element."), ReferenceParser::getQualifier(ref).c_str());
  }
}


void SharedRepository::remove(Char const *qualifier)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::remove"),
                                       STR("Invalid scope value."), qualifier);
      }
      if (this->qualifierSeeker.tryRemove(qualifier2, parent)) return;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryRemove(qualifier2, parent)) return;
    }
  }
  throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::remove"),
                                 STR("Qualifier doesn't refer to an existing element."), qualifier);
}


Bool SharedRepository::tryRemove(Reference const *ref)
{
  if (ref->isA<ScopeReference>()) {
    return this->referenceSeeker.tryRemove(ref, &this->stack);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryRemove(ref, parent)) return true;
    }
    return false;
  }
}


Bool SharedRepository::tryRemove(Char const *qualifier)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) break;
      if (this->qualifierSeeker.tryRemove(qualifier2, parent)) return true;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryRemove(qualifier2, parent)) return true;
    }
  }
  return false;
}


IdentifiableObject* SharedRepository::get(Reference const *ref)
{
  if (ref->isA<ScopeReference>()) {
    return this->referenceSeeker.get(ref, &this->stack);
  } else {
    // The default is to go downward through the stack.
    IdentifiableObject *obj;
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryGet(ref, parent, obj)) return obj;
    }
    throw GeneralException(STR("Couldn't set value. Reference doesn't point to an existing element."),
                           STR("Core::Data::SharedRepository::get"));
  }
}


void SharedRepository::get(Reference const *ref, PlainModulePairedPtr &retVal)
{
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.get(ref, &this->stack, retVal);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryGet(ref, parent, retVal)) return;
    }
    throw GeneralException(STR("Couldn't set value. Reference doesn't point to an existing element."),
                           STR("Core::Data::SharedRepository::get"));
  }
}


IdentifiableObject* SharedRepository::get(Char const *qualifier)
{
  Char const *qualifier2 = qualifier;
  IdentifiableObject *obj;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::get"),
                                       STR("Invalid scope value."), qualifier);
      }
      if (this->qualifierSeeker.tryGet(qualifier2, parent, obj)) return obj;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, obj)) return obj;
    }
  }
  throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::get"),
                                 STR("Qualifier doesn't point to an existing element."), qualifier);
}


void SharedRepository::get(Char const *qualifier, PlainModulePairedPtr &retVal)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) {
        throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::get"),
                                       STR("Invalid scope value."), qualifier);
      }
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return;
    }
  }
  throw InvalidArgumentException(STR("qualifier"), STR("Core::Data::SharedRepository::get"),
                                 STR("Qualifier doesn't point to an existing element."), qualifier);
}


Bool SharedRepository::tryGet(Reference const *ref, IdentifiableObject *&retVal)
{
  if (ref->isA<ScopeReference>()) {
    return this->referenceSeeker.tryGet(ref, &this->stack, retVal);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryGet(ref, parent, retVal)) return true;
    }
    return false;
  }
}


Bool SharedRepository::tryGet(Reference const *ref, PlainModulePairedPtr &retVal)
{
  if (ref->isA<ScopeReference>()) {
    return this->referenceSeeker.tryGet(ref, &this->stack, retVal);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->referenceSeeker.tryGet(ref, parent, retVal)) return true;
    }
    return false;
  }
}


Bool SharedRepository::tryGet(Char const *qualifier, IdentifiableObject *&retVal)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) break;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return true;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return true;
    }
  }
  return false;
}


Bool SharedRepository::tryGet(Char const *qualifier, PlainModulePairedPtr &retVal)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegment(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    ++qualifier2;
    IdentifiableObject *parent;
    Int index = 0;
    while (index != -1) {
      if (!ref->getValue(0, &this->stack, parent, index)) break;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return true;
    }
  } else {
    qualifier2 = qualifier;
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      if (this->qualifierSeeker.tryGet(qualifier2, parent, retVal)) return true;
    }
  }
  return false;
}

} } // namespace
