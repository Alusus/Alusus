/**
 * @file Core/Data/SharedList.cpp
 * Contains the implementation of class Core::Data::SharedList.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdarg.h>

namespace Core { namespace Data
{

//==============================================================================
// Constructors

SharedList::SharedList(const std::initializer_list<SharedPtr<IdentifiableObject>> &args)
{
  this->reserve(args.size());
  for (auto arg : args) this->add(arg);
}


//==============================================================================
// Member Functions

void SharedList::set(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedList::set"),
                                   STR("Index out of range."));
  } else {
    this->list[index] = val;
  }
}


SharedPtr<IdentifiableObject> const& SharedList::getShared(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedList::getShared"),
                                   STR("Index out of range."));
  }
  return this->list[index];
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
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedList::remove"),
                                   STR("Index out of range."));
  }
  this->list.erase(this->list.begin()+index);
}

} } // namespace
