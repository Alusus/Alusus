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

void GrammarContext::traceValue(IdentifiableObject *val, PlainModulePairedPtr &retVal, Module *module)
{
  retVal.object = val;
  retVal.module = module;
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<Reference>()) return;
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  do {
    if (retVal.module != 0 && retVal.module != curModule) {
      this->setModule(retVal.module);
      curModule = retVal.module;
    }
    this->repository.get(static_cast<Reference*>(retVal.object), retVal);
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (curModule != oldModule) this->setModule(oldModule);
}


//==============================================================================
// Term Helper Functions

void GrammarContext::getListTermData(ListTerm *term, PlainModulePairedPtr &retVal, Module *module)
{
  this->traceValue(term->getData().get(), retVal, module);
  if (retVal.object != 0 && !retVal.object->isA<SharedList>() && !retVal.object->isA<Integer>()) {
    throw EXCEPTION(GenericException, STR("Type of list term data is invalid"));
  }
}


Word GrammarContext::getListTermChildCount(ListTerm *term, PlainModulePairedPtr const &listData) const
{
  if (term->isStatic()) {
    if (listData.object == 0) return static_cast<SharedList*>(term->getTerms().get())->getCount();
    else if (listData.object->isA<Integer>()) return 1;
    else if (listData.object->isA<SharedList>()) return static_cast<SharedList*>(listData.object)->getCount();
    else {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
                      STR("Must be Integer or SharedList for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
                      STR("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<SharedList>()) {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
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
      Integer *index2 = io_cast<Integer>(static_cast<SharedList*>(listData.object)->get(index));
      if (index2 == 0) {
        throw EXCEPTION(InvalidArgumentException, STR("listData"),
                        STR("List must contain Integers for static list terms."));
      }
      retTerm = term->getTerm(index2->get()).get();
      retData.reset();
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
                      STR("Must be Integer or SharedList for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
                      STR("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<SharedList>()) {
      throw EXCEPTION(InvalidArgumentException, STR("listData"),
                      STR("Must be of type SharedList for dynamic list terms."));
    } else {
      if (term->getTargetRef() == 0) {
        throw EXCEPTION(GenericException, STR("Dynamic term doesn't have a target ref."));
      }
      ASSERT(term->getTerms()->isDerivedFrom<Term>());
      retTerm = static_cast<Term*>(term->getTerms().get());
      retData.object = static_cast<SharedList*>(listData.object)->get(index);
      retData.module = listData.module;
    }
  }
}


void GrammarContext::useListTermChild(ListTerm *term, Int index, PlainModulePairedPtr &listData,
                                      Term *&retTerm, PlainModulePairedPtr *retData)
{
  if (retData == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("retData"), STR("Must not be null."));
  }
  this->getListTermChild(term, index, listData, retTerm, *retData);

  if (term->getTargetRef() != 0) {
    if (retData->module == 0) this->repository.set(term->getTargetRef().get(), retData->object);
    else this->repository.set(term->getTargetRef().get(), retData);
  }
}


Integer* GrammarContext::getTokenTermId(TokenTerm *term, Module *module)
{
  IdentifiableObject *id = this->traceValue(term->getTokenId().get(), module);
  if (id == 0 || !id->isA<Integer>()) {
    throw EXCEPTION(GenericException, STR("Token term's ID is invalid."));
  }
  return static_cast<Integer*>(id);
}


IdentifiableObject* GrammarContext::getTokenTermText(TokenTerm *term, Module *module)
{
  IdentifiableObject *text = this->traceValue(term->getTokenText().get(), module);
  if (text != 0 && !text->isA<String>() && !text->isA<SharedMap>()) {
    throw EXCEPTION(GenericException, STR("Token term's text is of invalid type."));
  }
  return text;
}


void GrammarContext::getReferencedCharGroup(Reference const *ref, CharGroupDefinition *&charGroupDef, Module *module)
{
  IdentifiableObject *obj = this->traceValue(const_cast<Reference*>(ref), module);
  if (obj == 0 || !obj->isA<CharGroupDefinition>()) {
    throw EXCEPTION(GenericException, STR("Reference does not point to a char group definition."));
  }
  charGroupDef = static_cast<CharGroupDefinition*>(obj);
}


void GrammarContext::getReferencedSymbol(Reference const *ref, Module *&retModule, SymbolDefinition *&retDef,
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
    this->repository.get(static_cast<Reference*>(retVal.object), retVal);
    // If the reference points to a grammar module, then the reference wants the module's start (default) definition.
    if (retVal.object != 0 && retVal.object->isA<GrammarModule>()) {
      retVal.module = static_cast<GrammarModule*>(retVal.object);
      retVal.object = static_cast<GrammarModule*>(retVal.object)->getStartRef().get();
    }
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<SymbolDefinition>()) {
    throw EXCEPTION(GenericException, STR("Reference does not point to a symbol definition."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  retDef = static_cast<SymbolDefinition*>(retVal.object);
  retModule = retVal.module;
}


Integer* GrammarContext::getMultiplyTermMax(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *max = this->traceValue(term->getMaxOccurances().get(), module);
  if (max != 0 && !max->isA<Integer>()) {
    throw EXCEPTION(GenericException, STR("Multiply term's max occurances is of invalid type."));
  }
  return static_cast<Integer*>(max);
}


Integer* GrammarContext::getMultiplyTermMin(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *min = this->traceValue(term->getMinOccurances().get(), module);
  if (min != 0 && !min->isA<Integer>()) {
    throw EXCEPTION(GenericException, STR("Multiply term's min occurances is of invalid type."));
  }
  return static_cast<Integer*>(min);
}


Integer* GrammarContext::getMultiplyTermPriority(MultiplyTerm *term, Module *module)
{
  IdentifiableObject *priority = this->traceValue(term->getPriority().get(), module);
  if (priority != 0 && !priority->isA<Integer>()) {
    throw EXCEPTION(GenericException, STR("Multiply term's priority occurances is of invalid type."));
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
      this->repository.get(static_cast<Reference*>(retVal.object), retVal);
      // A definition could have a term reference to another definition which means it wants the terms of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getTerm().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<Term>()) {
    throw EXCEPTION(InvalidArgumentException, STR("definition"),
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
      this->repository.get(static_cast<Reference*>(retVal.object), retVal);
      // A definition could have a vars reference to another definition which means it wants the vars of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getVars().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object != 0 && !retVal.object->isA<SharedMap>()) {
    throw EXCEPTION(InvalidArgumentException, STR("definition"),
                    STR("Symbol's vars should be of type SharedMap or Reference to it."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  return static_cast<SharedMap*>(retVal.object);
}


//==============================================================================
// Other Helper Functions

Module* GrammarContext::getAssociatedLexerModule(Module *module)
{
  // TODO: Modify this function so that it searches the root module if it can't
  //       find an associated lexer module in the given module.

  if (module == 0) module = this->getModule();
  if (module == 0) module = this->getRoot();
  GrammarModule *grammarModule = io_cast<GrammarModule>(module);

  // Find the reference to the lexer module.
  Reference *lmr = 0;
  while (lmr == 0 && grammarModule != 0) {
    lmr = grammarModule->getLexerModuleRef().get();
    if (lmr == 0) grammarModule = grammarModule->getParent();
  }

  // Find the module itself.
  if (lmr == 0) return 0;
  Module *lm = io_cast<Module>(this->traceValue(lmr, grammarModule));
  if (lm == 0) {
    throw EXCEPTION(GenericException, STR("The module has an invalid lexer module reference."));
  }
  return lm;
}

} } // namespace
