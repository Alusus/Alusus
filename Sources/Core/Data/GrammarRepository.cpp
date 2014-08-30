/**
 * @file Core/Data/GrammarRepository.cpp
 * Contains the implementation of class Core::Data::GrammarRepository.
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
// Constructor

GrammarRepository::GrammarRepository() : repository(10, 2)
{
  this->repository.setOwner(true);
  this->repository.pushLevel(STR("root"), std::make_shared<GrammarModule>());
  this->repository.pushLevel(STR("module"), SharedPtr<IdentifiableObject>());
  this->repository.pushLevel(STR("pmodule"), SharedPtr<IdentifiableObject>());
}


//==============================================================================
// Member Functions

void GrammarRepository::setModule(SharedPtr<Module> const &module)
{
  this->repository.setLevel(module, GrammarScopeIndex::MODULE);
  if (module != 0 && module->isA<Data::GrammarModule>()) {
    this->repository.setLevel(module.s_cast_get<GrammarModule>()->getParent(), GrammarScopeIndex::PMODULE);
  } else {
    this->repository.setLevel(SharedPtr<IdentifiableObject>(), GrammarScopeIndex::PMODULE);
  }
}


void GrammarRepository::clear()
{
  this->repository.setLevel(std::make_shared<GrammarModule>(), GrammarScopeIndex::ROOT);
  this->repository.setLevel(SharedPtr<IdentifiableObject>(), GrammarScopeIndex::MODULE);
  this->repository.setLevel(SharedPtr<IdentifiableObject>(), GrammarScopeIndex::PMODULE);
}


void GrammarRepository::setSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val)
{
  this->repository.setSharedValue(ref, val);

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    SharedModulePairedPtr ptr;
    this->repository.getSharedValue(ref, ptr);
    this->setModule(ptr.module);
    initializable->initialize(this);
    this->setModule(oldModule);
  }
}


void GrammarRepository::setSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val)
{
  this->repository.setSharedValue(qualifier, val);

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    SharedModulePairedPtr ptr;
    this->repository.getSharedValue(qualifier, ptr);
    this->setModule(ptr.module);
    initializable->initialize(this);
    this->setModule(oldModule);
  }
}


Bool GrammarRepository::trySetSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val)
{
  if (!this->repository.trySetSharedValue(ref, val)) return false;

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    SharedModulePairedPtr ptr;
    this->repository.getSharedValue(ref, ptr);
    this->setModule(ptr.module);
    initializable->initialize(this);
    this->setModule(oldModule);
  }

  return true;
}


Bool GrammarRepository::trySetSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val)
{
  if (!this->repository.trySetSharedValue(qualifier, val)) return false;

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    SharedModulePairedPtr ptr;
    this->repository.getSharedValue(qualifier, ptr);
    this->setModule(ptr.module);
    initializable->initialize(this);
    this->setModule(oldModule);
  }

  return true;
}


IdentifiableObject* GrammarRepository::tracePlainValue(IdentifiableObject *val)
{
  PlainModulePairedPtr ptr;
  this->tracePlainValue(val, ptr);
  return ptr.object;
}


void GrammarRepository::tracePlainValue(IdentifiableObject *val, PlainModulePairedPtr &retVal)
{
  retVal.object = val;
  if (val == 0 || !val->isDerivedFrom<Reference>()) {
    retVal.module = 0;
    return;
  }
  SharedPtr<Module> oldModule = this->getModule();
  Module *curModule = oldModule.get();
  SharedModulePairedPtr sharedRetVal;
  do {
    if (sharedRetVal.module != 0 && sharedRetVal.module.get() != curModule) {
      this->setModule(sharedRetVal.module);
      curModule = sharedRetVal.module.get();
    }
    this->repository.getSharedValue(static_cast<Reference*>(retVal.object), sharedRetVal);
    retVal.object = sharedRetVal.object.get();
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (curModule != oldModule.get()) this->setModule(oldModule);
  retVal.module = sharedRetVal.module.get();
}


SharedPtr<IdentifiableObject> GrammarRepository::traceSharedValue(const SharedPtr<IdentifiableObject> &val)
{
  SharedModulePairedPtr ptr;
  this->traceSharedValue(val, ptr);
  return ptr.object;
}


void GrammarRepository::traceSharedValue(const SharedPtr<IdentifiableObject> &val, SharedModulePairedPtr &retVal)
{
  retVal.object = val;
  retVal.module = SharedPtr<Module>();
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<Reference>()) return;
  SharedPtr<Module> oldModule = this->getModule();
  Module *curModule = oldModule.get();
  do {
    if (retVal.module != 0 && retVal.module != curModule) {
      this->setModule(retVal.module);
      curModule = retVal.module.get();
    }
    this->repository.getSharedValue(retVal.object.s_cast_get<Reference>(), retVal);
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (curModule != oldModule.get()) this->setModule(oldModule);
}

} } // namespace
