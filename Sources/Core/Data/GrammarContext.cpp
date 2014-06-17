/**
 * @file Core/Data/GrammarContext.cpp
 * Contains the implementation of class Core::Data::GrammarContext.
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

GrammarContext::GrammarContext() : repository(10, 5)
{
  this->repository.pushLevel(STR("root"), 0);
  this->repository.pushLevel(STR("module"), 0);
  this->repository.pushLevel(STR("pmodule"), 0);
  this->repository.pushLevel(STR("stack"), 0);
  this->repository.pushLevel(STR("args"), 0);
}


//==============================================================================
// Misc Functions

void GrammarContext::tracePlainValue(IdentifiableObject *val, PlainModulePairedPtr &retVal, Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  retVal.object = val;
  retVal.module = module;
  while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>()) {
    if (retVal.module != 0 && retVal.module != curModule) {
      this->setModule(retVal.module);
      curModule = retVal.module;
    }
    this->repository.getPlainValue(static_cast<Reference*>(retVal.object), retVal);
  }
  if (curModule != oldModule) this->setModule(oldModule);
}


//==============================================================================
// Term Helper Functions

void GrammarContext::getListTermData(ListTerm *term, PlainModulePairedPtr &retVal, Module *module)
{
  this->tracePlainValue(term->getData().get(), retVal, module);
  if (retVal.object != 0 && !retVal.object->isA<SharedList>() && !retVal.object->isA<Integer>()) {
    throw GeneralException(STR("Type of list term data is invalid"),
                           STR("Core::Data::GrammarContext::getListTermData"));
  }
}


Word GrammarContext::getListTermChildCount(ListTerm *term, PlainModulePairedPtr const &listData) const
{
  if (term->isStatic()) {
    if (listData.object == 0) return static_cast<SharedList*>(term->getTerms().get())->getCount();
    else if (listData.object->isA<Integer>()) return 1;
    else if (listData.object->isA<SharedList>()) return static_cast<SharedList*>(listData.object)->getCount();
    else {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarContext::getListTermChildCount"),
                                     STR("Must be Integer or SharedList for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarContext::getListTermChildCount"),
                                     STR("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<SharedList>()) {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarContext::getListTermChildCount"),
                                     STR("Must be of type SharedList for dynamic list terms."));
    } else {
      return static_cast<SharedList*>(listData.object)->getCount();
    }
  }
}


void GrammarContext::getListTermChild(ListTerm *term, Int index, PlainModulePairedPtr &listData,
                                      Term *&retTerm, PlainModulePairedPtr &retData) const
{
  if (term->isStatic()) {
    if (listData.object == 0) {
      retTerm = term->getTerm(index).get();
      retData.reset();
    } else if (listData.object->isA<Integer>()) {
      retTerm = term->getTerm(static_cast<Integer*>(listData.object)->get()).get();
      retData.reset();
    } else if (listData.object->isA<SharedList>()) {
      Integer *index2 = io_cast<Integer>(static_cast<SharedList*>(listData.object)->get(index).get());
      if (index2 == 0) {
        throw InvalidArgumentException(STR("listData"),
                                       STR("Core::Data::GrammarContext::getListTermChild"),
                                       STR("List must contain Integers for static list terms."));
      }
      retTerm = term->getTerm(index2->get()).get();
      retData.reset();
    } else {
      throw InvalidArgumentException(STR("listData"), STR("Core::Data::GrammarContext::getListTermChild"),
                                     STR("Must be Integer or SharedList for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw InvalidArgumentException(STR("listData"), STR("Core::Data::GrammarContext::getListTermChild"),
                                     STR("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<SharedList>()) {
      throw InvalidArgumentException(STR("listData"), STR("Core::Data::GrammarContext::getListTermChild"),
                                     STR("Must be of type SharedList for dynamic list terms."));
    } else {
      if (term->getTargetRef() == 0) {
        throw GeneralException(STR("Dynamic term doesn't have a target ref."),
                               STR("Core::Data::GrammarContext::getListTermChild"));
      }
      ASSERT(term->getTerms()->isDerivedFrom<Term>());
      retTerm = static_cast<Term*>(term->getTerms().get());
      retData.object = static_cast<SharedList*>(listData.object)->get(index).get();
      retData.module = listData.module;
    }
  }
}


void GrammarContext::useListTermChild(ListTerm *term, Int index, PlainModulePairedPtr &listData,
                                      Term *&retTerm, PlainModulePairedPtr *retData)
{
  if (retData == 0) {
    throw InvalidArgumentException(STR("retData"), STR("Core::Data::GrammarContext::useListTermChild"),
                                   STR("Must not be null."));
  }
  this->getListTermChild(term, index, listData, retTerm, *retData);

  if (term->getTargetRef() != 0) {
    if (retData->module == 0) this->repository.setPlainValue(term->getTargetRef().get(), retData->object);
    else this->repository.setPlainValue(term->getTargetRef().get(), retData);
  }
}


Integer* GrammarContext::getTokenTermId(TokenTerm *term, Module *module)
{
  IdentifiableObject *id = this->tracePlainValue(term->getTokenId().get(), module);
  if (id == 0 || !id->isA<Integer>()) {
    throw GeneralException(STR("Token term's ID is invalid."),
                           STR("Core::Data::GrammarContext::getTokenTermId"));
  }
  return static_cast<Integer*>(id);
}


IdentifiableObject* GrammarContext::getTokenTermText(TokenTerm *term, Module *module)
{
  IdentifiableObject *text = this->tracePlainValue(term->getTokenText().get(), module);
  if (text != 0 && !text->isA<String>() && !text->isA<SharedMap>()) {
    throw GeneralException(STR("Token term's text is of invalid type."),
                           STR("Core::Data::GrammarContext::getTokenTermText"));
  }
  return text;
}


void GrammarContext::getReferencedDefinition(Reference const *ref, Module *&retModule, SymbolDefinition *&retDef,
                                             Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainModulePairedPtr retVal(const_cast<Reference*>(ref), module);
  do {
    if (retVal.module != 0 && retVal.module != curModule) {
      this->setModule(retVal.module);
      curModule = retVal.module;
    }
    this->repository.getPlainValue(static_cast<Reference*>(retVal.object), retVal);
    // If the reference points to a grammar module, then the reference wants the module's start (default) definition.
    if (retVal.object != 0 && retVal.object->isA<GrammarModule>()) {
      retVal.module = static_cast<GrammarModule*>(retVal.object);
      retVal.object = static_cast<GrammarModule*>(retVal.object)->getStartRef().get();
    }
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<SymbolDefinition>()) {
    throw GeneralException(STR("Reference does not point to a definition."),
                           STR("Core::Data::GrammarContext::getReferencedDefinition"));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  retDef = static_cast<SymbolDefinition*>(retVal.object);
  retModule = retVal.module;
}


Integer* GrammarContext::getMultiplyTermMax(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *max = this->tracePlainValue(term->getMaxOccurances().get(), module);
  if (max != 0 && !max->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's max occurances is of invalid type."),
                           STR("Core::Data::GrammarContext::getMultiplyTermMax"));
  }
  return static_cast<Integer*>(max);
}


Integer* GrammarContext::getMultiplyTermMin(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *min = this->tracePlainValue(term->getMinOccurances().get(), module);
  if (min != 0 && !min->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's min occurances is of invalid type."),
                           STR("Core::Data::GrammarContext::getMultiplyTermMin"));
  }
  return static_cast<Integer*>(min);
}


Integer* GrammarContext::getMultiplyTermPriority(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *priority = this->tracePlainValue(term->getPriority().get(), module);
  if (priority != 0 && !priority->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's priority occurances is of invalid type."),
                           STR("Core::Data::GrammarContext::getMultiplyTermPriority"));
  }
  return static_cast<Integer*>(priority);
}


//==============================================================================
// Symbol Definition Helper Functions

Term* GrammarContext::getSymbolTerm(const SymbolDefinition *definition, Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainModulePairedPtr retVal(definition->getTerm().get(), module);
  if (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>()) {
    do {
      if (retVal.module != 0 && retVal.module != curModule) {
        this->setModule(retVal.module);
        curModule = retVal.module;
      }
      this->repository.getPlainValue(static_cast<Reference*>(retVal.object), retVal);
      // A definition could have a term reference to another definition which means it wants the terms of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getTerm().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<Term>()) {
    throw InvalidArgumentException(STR("definition"), STR("Core::Data::GrammarContext::getSymbolTerm"),
                                   STR("Symbol's term should be of type Term or Reference to it."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  return static_cast<Term*>(retVal.object);
}


SharedMap* GrammarContext::getSymbolVars(const SymbolDefinition *definition, Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainModulePairedPtr retVal(definition->getVars().get(), module);
  if (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>()) {
    do {
      if (retVal.module != 0 && retVal.module != curModule) {
        this->setModule(retVal.module);
        curModule = retVal.module;
      }
      this->repository.getPlainValue(static_cast<Reference*>(retVal.object), retVal);
      // A definition could have a vars reference to another definition which means it wants the vars of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getVars().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object != 0 && !retVal.object->isA<SharedMap>()) {
    throw InvalidArgumentException(STR("definition"), STR("Core::Data::GrammarContext::getSymbolVars"),
                                   STR("Symbol's vars should be of type SharedMap or Reference to it."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  return static_cast<SharedMap*>(retVal.object);
}

} } // namespace
