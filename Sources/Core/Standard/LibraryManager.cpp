/**
 * @file Core/Standard/LibraryManager.cpp
 * Contains the implementation of class Standard::LibraryManager.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <dlfcn.h>

namespace Core { namespace Standard
{

void LibraryManager::addLibrary(PtrWord id, LibraryGateway *gateway)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) {
      ASSERT(gateway == this->entries[i].gateway);
      ++this->entries[i].refCount;
      gateway->initializeDuplicate(this->root);
      return;
    }
  }
  this->entries.push_back(Entry(id, gateway));
  gateway->initialize(this->root);
}


void LibraryManager::removeLibrary(PtrWord id)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) {
      if (this->entries[i].refCount == 1) {
        this->entries[i].gateway->uninitialize(this->root);
        this->entries.erase(this->entries.begin() + i);
      } else {
        --this->entries[i].refCount;
        this->entries[i].gateway->uninitializeDuplicate(this->root);
      }
      return;
    }
  }
  throw InvalidArgumentException(STR("id"), STR("Core::Standard::LibraryManager::removeLibrary"),
                                 STR("ID not found among loaded libraries."));
}


PtrWord LibraryManager::findLibrary(const Char *libId)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].gateway->getLibraryId() == libId) return this->entries[i].id;
  }
  return 0;
}


LibraryGateway* LibraryManager::getGateway(PtrWord id)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].id == id) return this->entries[i].gateway;
  }
  throw InvalidArgumentException(STR("id"), STR("Core::Standard::LibraryManager::getGateway"),
                                 STR("ID not found among loaded libraries."));
}


LibraryGateway* LibraryManager::getGateway(const Char *libId)
{
  for (Word i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].gateway->getLibraryId() == libId) return this->entries[i].gateway;
  }
  throw InvalidArgumentException(STR("libId"), STR("Core::Standard::LibraryManager::getGateway"),
                                 STR("ID not found among loaded libraries."), libId);
}


PtrWord LibraryManager::load(const Char *path)
{
  void *handle = dlopen(path, RTLD_NOW);
  if (handle == 0) return 0;

  LibraryGatewayGetter fn = reinterpret_cast<LibraryGatewayGetter>(dlsym(handle, LIBRARY_GATEWAY_GETTER_NAME));
  if (fn == 0) {
    dlclose(handle);
    return 0;
  }

  LibraryGateway *gateway = fn();
  if (gateway == 0) {
    dlclose(handle);
    return 0;
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

} } // namespace
