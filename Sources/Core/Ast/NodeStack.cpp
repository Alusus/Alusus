/**
 * @file Core/Ast/NodeStack.cpp
 * Contains the implementation of class Core::Ast::NodeStack.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Ast
{

//==============================================================================
// Data Functions

void NodeStack::pop()
{
  if (this->stack.size() > 0) {
    this->stack.pop_back();
  } else {
    if (this->trunkIndex >= 0) {
      this->trunkIndex--;
    } else {
      // This should never be reached.
      throw EXCEPTION(GenericException, S("Stack is empty."));
    }
  }
}


void NodeStack::set(SharedPtr<Node> const &obj, Int index)
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
      this->stack[index-(this->trunkIndex+1)] = obj;
    }
  } else {
    this->stack[index] = obj;
  }
}


SharedPtr<Node> const& NodeStack::get(Int index) const
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
      return this->stack[index-(this->trunkIndex+1)];
    }
  } else {
    return this->stack[index];
  }
}


void NodeStack::copyFrom(NodeStack const *src)
{
  if (src == 0) {
    throw EXCEPTION(InvalidArgumentException, S("src"), S("Argument cannot be null."));
  }
  this->clear();
  for (Int i = 0; i < src->getCount(); ++i) {
    this->push(src->get(i));
  }
}


Bool NodeStack::isShared(Int index) const
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
      return this->stack[index-(this->trunkIndex+1)].getRefCounter()->count != 1;
    }
  } else {
    return this->stack[index].getRefCounter()->count != 1;
  }
  // Dummy return statement to avoid compilation error. This won't be reached.
  return false;
}


//==============================================================================
// Branching Functions

void NodeStack::setBranchingInfo(NodeStack *ds, Int ti)
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


void NodeStack::ownTop()
{
  ASSERT(this->getCount() > 0);
  if (this->stack.size() > 0) return;
  ASSERT(this->trunkStack != 0);
  ASSERT(this->trunkIndex > -1);
  if (static_cast<Int>(this->trunkStack->getCount()) <= this->trunkIndex) {
    throw EXCEPTION(GenericException, S("Trunk stack has been modified."));
  }
  auto srcData = this->trunkStack->get(this->trunkIndex);
  this->trunkIndex--;
  this->stack.push_back(srcData);
}


//==============================================================================
// DynamicContaining Implementation

void NodeStack::removeElement(Int index)
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
      this->stack.erase(this->stack.begin() + (index-(this->trunkIndex+1)));
    }
  } else {
    this->stack.erase(this->stack.begin() + index);
  }
}


void NodeStack::insertElement(Int index, Node *val)
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
      this->stack.insert(this->stack.begin() + (index-(this->trunkIndex+1)), getSharedPtr(val));
    }
  } else {
    this->stack.insert(this->stack.begin() + index, getSharedPtr(val));
  }
}

} // namespace
