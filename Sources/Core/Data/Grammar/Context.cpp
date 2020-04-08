/**
 * @file Core/Data/Grammar/Context.cpp
 * Contains the implementation of class Core::Data::Grammar/Context.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
  if (val == 0 || !val->isA<Reference>()) return val;
  auto ref = static_cast<Reference*>(val);
  if (ref->isValueCacheEnabled() && ref->getCachedValue() != 0) return ref->getCachedValue();
  auto originalRef = ref;
  do {
    auto module = static_cast<Reference*>(val)->findOwner<Module>();
    this->setModule(module);
    if (!static_cast<Reference*>(val)->getValue(this, val)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  } while (val != 0 && val->isA<Reference>());
  if (originalRef->isValueCacheEnabled()) originalRef->setCachedValue(val);
  return val;
}


//==============================================================================
// Term Helper Functions

TiObject* Context::getListTermFilter(ListTerm *term)
{
  auto retVal = this->traceValue(term->getFilter().get());
  if (retVal != 0 && !retVal->isA<List>() && !retVal->isA<TiInt>()) {
    throw EXCEPTION(GenericException, S("Type of list term filter is invalid"));
  }
  return retVal;
}


Word Context::getListTermChildCount(ListTerm *term, TiObject *listFilter) const
{
  if (listFilter == 0) return static_cast<List*>(term->getTerms().get())->getCount();
  else if (listFilter->isA<TiInt>()) return 1;
  else if (listFilter->isA<List>()) return static_cast<List*>(listFilter)->getCount();
  else {
    throw EXCEPTION(InvalidArgumentException, S("listFilter"), S("Must be TiInt or List for static list terms."));
  }
}


Term* Context::getListTermChild(ListTerm *term, Int index, TiObject *listFilter) const
{
  if (listFilter == 0) {
    return term->getTerm(index);
  } else if (listFilter->isA<TiInt>()) {
    return term->getTerm(static_cast<TiInt*>(listFilter)->get());
  } else if (listFilter->isA<List>()) {
    auto index2 = static_cast<List*>(listFilter)->getElement(index);
    if (index2 == 0 || !index2->isA<TiInt>()) {
      throw EXCEPTION(
        InvalidArgumentException, S("listFilter"), S("List must contain Integers for static list terms.")
      );
    }
    return term->getTerm(static_cast<TiInt*>(index2)->get());
  } else {
    throw EXCEPTION(
      InvalidArgumentException, S("listFilter"), S("Must be TiInt or List for static list terms.")
    );
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
    if (!ref->getCachedValue()->isA<SymbolDefinition>()) {
      throw EXCEPTION(GenericException, S("Reference does not point to a symbol definition."));
    }
    return static_cast<SymbolDefinition*>(ref->getCachedValue());
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
  } while (retVal != 0 && retVal->isA<Reference>());
  if (retVal == 0 || !retVal->isA<SymbolDefinition>()) {
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

Map* Context::getSymbolVars(const SymbolDefinition *definition)
{
  TiObject *vars = this->traceValue(definition->getVars().get());
  if (vars != 0 && !vars->isA<Map>()) {
    throw EXCEPTION(GenericException, S("Symbol's vars is of invalid type."));
  }
  return static_cast<Map*>(vars);
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

LexerModule* Context::getAssociatedLexerModule(Module *module)
{
  Reference *lmr = 0;

  // Find the reference to the lexer module of the current module.
  if (module == 0) module = this->getModule();
  if (module != 0) lmr = module->getLexerModuleRef().get();

  // If we can't find a lexer module, we'll grab the root's lexer module.
  if (lmr == 0) lmr = this->getRoot()->getLexerModuleRef().get();

  // Find the module itself.
  if (lmr == 0) return 0;
  auto *lm = this->traceValue(lmr);
  if (lm == 0 || !lm->isA<LexerModule>()) {
    throw EXCEPTION(GenericException, S("The module has an invalid lexer module reference."));
  }
  return static_cast<LexerModule*>(lm);
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
  auto sp = this->traceValue(spr);
  if (sp == 0 || !sp->isA<List>()) {
    throw EXCEPTION(GenericException, S("The module has an invalid error sync pairs reference."));
  }
  return static_cast<List*>(sp);
}


//==============================================================================
// MapContaining Implementation

void Context::setElement(Int index, TiObject *val)
{
  switch(index) {
    case 0: {
      if (val != 0 && !val->isA<Module>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
      }
      this->root = static_cast<Module*>(val);
      break;
    }
    case 1: {
      if (val != 0 && !val->isA<Module>()) {
        throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
      }
      this->module = static_cast<Module*>(val);
      break;
    }
    case 2: {
      if (val != 0 && !val->isA<VariableStack>()) {
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
    if (val != 0 && !val->isA<Module>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
    }
    this->root = static_cast<Module*>(val);
    return 0;
  } else if (name == S("module")) {
    if (val != 0 && !val->isA<Module>()) {
      throw EXCEPTION(InvalidArgumentException, S("val"), S("Must be of type Module."));
    }
    this->module = static_cast<Module*>(val);
    return 1;
  } else if (name == S("stack")) {
    if (val != 0 && !val->isA<VariableStack>()) {
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
