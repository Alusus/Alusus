/**
 * @file Core/Basic/GlobalStorage.cpp
 * Contains the implementation of class Core::Basic::GlobalStorage.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Function

void* GlobalStorage::getObject(Char const *desc)
{
  Int i = this->map.findPos(Str(true, desc));
  if (i == -1) {
    return 0;
  } else {
    return this->map.valAt(i);
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
