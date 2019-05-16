/**
 * @file Core/Data/Grammar/Context.cpp
 * Contains the implementation of class Core::Data::Grammar/Context.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Misc Functions

void Context::traceValue(TiObject *val, Module *module, TiObject *&retVal, Module *&retModule)
{
  retVal = val;
  retModule = module;
  if (retVal == 0 || !retVal->isDerivedFrom<Reference>()) return;
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  do {
    if (retModule != 0 && retModule != curModule) {
      this->setModule(static_cast<Module*>(retModule));
      curModule = retModule;
    }
    if (!static_cast<Reference*>(retVal)->getValue(this, retVal, &retModule)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  } while (retVal != 0 && retVal->isDerivedFrom<Reference>());
  if (curModule != oldModule) this->setModule(oldModule);
}


//==============================================================================
// Term Helper Functions

void Context::getListTermData(ListTerm *term, PlainPairedPtr &retVal, Module *module)
{
  Module *retModule;
  this->traceValue(term->getData().get(), module, retVal.object, retModule);
  if (retVal.object != 0 && !retVal.object->isA<List>() && !retVal.object->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Type of list term data is invalid"));
  }
  retVal.parent = retModule;
}


Word Context::getListTermChildCount(ListTerm *term, PlainPairedPtr const &listData) const
{
  if (term->isStatic()) {
    if (listData.object == 0) return static_cast<List*>(term->getTerms().get())->getCount();
    else if (listData.object->isA<TiInt>()) return 1;
    else if (listData.object->isA<List>()) return static_cast<List*>(listData.object)->getCount();
    else {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must be TiInt or List for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<List>()) {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must be of type List for dynamic list terms."));
    } else {
      return static_cast<List*>(listData.object)->getCount();
    }
  }
}


void Context::getListTermChild(
  ListTerm *term, Int index, PlainPairedPtr &listData, Term *&retTerm, PlainPairedPtr &retData
) const
{
  if (term->isStatic()) {
    if (listData.object == 0) {
      retTerm = term->getTerm(index).get();
      retData.reset();
    } else if (listData.object->isA<TiInt>()) {
      retTerm = term->getTerm(static_cast<TiInt*>(listData.object)->get()).get();
      retData.reset();
    } else if (listData.object->isA<List>()) {
      TiInt *index2 = ti_cast<TiInt>(static_cast<List*>(listData.object)->getElement(index));
      if (index2 == 0) {
        throw EXCEPTION(InvalidArgumentException, S("listData"),
                        S("List must contain Integers for static list terms."));
      }
      retTerm = term->getTerm(index2->get()).get();
      retData.reset();
    } else {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must be TiInt or List for static list terms."));
    }
  } else {
    if (listData.object == 0) {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must not be null for dynamic list terms."));
    } else if (!listData.object->isA<List>()) {
      throw EXCEPTION(InvalidArgumentException, S("listData"),
                      S("Must be of type List for dynamic list terms."));
    } else {
      if (term->getTargetRef() == 0) {
        throw EXCEPTION(GenericException, S("Dynamic term doesn't have a target ref."));
      }
      ASSERT(term->getTerms()->isDerivedFrom<Term>());
      retTerm = static_cast<Term*>(term->getTerms().get());
      retData.object = static_cast<List*>(listData.object)->getElement(index);
      retData.parent = listData.parent;
    }
  }
}


void Context::useListTermChild(
  ListTerm *term, Int index, PlainPairedPtr &listData, Term *&retTerm, PlainPairedPtr *retData
) {
  if (retData == 0) {
    throw EXCEPTION(InvalidArgumentException, S("retData"), S("Must not be null."));
  }
  this->getListTermChild(term, index, listData, retTerm, *retData);

  if (term->getTargetRef() != 0) {
    if (retData->parent == 0) {
      if (!term->getTargetRef()->setValue(this, retData->object)) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
    } else {
      if (!term->getTargetRef()->setValue(this, retData)) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
    }
  }
}


TiInt* Context::getTokenTermId(TokenTerm *term, Module *module)
{
  TiObject *id = this->traceValue(term->getTokenId().get(), module);
  if (id == 0 || !id->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Token term's ID is invalid."));
  }
  return static_cast<TiInt*>(id);
}


TiObject* Context::getTokenTermText(TokenTerm *term, Module *module)
{
  TiObject *text = this->traceValue(term->getTokenText().get(), module);
  if (text != 0 && !text->isA<TiStr>() && !text->isA<Map>()) {
    throw EXCEPTION(GenericException, S("Token term's text is of invalid type."));
  }
  return text;
}


void Context::getReferencedCharGroup(Reference const *ref, CharGroupDefinition *&charGroupDef, Module *module)
{
  TiObject *obj = this->traceValue(const_cast<Reference*>(ref), module);
  if (obj == 0 || !obj->isA<CharGroupDefinition>()) {
    throw EXCEPTION(GenericException, S("Reference does not point to a char group definition."));
  }
  charGroupDef = static_cast<CharGroupDefinition*>(obj);
}


void Context::getReferencedSymbol(
  Reference const *ref, Module *&retModule, SymbolDefinition *&retDef, Module *module
) {
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainPairedPtr retVal(const_cast<Reference*>(ref), module);
  do {
    if (retVal.parent != 0 && retVal.parent != curModule) {
      this->setModule(static_cast<Module*>(retVal.parent));
      curModule = static_cast<Module*>(retVal.parent);
    }
    if (!static_cast<Reference*>(retVal.object)->getValue(
      this, retVal.object, reinterpret_cast<Module**>(&retVal.parent)
    )) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
    // If the reference points to a grammar module, then the reference wants the module's start (default) definition.
    if (retVal.object != 0 && retVal.object->isA<Module>()) {
      retVal.parent = retVal.object;
      retVal.object = static_cast<Module*>(retVal.object)->getStartRef().get();
    }
  } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<SymbolDefinition>()) {
    throw EXCEPTION(GenericException, S("Reference does not point to a symbol definition."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  retDef = static_cast<SymbolDefinition*>(retVal.object);
  retModule = static_cast<Module*>(retVal.parent);
}


TiInt* Context::getMultiplyTermMax(MultiplyTerm *term, Module *module)
{
  TiObject *max = this->traceValue(term->getMax().get(), module);
  if (max != 0 && !max->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's max occurances is of invalid type."));
  }
  return static_cast<TiInt*>(max);
}


TiInt* Context::getMultiplyTermMin(MultiplyTerm *term, Module *module)
{
  TiObject *min = this->traceValue(term->getMin().get(), module);
  if (min != 0 && !min->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's min occurances is of invalid type."));
  }
  return static_cast<TiInt*>(min);
}


TiInt* Context::getMultiplyTermPriority(MultiplyTerm *term, Module *module)
{
  TiObject *priority = this->traceValue(term->getPriority().get(), module);
  if (priority != 0 && !priority->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's priority is of invalid type."));
  }
  return static_cast<TiInt*>(priority);
}


TiInt* Context::getTermFlags(Term *term, Module *module)
{
  TiObject *flags = this->traceValue(term->getFlags().get(), module);
  if (flags != 0 && !flags->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Term's flags is of invalid type."));
  }
  return static_cast<TiInt*>(flags);
}


//==============================================================================
// Symbol Definition Helper Functions

Term* Context::getSymbolTerm(SymbolDefinition const *definition, Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainPairedPtr retVal(definition->getTerm().get(), module);
  if (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>()) {
    do {
      if (retVal.parent != 0 && retVal.parent != curModule) {
        this->setModule(static_cast<Module*>(retVal.parent));
        curModule = static_cast<Module*>(retVal.parent);
      }
      if (!static_cast<Reference*>(retVal.object)->getValue(
        this, retVal.object, reinterpret_cast<Module**>(&retVal.parent)
      )) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
      // A definition could have a term reference to another definition which means it wants the terms of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getTerm().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object == 0 || !retVal.object->isDerivedFrom<Term>()) {
    throw EXCEPTION(InvalidArgumentException, S("definition"),
                    S("Symbol's term should be of type Term or Reference to it."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  return static_cast<Term*>(retVal.object);
}


Map* Context::getSymbolVars(const SymbolDefinition *definition, Module *module)
{
  Module *oldModule = this->getModule();
  Module *curModule = oldModule;
  PlainPairedPtr retVal(definition->getVars().get(), module);
  if (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>()) {
    do {
      if (retVal.parent != 0 && retVal.parent != curModule) {
        this->setModule(static_cast<Module*>(retVal.parent));
        curModule = static_cast<Module*>(retVal.parent);
      }
      if (!static_cast<Reference*>(retVal.object)->getValue(
        this, retVal.object, reinterpret_cast<Module**>(&retVal.parent)
      )) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
      // A definition could have a vars reference to another definition which means it wants the vars of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal.object != 0 && retVal.object->isA<SymbolDefinition>()) {
        retVal.object = static_cast<SymbolDefinition*>(retVal.object)->getVars().get();
      }
    } while (retVal.object != 0 && retVal.object->isDerivedFrom<Reference>());
  }
  if (retVal.object != 0 && !retVal.object->isA<Map>()) {
    throw EXCEPTION(InvalidArgumentException, S("definition"),
                    S("Symbol's vars should be of type Map or Reference to it."));
  }
  if (curModule != oldModule) this->setModule(oldModule);
  return static_cast<Map*>(retVal.object);
}


TiInt* Context::getSymbolPriority(SymbolDefinition const *definition, Module *module)
{
  TiObject *priority = this->traceValue(definition->getPriority().get(), module);
  if (priority != 0 && !priority->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Symbol's priority is of invalid type."));
  }
  return static_cast<TiInt*>(priority);
}


TiInt* Context::getSymbolFlags(SymbolDefinition const *definition, Module *module)
{
  TiObject *flags = this->traceValue(definition->getFlags().get(), module);
  if (flags != 0 && !flags->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Symbol's flags is of invalid type."));
  }
  return static_cast<TiInt*>(flags);
}


//==============================================================================
// Other Helper Functions

Module* Context::getAssociatedLexerModule(Module *module)
{
  Reference *lmr = 0;

  // Find the reference to the lexer module of the current module.
  if (module == 0) module = this->getModule();
  Module *grammarModule = ti_cast<Module>(module);
  if (grammarModule != 0) lmr = grammarModule->getLexerModuleRef().get();

  // If we can't find a lexer module, we'll grab the root's lexer module.
  if (lmr == 0) {
    grammarModule = ti_cast<Module>(this->getRoot());
    if (grammarModule != 0) lmr = grammarModule->getLexerModuleRef().get();
  }

  // Find the module itself.
  if (lmr == 0) return 0;
  Module *lm = ti_cast<Module>(this->traceValue(lmr, grammarModule));
  if (lm == 0) {
    throw EXCEPTION(GenericException, S("The module has an invalid lexer module reference."));
  }
  return lm;
}


List* Context::getAssociatedErrorSyncBlockPairs(Module *module)
{
  Reference *spr = 0;

  // Find the reference to the sync pairs of the current module.
  if (module == 0) module = this->getModule();
  Module *grammarModule = ti_cast<Module>(module);
  if (grammarModule != 0) spr = grammarModule->getErrorSyncBlockPairsRef().get();

  // If we can't find the sync pairs, we'll grab the root's sync pairs.
  if (spr == 0) {
    grammarModule = ti_cast<Module>(this->getRoot());
    if (grammarModule != 0) spr = grammarModule->getErrorSyncBlockPairsRef().get();
  }

  // Find the list itself.
  if (spr == 0) return 0;
  List *sp = ti_cast<List>(this->traceValue(spr, grammarModule));
  if (sp == 0) {
    throw EXCEPTION(GenericException, S("The module has an invalid error sync pairs reference."));
  }
  return sp;
}


//==============================================================================
// MapContaining Implementation

void Context::setElement(Int index, TiObject *val)
{
  switch(index) {
    case 0: {
      if (val != 0 && !val->isDerivedFrom<Module>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
      }
      this->root = static_cast<Module*>(val);
      break;
    }
    case 1: {
      if (val != 0 && !val->isDerivedFrom<Module>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
      }
      this->module = static_cast<Module*>(val);
      this->bmodule = this->module->getBaseModule();
      break;
    }
    case 2: {
      throw EXCEPTION(GenericException, S("Element cannot be manually set."));
    }
    case 3: {
      if (val != 0 && !val->isDerivedFrom<VariableStack>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type VariableStack."));
      }
      this->stack = static_cast<VariableStack*>(val);
      break;
    }
    case 4: {
      this->args = val;
      break;
    }
    default: {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."));
    }
  }
}


Int Context::setElement(Char const *key, TiObject *val)
{
  auto name = SBSTR(key);
  if (name == S("root")) {
    if (val != 0 && !val->isDerivedFrom<Module>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
    }
    this->root = static_cast<Module*>(val);
    return 0;
  } else if (name == S("module")) {
    if (val != 0 && !val->isDerivedFrom<Module>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
    }
    this->module = static_cast<Module*>(val);
    this->bmodule = this->module->getBaseModule();
    return 1;
  } else if (name == S("bmodule")) {
    throw EXCEPTION(GenericException, S("Element cannot be manually set."));
  } else if (name == S("stack")) {
    if (val != 0 && !val->isDerivedFrom<VariableStack>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type VariableStack."));
    }
    this->stack = static_cast<VariableStack*>(val);
    return 3;
  } else if (name == S("args")) {
    this->args = val;
    return 4;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."));
  }
}


TypeInfo* Context::getElementNeededType(Int index) const
{
  switch(index) {
    case 0: return Module::getTypeInfo();
    case 1: return Module::getTypeInfo();
    case 2: return Module::getTypeInfo();
    case 3: return VariableStack::getTypeInfo();
    case 4: return TiObject::getTypeInfo();
    default: {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."));
    }
  }
}


TypeInfo* Context::getElementNeededType(Char const *key) const
{
  auto name = SBSTR(key);
  if (name == S("root")) {
    return Module::getTypeInfo();
  } else if (name == S("module")) {
    return Module::getTypeInfo();
  } else if (name == S("bmodule")) {
    return Module::getTypeInfo();
  } else if (name == S("stack")) {
    return VariableStack::getTypeInfo();
  } else if (name == S("args")) {
    return TiObject::getTypeInfo();
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."));
  }
}


TiObject* Context::getElement(Int index) const
{
  switch(index) {
    case 0: return this->root;
    case 1: return this->module;
    case 2: return this->bmodule;
    case 3: return this->stack;
    case 4: return this->args;
    default: {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."));
    }
  }
}


TiObject* Context::getElement(Char const *key) const
{
  auto name = SBSTR(key);
  if (name == S("root")) {
    return this->root;
  } else if (name == S("module")) {
    return this->module;
  } else if (name == S("bmodule")) {
    return this->bmodule;
  } else if (name == S("stack")) {
    return this->stack;
  } else if (name == S("args")) {
    return this->args;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."));
  }
}


SbStr const& Context::getElementKey(Int index) const
{
  switch(index) {
    case 0: return SBSTR(S("root"));
    case 1: return SBSTR(S("module"));
    case 2: return SBSTR(S("bmodule"));
    case 3: return SBSTR(S("stack"));
    case 4: return SBSTR(S("args"));
    default: {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."));
    }
  }
}


Int Context::findElementIndex(Char const *key) const
{
  if (SBSTR(key) == S("root")) {
    return 0;
  } else if (SBSTR(key) == S("module")) {
    return 1;
  } else if (SBSTR(key) == S("bmodule")) {
    return 2;
  } else if (SBSTR(key) == S("stack")) {
    return 3;
  } else if (SBSTR(key) == S("args")) {
    return 4;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."), key);
  }
}

} // namespace
