/**
 * @file Core/Data/PlainRepository.cpp
 * Contains the implementation of class Core::Data::Logger.
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

void PlainRepository::popLevel()
{
  if (this->stack.getCount() > 0) {
    this->stack.remove(this->stack.getCount()-1);
  } else {
    if (this->trunkIndex >= 0) {
      this->trunkIndex--;
    } else {
      // This should never be reached.
      throw EXCEPTION(GenericException, STR("Stack is empty."));
    }
  }
}


void PlainRepository::setLevel(IdentifiableObject *obj, Int index)
{
  if (this->getLevelCount() == 0) {
    throw EXCEPTION(GenericException, STR("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
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


void PlainRepository::setLevel(Char const *scope, IdentifiableObject *obj, Int index)
{
  if (this->getLevelCount() == 0) {
    throw EXCEPTION(GenericException, STR("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
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


IdentifiableObject* PlainRepository::getLevelData(Int index) const
{
  if (this->getLevelCount() == 0) {
    throw EXCEPTION(GenericException, STR("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
    }
    index = this->getLevelCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkRepo != 0);
    if (index <= this->trunkIndex) {
      return this->trunkRepo->getLevelData(index);
    } else {
      return this->stack.get(index-(this->trunkIndex+1));
    }
  } else {
    return this->stack.get(index);
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return this->stack.get(index);
}


const SbStr& PlainRepository::getLevelScope(Int index) const
{
  if (this->getLevelCount() == 0) {
    throw EXCEPTION(GenericException, STR("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getLevelCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index is out of range."), index);
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


void PlainRepository::copyFrom(PlainRepository const *src)
{
  if (src == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("src"), STR("Argument cannot be null."));
  }
  this->clear();
  for (Int i = 0; i < src->getLevelCount(); ++i) {
    this->pushLevel(src->getLevelScope(i).c_str(), src->getLevelData(i));
  }
}


//==============================================================================
// Branching Functions

void PlainRepository::setBranchingInfo(PlainRepository *ds, Int ti)
{
  if (ds == 0) ti = -1;
  else if (ti < -1 || ti >= static_cast<Int>(ds->getLevelCount())) {
    throw EXCEPTION(InvalidArgumentException, STR("ti"),
                    STR("Must be between -1 and ds->getLevelCount()-1 when ds is not null."));
  }
  this->clear();
  this->trunkRepo = ds;
  this->trunkIndex = ti;
}


void PlainRepository::ownTopLevel()
{
  ASSERT(this->getLevelCount() > 0);
  if (this->stack.getCount() > 0) return;
  ASSERT(this->trunkRepo != 0);
  ASSERT(this->trunkIndex > -1);
  if (static_cast<Int>(this->trunkRepo->getLevelCount()) <= this->trunkIndex) {
    throw EXCEPTION(GenericException, STR("Trunk stack has been modified."));
  }
  auto srcData = this->trunkRepo->getLevelData(this->trunkIndex);
  Str scope = this->trunkRepo->getLevelScope(this->trunkIndex);
  this->trunkIndex--;
  this->stack.add(scope.c_str(), srcData);
}


//==============================================================================
// Provider Implementation

void PlainRepository::set(Reference const *ref, SeekerSetLambda handler)
{
  IdentifiableObject *objToSet = 0;
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.set(ref, &this->stack,
                              [=,&objToSet](Int index, IdentifiableObject *&obj)->RefOp {
      RefOp ret = handler(index, obj);
      if (this->owningEnabled && isPerform(ret)) {
        // We can't call setTreeIds until the object is set to the tree, so we will cache the
        // pointer to be set later. If we already have an object from a previous iteration then
        // it's safe to set it now.
        if (objToSet != 0) setTreeIds(objToSet);
        objToSet = obj;
      }
      return ret;
    });
    // If we have a cached object we'll set its id now.
    if (objToSet != 0) setTreeIds(objToSet);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      RefOp ret;
      this->referenceSeeker.set(ref, parent,
                                [=,&ret,&objToSet](Int index, IdentifiableObject *&obj)->RefOp {
        ret = handler(index, obj);
        if (this->owningEnabled && isPerform(ret)) {
          // We can't call setTreeIds until the object is set to the tree, so we will cache the
          // pointer to be set later. If we already have an object from a previous iteration then
          // it's safe to set it now.
          if (objToSet != 0) setTreeIds(objToSet);
          objToSet = obj;
        }
        return ret;
      });
      if (!isMove(ret)) break;
    }
    // If we have a cached object we'll set its id now.
    if (objToSet != 0) setTreeIds(objToSet);
  }
}


void PlainRepository::set(Char const *qualifier, SeekerSetLambda handler)
{
  IdentifiableObject *objToSet = 0;
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegmentToTemp(qualifier2);
  // Set the value first.
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    this->qualifierSeeker.set(qualifier, &this->stack,
                              [=,&objToSet](Int index, IdentifiableObject *&obj)->RefOp {
      RefOp ret = handler(index, obj);
      if (this->owningEnabled && isPerform(ret)) {
        // We can't call setTreeIds until the object is set to the tree, so we will cache the
        // pointer to be set later. If we already have an object from a previous iteration then
        // it's safe to set it now.
        if (objToSet != 0) setTreeIds(objToSet);
        objToSet = obj;
      }
      return ret;
    });
    // If we have a cached object we'll set its id now.
    if (objToSet != 0) setTreeIds(objToSet);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      RefOp ret;
      this->qualifierSeeker.set(qualifier, parent,
                                [=,&ret,&objToSet](Int index, IdentifiableObject *&obj)->RefOp {
        ret = handler(index, obj);
        if (this->owningEnabled && isPerform(ret)) {
          // We can't call setTreeIds until the object is set to the tree, so we will cache the
          // pointer to be set later. If we already have an object from a previous iteration then
          // it's safe to set it now.
          if (objToSet != 0) setTreeIds(objToSet);
          objToSet = obj;
        }
        return ret;
      });
      if (!isMove(ret)) break;
    }
    // If we have a cached object we'll set its id now.
    if (objToSet != 0) setTreeIds(objToSet);
  }
}


void PlainRepository::remove(Reference const *ref, SeekerRemoveLambda handler)
{
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.remove(ref, &this->stack, handler);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      RefOp ret;
      this->referenceSeeker.remove(ref, parent,
                                   [=,&ret](Int index, IdentifiableObject *obj)->RefOp {
        ret = handler(index, obj);
        return ret;
      });
      if (!isMove(ret)) break;
    }
  }
}


void PlainRepository::remove(Char const *qualifier, SeekerRemoveLambda handler)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegmentToTemp(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    this->qualifierSeeker.remove(qualifier, &this->stack, handler);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *parent = this->stack.get(i);
      if (parent == 0) continue;
      RefOp ret;
      this->qualifierSeeker.remove(qualifier, parent,
                                   [=,&ret](Int index, IdentifiableObject *obj)->RefOp {
        ret = handler(index, obj);
        return ret;
      });
      if (!isMove(ret)) break;
    }
  }
}


void PlainRepository::forEach(Reference const *ref, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo)
{
  if (ref->isA<ScopeReference>()) {
    this->referenceSeeker.forEach(ref, &this->stack, handler, parentTypeInfo);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *source = this->stack.get(i);
      if (source == 0) continue;
      RefOp ret;
      this->referenceSeeker.forEach(ref, source,
                                    [=,&ret](Int index, IdentifiableObject *obj, IdentifiableObject *parent)->RefOp {
        ret = handler(index, obj, parent);
        return ret;
      }, parentTypeInfo);
      if (!isMove(ret)) break;
    }
  }
}


void PlainRepository::forEach(Char const *qualifier, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo)
{
  Char const *qualifier2 = qualifier;
  ReferenceParser parser;
  Reference const *ref = &parser.parseQualifierSegmentToTemp(qualifier2);
  if (ref->isA<ScopeReference>()) {
    ASSERT(*qualifier2 == CHR(':'));
    this->qualifierSeeker.forEach(qualifier, &this->stack, handler, parentTypeInfo);
  } else {
    // The default is to go downward through the stack.
    for (Int i = this->stack.getCount()-1; i>=0; --i) {
      IdentifiableObject *source = this->stack.get(i);
      if (source == 0) continue;
      RefOp ret;
      this->qualifierSeeker.forEach(qualifier, source,
                                    [=,&ret](Int index, IdentifiableObject *obj, IdentifiableObject *parent)->RefOp {
        ret = handler(index, obj, parent);
        return ret;
      }, parentTypeInfo);
      if (!isMove(ret)) break;
    }
  }
}

} } // namespace
