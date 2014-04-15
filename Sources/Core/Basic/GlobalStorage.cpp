/**
 * @file Core/Basic/GlobalStorage.cpp
 * Contains the implementation of class Core::Basic::GlobalStorage.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Function

void* GlobalStorage::getObject(const Char *desc)
{
  Int i = this->index.find(Str(desc));
  if (i == -1) {
    return 0;
  } else {
    return this->objects[i].ptr;
  }
}


void GlobalStorage::setObject(const Char *desc, void *object)
{
  Int i = this->index.find(Str(desc));
  if (i == -1) {
    this->objects.push_back(Desc());
    this->objects.back().str = desc;
    this->objects.back().ptr = object;
    this->index.add();
  } else {
    this->objects[i].ptr = object;
  }
}


/// Get the singleton object.
GlobalStorage* GlobalStorage::getSingleton()
{
  // We'll grab this pointer from the shared library to make sure it's shared among all libraries.
  GlobalStorage *globalStorage=reinterpret_cast<GlobalStorage*>(getCoreGlobalStorage());
  if (globalStorage == 0) {
    globalStorage = new GlobalStorage;
    setCoreGlobalStorage(reinterpret_cast<void*>(globalStorage));
  }
  return globalStorage;
}

} } // namespace
