/**
 * @file Core/Data/DataStack.cpp
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

void DataStack::pop()
{
  if (this->stack.size() > 0) {
    this->stack.pop_back();
  } else {
    if (this->trunkIndex >= 0) {
      this->trunkIndex--;
    } else {
      // This should never be reached.
      throw GeneralException(STR("Stack is empty."), STR("Core::Data::DataStack::pop"));
    }
  }
}


void DataStack::set(const SharedPtr<IdentifiableObject> &obj, Int index)
{
  if (this->getCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::DataStack::set"));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::set"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::set"),
                                     STR("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      this->trunkStack->set(obj, index);
    } else {
      this->stack[index-(this->trunkIndex+1)] = obj;
    }
  } else {
    this->stack[index] = obj;
  }
}


const SharedPtr<IdentifiableObject>& DataStack::get(Int index) const
{
  if (this->getCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::DataStack::get"));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::get"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::get"),
                                     STR("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      return this->trunkStack->get(index);
    } else {
      return this->stack[index-(this->trunkIndex+1)];
    }
  } else {
    return this->stack[index];
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return this->stack[index];
}


void DataStack::copyFrom(const DataStack *src)
{
  if (src == 0) {
    throw InvalidArgumentException(STR("src"), STR("Core::Data::DataStack::copyFrom"),
                                   STR("Argument cannot be null."));
  }
  this->clear();
  for (Int i = 0; i < src->getCount(); ++i) {
    this->push(src->get(i));
  }
}


Bool DataStack::isShared(Int index) const
{
  if (this->getCount() == 0) {
    throw GeneralException(STR("Stack is empty."), STR("Core::Data::DataStack::isShared"));
  }
  if (index >= 0) {
    if (index >= this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::isShared"),
                                     STR("Index is out of range."), index);
    }
  } else {
    if ((-index) > this->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::DataStack::isShared"),
                                     STR("Index is out of range."), index);
    }
    index = this->getCount() + index;
  }

  if (this->trunkIndex >= 0) {
    ASSERT(this->trunkStack != 0);
    if (index <= this->trunkIndex) {
      // This level is shared with the trunk state.
      return true;
    } else {
      return !this->stack[index-(this->trunkIndex+1)].unique();
    }
  } else {
    return !this->stack[index].unique();
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return !this->stack[index].unique();
}


//==============================================================================
// Data Functions

void DataStack::setBranchingInfo(DataStack *ds, Int ti)
{
  if (ds == 0) ti = -1;
  else if (ti < -1 || ti >= static_cast<Int>(ds->getCount())) {
    throw InvalidArgumentException(STR("ti"), STR("Core::Data::DataStack::setBranchingInfo"),
                                   STR("Must be between -1 and ds->getCount()-1 when ds is not null."));
  }
  this->clear();
  this->trunkStack = ds;
  this->trunkIndex = ti;
}


void DataStack::ownTopLevel()
{
  ASSERT(this->getCount() > 0);
  if (this->stack.size() > 0) return;
  ASSERT(this->trunkStack != 0);
  ASSERT(this->trunkIndex > -1);
  if (static_cast<Int>(this->trunkStack->getCount()) <= this->trunkIndex) {
    throw GeneralException(STR("Trunk stack has been modified."),
                           STR("Core::Data::DataStack::ownTopLevel"));
  }
  auto srcData = this->trunkStack->get(this->trunkIndex);
  this->trunkIndex--;
  this->stack.push_back(srcData);
}

} } // namespace
