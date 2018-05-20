/**
 * @file Core/Data/Module.cpp
 * Contains the implementation of class Core::Data::Module.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

Module::Module(const std::initializer_list<Argument> &args) : definitions(true)
{
  this->contentChangeNotifier.relay(this->definitions.contentChangeNotifier);

  for (auto arg : args) {
    this->add(arg.id, arg.shared);
  }
}


Module::~Module()
{
  for (Int i = 0; i < this->definitions.getCount(); ++i) {
    TiObject *obj = this->definitions.get(i);
    DISOWN_PLAINPTR(obj);
  }
}


//==============================================================================
// Definitions Access Functions

Int Module::add(Char const *key, SharedPtr<TiObject> const &val)
{
  Int ret = this->definitions.add(key, val);
  OWN_SHAREDPTR(val);
  return ret;
}


void Module::insert(Int index, Char const *key, SharedPtr<TiObject> const &val)
{
  this->definitions.insert(index, key, val);
  OWN_SHAREDPTR(val);
}


Int Module::set(Char const *key, SharedPtr<TiObject> const &val, Bool insertIfNew)
{
  Int index = this->definitions.findIndex(key);
  if (index != -1) {
    TiObject *old = this->definitions.get(index);
    DISOWN_PLAINPTR(old);
    this->definitions.set(index, val);
    OWN_SHAREDPTR(val);
  } else {
    index = this->definitions.set(key, val, insertIfNew);
    OWN_SHAREDPTR(val);
  }
  return index;
}


void Module::set(Int index, SharedPtr<TiObject> const &val)
{
  TiObject *old = this->definitions.get(index);
  DISOWN_PLAINPTR(old);
  this->definitions.set(index, val);
  OWN_SHAREDPTR(val);
}


void Module::clear()
{
  for (Int i = 0; i < this->definitions.getCount(); ++i) {
    TiObject *obj = this->definitions.get(i);
    DISOWN_PLAINPTR(obj);
  }
  this->definitions.clear();
}


//==============================================================================
// MapContainer Implementation

void Module::set(Int index, TiObject *val)
{
  TiObject *old = this->definitions.get(index);
  DISOWN_PLAINPTR(old);
  this->definitions.set(index, val);
  OWN_PLAINPTR(val);
}


void Module::remove(Int index)
{
  TiObject *old = this->definitions.get(index);
  DISOWN_PLAINPTR(old);
  this->definitions.remove(index);
}


Int Module::set(Char const *key, TiObject *val)
{
  Int index = this->definitions.findIndex(key);
  if (index != -1) {
    TiObject *old = this->definitions.get(index);
    DISOWN_PLAINPTR(old);
    this->definitions.set(index, val);
    OWN_PLAINPTR(val);
  } else {
    index = this->definitions.set(key, val);
    OWN_PLAINPTR(val);
  }
  return index;
}


void Module::remove(Char const *key)
{
  Int index = this->definitions.findIndex(key);
  if (index != -1) {
    TiObject *old = this->definitions.get(index);
    DISOWN_PLAINPTR(old);
    this->definitions.remove(index);
  } else {
    this->definitions.remove(key);
  }
}

} } // namespace
