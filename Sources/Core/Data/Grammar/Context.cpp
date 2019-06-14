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

TiObject* Context::traceValue(TiObject *val)
{
  auto ref = ti_cast<Reference>(val);
  if (ref == 0) return val;
  if (ref->isValueCacheEnabled() && ref->getCachedValue() != 0) return ref->getCachedValue();
  auto originalRef = ref;
  do {
    auto module = static_cast<Reference*>(val)->findOwner<Module>();
    this->setModule(module);
    if (!static_cast<Reference*>(val)->getValue(this, val)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  } while (val != 0 && val->isDerivedFrom<Reference>());
  if (originalRef->isValueCacheEnabled()) originalRef->setCachedValue(val);
  return val;
}


//==============================================================================
// Term Helper Functions

TiObject* Context::getListTermData(ListTerm *term)
{
  auto retVal = this->traceValue(term->getData().get());
  if (retVal != 0 && !retVal->isA<List>() && !retVal->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Type of list term data is invalid"));
  }
  return retVal;
}


Word Context::getListTermChildCount(ListTerm *term, TiObject *listData) const
{
  if (term->isStatic()) {
    if (listData == 0) return static_cast<List*>(term->getTerms().get())->getCount();
    else if (listData->isA<TiInt>()) return 1;
    else if (listData->isA<List>()) return static_cast<List*>(listData)->getCount();
    else {
      throw EXCEPTION(InvalidArgumentException, S("listData"), S("Must be TiInt or List for static list terms."));
    }
  } else {
    if (listData == 0) {
      throw EXCEPTION(InvalidArgumentException, S("listData"), S("Must not be null for dynamic list terms."));
    } else if (!listData->isA<List>()) {
      throw EXCEPTION(InvalidArgumentException, S("listData"), S("Must be of type List for dynamic list terms."));
    } else {
      return static_cast<List*>(listData)->getCount();
    }
  }
}


void Context::getListTermChild(
  ListTerm *term, Int index, TiObject *listData, Term *&retTerm, TiObject *&retData
) const
{
  if (term->isStatic()) {
    if (listData == 0) {
      retTerm = term->getTerm(index).get();
      retData = 0;
    } else if (listData->isA<TiInt>()) {
      retTerm = term->getTerm(static_cast<TiInt*>(listData)->get()).get();
      retData = 0;
    } else if (listData->isA<List>()) {
      TiInt *index2 = ti_cast<TiInt>(static_cast<List*>(listData)->getElement(index));
      if (index2 == 0) {
        throw EXCEPTION(
          InvalidArgumentException, S("listData"), S("List must contain Integers for static list terms.")
        );
      }
      retTerm = term->getTerm(index2->get()).get();
      retData = 0;
    } else {
      throw EXCEPTION(
        InvalidArgumentException, S("listData"), S("Must be TiInt or List for static list terms.")
      );
    }
  } else {
    if (listData == 0) {
      throw EXCEPTION(
        InvalidArgumentException, S("listData"), S("Must not be null for dynamic list terms.")
      );
    } else if (!listData->isA<List>()) {
      throw EXCEPTION(
        InvalidArgumentException, S("listData"), S("Must be of type List for dynamic list terms.")
      );
    } else {
      if (term->getTargetRef() == 0) {
        throw EXCEPTION(GenericException, S("Dynamic term doesn't have a target ref."));
      }
      ASSERT(term->getTerms()->isDerivedFrom<Term>());
      retTerm = static_cast<Term*>(term->getTerms().get());
      retData = static_cast<List*>(listData)->getElement(index);
    }
  }
}


void Context::useListTermChild(
  ListTerm *term, Int index, TiObject *listData, Term *&retTerm, TiObject *&retData
) {
  if (retData == 0) {
    throw EXCEPTION(InvalidArgumentException, S("retData"), S("Must not be null."));
  }
  this->getListTermChild(term, index, listData, retTerm, retData);

  if (term->getTargetRef() != 0) {
    if (!term->getTargetRef()->setValue(this, retData)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  }
}


TiInt* Context::getTokenTermId(TokenTerm *term)
{
  TiObject *id = this->traceValue(term->getTokenId().get());
  if (id == 0 || !id->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Token term's ID is invalid."));
  }
  return static_cast<TiInt*>(id);
}


TiObject* Context::getTokenTermText(TokenTerm *term)
{
  TiObject *text = this->traceValue(term->getTokenText().get());
  if (text != 0 && !text->isA<TiStr>() && !text->isA<Map>()) {
    throw EXCEPTION(GenericException, S("Token term's text is of invalid type."));
  }
  return text;
}


CharGroupDefinition* Context::getReferencedCharGroup(Reference const *ref)
{
  TiObject *obj = this->traceValue(const_cast<Reference*>(ref));
  if (obj == 0 || !obj->isA<CharGroupDefinition>()) {
    throw EXCEPTION(GenericException, S("Reference does not point to a char group definition."));
  }
  return static_cast<CharGroupDefinition*>(obj);
}


SymbolDefinition* Context::getReferencedSymbol(Reference const *ref)
{
  TiObject *retVal = const_cast<Reference*>(ref);
  if (ref->isValueCacheEnabled() && ref->getCachedValue() != 0) {
    auto symbolDef = ti_cast<SymbolDefinition>(ref->getCachedValue());
    if (symbolDef == 0) {
      throw EXCEPTION(GenericException, S("Reference does not point to a symbol definition."));
    }
    return symbolDef;
  }
  auto originalRef = ref;
  do {
    auto module = static_cast<Reference*>(retVal)->findOwner<Module>();
    this->setModule(module);
    if (!static_cast<Reference*>(retVal)->getValue(this, retVal)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
    // If the reference points to a grammar module, then the reference wants the module's start (default) definition.
    if (retVal != 0 && retVal->isA<Module>()) {
      retVal = static_cast<Module*>(retVal)->getStartRef().get();
    }
  } while (retVal != 0 && retVal->isDerivedFrom<Reference>());
  if (retVal == 0 || !retVal->isDerivedFrom<SymbolDefinition>()) {
    throw EXCEPTION(GenericException, S("Reference does not point to a symbol definition."));
  }
  if (originalRef->isValueCacheEnabled()) originalRef->setCachedValue(retVal);
  return static_cast<SymbolDefinition*>(retVal);
}


TiInt* Context::getMultiplyTermMax(MultiplyTerm *term)
{
  TiObject *max = this->traceValue(term->getMax().get());
  if (max != 0 && !max->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's max occurances is of invalid type."));
  }
  return static_cast<TiInt*>(max);
}


TiInt* Context::getMultiplyTermMin(MultiplyTerm *term)
{
  TiObject *min = this->traceValue(term->getMin().get());
  if (min != 0 && !min->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's min occurances is of invalid type."));
  }
  return static_cast<TiInt*>(min);
}


TiInt* Context::getMultiplyTermPriority(MultiplyTerm *term)
{
  TiObject *priority = this->traceValue(term->getPriority().get());
  if (priority != 0 && !priority->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Multiply term's priority is of invalid type."));
  }
  return static_cast<TiInt*>(priority);
}


TiInt* Context::getTermFlags(Term *term)
{
  TiObject *flags = this->traceValue(term->getFlags().get());
  if (flags != 0 && !flags->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Term's flags is of invalid type."));
  }
  return static_cast<TiInt*>(flags);
}


//==============================================================================
// Symbol Definition Helper Functions

Term* Context::getSymbolTerm(SymbolDefinition const *definition)
{
  TiObject *retVal = definition->getTerm().get();
  auto ref = ti_cast<Reference>(retVal);
  if (ref != 0) {
    if (ref->isValueCacheEnabled() && ref->getCachedValue() != 0) {
      auto term = ti_cast<Term>(ref->getCachedValue());
      if (term == 0) {
        throw EXCEPTION(
          InvalidArgumentException, S("definition"), S("Symbol's term should be of type Term or Reference to it.")
        );
      }
      return term;
    }
    auto originalRef = ref;
    do {
      auto module = ref->findOwner<Module>();
      this->setModule(module);
      if (!ref->getValue(this, retVal)) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
      // A definition could have a term reference to another definition which means it wants the terms of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal != 0 && retVal->isA<SymbolDefinition>()) {
        retVal = static_cast<SymbolDefinition*>(retVal)->getTerm().get();
      }
      ref = ti_cast<Reference>(retVal);
    } while (ref != 0);
    if (originalRef->isValueCacheEnabled()) originalRef->setCachedValue(retVal);
  }
  if (retVal == 0 || !retVal->isDerivedFrom<Term>()) {
    throw EXCEPTION(
      InvalidArgumentException, S("definition"), S("Symbol's term should be of type Term or Reference to it.")
    );
  }
  return static_cast<Term*>(retVal);
}


Map* Context::getSymbolVars(const SymbolDefinition *definition)
{
  TiObject *retVal = definition->getVars().get();
  auto ref = ti_cast<Reference>(retVal);
  if (ref != 0) {
    if (ref->isValueCacheEnabled() && ref->getCachedValue() != 0) {
      auto map = ti_cast<Map>(ref->getCachedValue());
      if (map == 0) {
        throw EXCEPTION(
          InvalidArgumentException, S("definition"), S("Symbol's vars should be of type Map or Reference to it.")
        );
      }
      return map;
    }
    auto originalRef = ref;
    do {
      auto module = ref->findOwner<Module>();
      this->setModule(module);
      if (!ref->getValue(this, retVal)) {
        throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
      }
      // A definition could have a vars reference to another definition which means it wants the vars of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (retVal != 0 && retVal->isA<SymbolDefinition>()) {
        retVal = static_cast<SymbolDefinition*>(retVal)->getVars().get();
      }
      ref = ti_cast<Reference>(retVal);
    } while (ref != 0);
    if (originalRef->isValueCacheEnabled()) originalRef->setCachedValue(retVal);
  }
  if (retVal != 0 && !retVal->isA<Map>()) {
    throw EXCEPTION(
      InvalidArgumentException, S("definition"), S("Symbol's vars should be of type Map or Reference to it.")
    );
  }
  return static_cast<Map*>(retVal);
}


TiInt* Context::getSymbolPriority(SymbolDefinition const *definition)
{
  TiObject *priority = this->traceValue(definition->getPriority().get());
  if (priority != 0 && !priority->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Symbol's priority is of invalid type."));
  }
  return static_cast<TiInt*>(priority);
}


TiInt* Context::getSymbolFlags(SymbolDefinition const *definition)
{
  TiObject *flags = this->traceValue(definition->getFlags().get());
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
  if (module != 0) lmr = module->getLexerModuleRef().get();

  // If we can't find a lexer module, we'll grab the root's lexer module.
  if (lmr == 0) lmr = this->getRoot()->getLexerModuleRef().get();

  // Find the module itself.
  if (lmr == 0) return 0;
  Module *lm = ti_cast<Module>(this->traceValue(lmr));
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
  if (module != 0) spr = module->getErrorSyncBlockPairsRef().get();

  // If we can't find the sync pairs, we'll grab the root's sync pairs.
  if (spr == 0) spr = this->getRoot()->getErrorSyncBlockPairsRef().get();

  // Find the list itself.
  if (spr == 0) return 0;
  List *sp = ti_cast<List>(this->traceValue(spr));
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
      break;
    }
    case 2: {
      if (val != 0 && !val->isDerivedFrom<VariableStack>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type VariableStack."));
      }
      this->stack = static_cast<VariableStack*>(val);
      break;
    }
    case 3: {
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
    return 1;
  } else if (name == S("stack")) {
    if (val != 0 && !val->isDerivedFrom<VariableStack>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type VariableStack."));
    }
    this->stack = static_cast<VariableStack*>(val);
    return 2;
  } else if (name == S("args")) {
    this->args = val;
    return 3;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."));
  }
}


TypeInfo* Context::getElementNeededType(Int index) const
{
  switch(index) {
    case 0: return Module::getTypeInfo();
    case 1: return Module::getTypeInfo();
    case 2: return VariableStack::getTypeInfo();
    case 3: return TiObject::getTypeInfo();
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
    case 2: return this->stack;
    case 3: return this->args;
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
    case 2: return SBSTR(S("stack"));
    case 3: return SBSTR(S("args"));
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
  } else if (SBSTR(key) == S("stack")) {
    return 2;
  } else if (SBSTR(key) == S("args")) {
    return 3;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Invalid key."), key);
  }
}

} // namespace
