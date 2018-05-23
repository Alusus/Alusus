/**
 * @file Core/Main/LibraryManager.cpp
 * Contains the implementation of class Core::Main::LibraryManager.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <dlfcn.h>

namespace Core::Main
{

void LibraryManager::addLibrary(PtrWord id, LibraryGateway *gateway)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) {
      ASSERT(gateway == this->entries[i].gateway);
      ++this->entries[i].refCount;
      if (gateway != 0) gateway->initializeDuplicate(this->root);
      return;
    }
  }
  this->entries.push_back(Entry(id, gateway));
  if (gateway != 0) gateway->initialize(this->root);
}


void LibraryManager::removeLibrary(PtrWord id)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) {
      if (this->entries[i].refCount == 1) {
        if (this->entries[i].gateway != 0) {
          this->entries[i].gateway->uninitialize(this->root);
        }
        this->entries.erase(this->entries.begin() + i);
      } else {
        --this->entries[i].refCount;
        if (this->entries[i].gateway != 0) {
          this->entries[i].gateway->uninitializeDuplicate(this->root);
        }
      }
      return;
    }
  }
  throw EXCEPTION(InvalidArgumentException, STR("id"),
                  STR("ID not found among loaded libraries."));
}


PtrWord LibraryManager::findLibrary(Char const *libId)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].gateway != 0) {
      if (this->entries[i].gateway->getLibraryId() == libId) return this->entries[i].id;
    }
  }
  return 0;
}


LibraryGateway* LibraryManager::getGateway(PtrWord id)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) return this->entries[i].gateway;
  }
  throw EXCEPTION(InvalidArgumentException, STR("id"),
                  STR("ID not found among loaded libraries."));
}


LibraryGateway* LibraryManager::getGateway(Char const *libId)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].gateway != 0) {
      if (this->entries[i].gateway->getLibraryId() == libId) return this->entries[i].gateway;
    }
  }
  throw EXCEPTION(InvalidArgumentException, STR("libId"),
                  STR("ID not found among loaded libraries."), libId);
}


PtrWord LibraryManager::load(Char const *path, Str &error)
{
  Str fullPath = this->root->findAbsolutePath(path);
  if (fullPath.empty()) fullPath = path;

  void *handle = dlopen(fullPath.c_str(), RTLD_NOW|RTLD_GLOBAL);
  if (handle == 0) {
    if (!error.empty()) error += STR("\n");
    error += path;
    error += STR(": ");
    error += dlerror();
    return 0;
  }

  // Get the library gateway if this library supports it, otherwise we'll just load the library.
  LibraryGateway *gateway = 0;
  LibraryGatewayGetter fn = reinterpret_cast<LibraryGatewayGetter>(dlsym(handle, LIBRARY_GATEWAY_GETTER_NAME));
  if (fn != 0) {
    gateway = fn();
    if (gateway == 0) {
      dlclose(handle);
      return 0;
    }
  }

  PtrWord id = reinterpret_cast<PtrWord>(handle);
  this->addLibrary(id, gateway);
  return id;
}


void LibraryManager::unload(PtrWord id)
{
  void *handle = reinterpret_cast<void*>(id);
  this->removeLibrary(id);
  dlclose(handle);
}


void LibraryManager::unloadAll()
{
  while (!this->entries.empty()) {
    this->unload(this->entries.back().id);
  }
}

} // namespace
