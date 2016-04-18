/**
 * @file Core/Data/SharedList.cpp
 * Contains the implementation of class Core::Data::SharedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
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
// Constructors

SharedList::SharedList(const std::initializer_list<SharedPtr<IdentifiableObject>> &args)
{
  this->reserve(args.size());
  for (auto arg : args) this->add(arg);
}

SharedList::~SharedList()
{
  for (Int i = 0; i < this->list.size(); ++i) {
    DISOWN_SHAREDPTR(this->list[i]);
  }
}


//==============================================================================
// Member Functions

void SharedList::add(const std::initializer_list<SharedPtr<IdentifiableObject>> &objs)
{
  if (this->list.capacity() < this->list.size() + objs.size()) this->list.reserve(this->list.size() + objs.size());
  for (auto obj : objs) {
    this->list.push_back(obj);
    OWN_SHAREDPTR(obj);
  }
}


Int SharedList::add(SharedPtr<IdentifiableObject> const &val)
{
  this->list.push_back(val);
  OWN_SHAREDPTR(val);
  return this->list.size()-1;
}


void SharedList::insert(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (index < 0 || index > this->getCount()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  this->list.insert(this->list.begin()+index, val);
  OWN_SHAREDPTR(val);
}


void SharedList::set(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
  } else {
    UPDATE_OWNED_SHAREDPTR(this->list[index], val);
  }
}


SharedPtr<IdentifiableObject> const& SharedList::getShared(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
  }
  return this->list[index];
}


void SharedList::clear()
{
  for (Int i = 0; i < this->list.size(); ++i) {
    DISOWN_SHAREDPTR(this->list[i]);
  }
  this->list.clear();
}


//==============================================================================
// DataOwner Implementation

void SharedList::unsetIndexes(Int from, Int to)
{
  for (Word i = 0; i < this->getCount(); ++i) {
    IdentifiableObject *obj = this->get(i);
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}


//==============================================================================
// ListContainer Implementation

void SharedList::remove(Int index)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
  }
  DISOWN_SHAREDPTR(this->list[index]);
  this->list.erase(this->list.begin()+index);
}

} } // namespace
