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
    auto pmodule = getSharedPtr(module.s_cast_get<GrammarModule>()->getParent());
    this->repository.setLevel(pmodule, GrammarScopeIndex::PMODULE);
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


Bool GrammarRepository::trySet(Reference const *ref, IdentifiableObject *val)
{
  if (!this->repository.trySet(ref, val)) return false;

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    PlainPairedPtr ptr;
    this->repository.get(ref, ptr.object, Module::getTypeInfo(), &ptr.parent);
    this->setModule(getSharedPtr(static_cast<Module*>(ptr.parent)));
    initializable->initialize(this);
    this->setModule(oldModule);
  }

  return true;
}


Bool GrammarRepository::trySet(Char const *qualifier, IdentifiableObject *val)
{
  if (!this->repository.trySet(qualifier, val)) return false;

  Initializable *initializable = val->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    PlainPairedPtr ptr;
    this->repository.get(qualifier, ptr.object, Module::getTypeInfo(), &ptr.parent);
    this->setModule(getSharedPtr(static_cast<Module*>(ptr.parent)));
    initializable->initialize(this);
    this->setModule(oldModule);
  }

  return true;
}


void GrammarRepository::traceValue(IdentifiableObject *val, IdentifiableObject *&retVal, Module *&retModule)
{
  retVal = val;
  retModule = 0;
  if (val == 0 || !val->isDerivedFrom<Reference>()) return;
  SharedPtr<Module> oldModule = this->getModule();
  Module *curModule = oldModule.get();
  do {
    if (retModule != 0 && retModule != curModule) {
      this->setModule(getSharedPtr(retModule));
      curModule = retModule;
    }
    this->repository.get(static_cast<Reference*>(retVal), retVal, retModule);
  } while (retVal != 0 && retVal->isDerivedFrom<Reference>());
  if (curModule != oldModule.get()) this->setModule(oldModule);
}

} } // namespace
