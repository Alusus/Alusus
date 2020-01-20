/**
 * @file Core/Data/DataStack.cpp
 * Contains the implementation of class Core::Data::DataStack.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data
{

//==============================================================================
// Data Functions

void DataStack::pop()
{
  if (this->stack.getCount() > 0) {
    this->stack.remove(this->stack.getCount()-1);
  } else {
    if (this->trunkIndex >= 0) {
      this->trunkIndex--;
    } else {
      // This should never be reached.
      throw EXCEPTION(GenericException, S("Stack is empty."));
    }
  }
}


void DataStack::set(SharedPtr<TiObject> const &obj, Int index)
{
  if (this->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      this->trunkStack->set(obj, index);
    } else {
      this->stack.set(index-(this->trunkIndex+1), obj);
    }
  } else {
    this->stack.set(index, obj);
  }
}


SharedPtr<TiObject> const& DataStack::get(Int index) const
{
  if (this->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      return this->trunkStack->get(index);
    } else {
      return this->stack.get(index-(this->trunkIndex+1));
    }
  } else {
    return this->stack.get(index);
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return this->stack.get(index);
}


void DataStack::copyFrom(DataStack const *src)
{
  if (src == 0) {
    throw EXCEPTION(InvalidArgumentException, S("src"), S("Argument cannot be null."));
  }
  this->clear();
  for (Int i = 0; i < src->getCount(); ++i) {
    this->push(src->get(i));
  }
}


Bool DataStack::isShared(Int index) const
{
  if (this->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      // This level is shared with the trunk state.
      return true;
    } else {
      return !this->stack.get(index-(this->trunkIndex+1)).unique();
    }
  } else {
    return !this->stack.get(index).unique();
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return false;
}


//==============================================================================
// Branching Functions

void DataStack::setBranchingInfo(DataStack *ds, Int ti)
{
  if (ds == 0) ti = -1;
  else if (ti < -1 || ti >= static_cast<Int>(ds->getCount())) {
    throw EXCEPTION(InvalidArgumentException, S("ti"),
                    S("Must be between -1 and ds->getCount()-1 when ds is not null."), ti);
  }
  this->clear();
  this->trunkStack = ds;
  this->trunkIndex = ti;
}


void DataStack::ownTop()
{
  ASSERT(this->getCount() > 0);
  if (this->stack.getCount() > 0) return;
  ASSERT(this->trunkStack != 0);
  ASSERT(this->trunkIndex > -1);
  if (static_cast<Int>(this->trunkStack->getCount()) <= this->trunkIndex) {
    throw EXCEPTION(GenericException, S("Trunk stack has been modified."));
  }
  auto srcData = this->trunkStack->get(this->trunkIndex);
  this->trunkIndex--;
  this->stack.add(srcData);
}


//==============================================================================
// DynamicContaining Implementation

void DataStack::removeElement(Int index)
{
  if (this->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Stack is empty."));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      // This level is shared with the trunk state.
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index refers to a level from a trunk state."));
    } else {
      this->stack.remove(index-(this->trunkIndex+1));
    }
  } else {
    this->stack.remove(index);
  }
}


void DataStack::insertElement(Int index, TiObject *val)
{
  if (this->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Stack is empty."));
  }
  if (index >= 0) {
    if (index > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      // This level is shared with the trunk state.
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index is within the range of the trunk state."));
    } else {
      this->stack.insertElement(index-(this->trunkIndex+1), val);
    }
  } else {
    this->stack.insertElement(index, val);
  }
}

} // namespace
