/**
 * @file Core/Data/List.cpp
 * Contains the implementation of class Core::Data::List.
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

List::List(const std::initializer_list<SharedPtr<IdentifiableObject>> &args)
{
  this->reserve(args.size());
  for (auto arg : args) this->add(arg);
}


//==============================================================================
// DataOwner Implementation

void List::unsetIndexes(Int from, Int to)
{
  for (Word i = 0; i < this->getCount(); ++i) {
    IdentifiableObject *obj = this->get(i).get();
    if (obj != 0) Data::unsetIndexes(obj, from, to);
  }
}


//==============================================================================
// ListSharedContainer Implementation

void List::set(Int index, const SharedPtr<IdentifiableObject> &val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::List::set"),
                                   STR("Index out of range."));
  } else {
    this->list[index] = val;
  }
}


void List::remove(Int index)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::List::remove"),
                                   STR("Index out of range."));
  }
  this->list.erase(this->list.begin()+index);
}


const SharedPtr<IdentifiableObject>& List::get(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::List::get"),
                                   STR("Index out of range."));
  }
  return this->list[index];
}

} } // namespace
