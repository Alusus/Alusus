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
  this->repository.setOwningEnabled(true);
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


void GrammarRepository::initializeObject(IdentifiableObject *obj)
{
  if (obj == 0) return;
  Initializable *initializable = obj->getInterface<Initializable>();
  if (initializable != 0) {
    SharedPtr<Module> oldModule = this->getModule();
    Node *node = io_cast<Node>(obj);
    Module *ownerModule = (node == 0 ? 0 : node->findOwner<Module>());
    this->setModule(getSharedPtr(ownerModule));
    initializable->initialize(this);
    this->setModule(oldModule);
  }
}


void GrammarRepository::set(Reference const *ref, SeekerSetLambda handler)
{
  IdentifiableObject *objToInit = 0;
  this->repository.set(ref, [=,&objToInit](Int index, IdentifiableObject *&obj)->RefOp {
    RefOp ret = handler(index, obj);

    if (isPerform(ret)) {
      // We shouldn't initialize until the object is set to the tree, so we will cache the
      // pointer to be initialized later. If we already have an object from a previous
      // iteration then it's safe to initialize it now.
      if (objToInit != 0) this->initializeObject(objToInit);
      objToInit = obj;
    }

    return ret;
  });
  // If we have a cached object we'll initialize it now.
  if (objToInit != 0) this->initializeObject(objToInit);
}


void GrammarRepository::set(Char const *qualifier, SeekerSetLambda handler)
{
  IdentifiableObject *objToInit = 0;
  this->repository.set(qualifier, [=,&objToInit](Int index, IdentifiableObject *&obj)->RefOp {
    RefOp ret = handler(index, obj);

    if (isPerform(ret)) {
      // We shouldn't initialize until the object is set to the tree, so we will cache the
      // pointer to be initialized later. If we already have an object from a previous
      // iteration then it's safe to initialize it now.
      if (objToInit != 0) this->initializeObject(objToInit);
      objToInit = obj;
    }

    return ret;
  });
  // If we have a cached object we'll initialize it now.
  if (objToInit != 0) this->initializeObject(objToInit);
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
