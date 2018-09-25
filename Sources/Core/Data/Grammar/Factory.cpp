/**
 * @file Core/Data/Grammar/Factory.cpp
 * Contains the implementation of class Core::Data::Grammar::Factory.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Member Functions

void Factory::setRootScope(ListContaining<TiObject> *rootScope)
{
  Module *grammarRoot = getGrammarRoot(rootScope, true);
  this->context.setRoot(grammarRoot);
  this->context.setModule(grammarRoot);
}


void Factory::set(Char const* qualifier, TiObject *val)
{
  auto ref = createReference(qualifier, &this->referenceCache);
  if (!ref->setValue(&this->context, val)) {
    throw EXCEPTION(InvalidArgumentException, S("qualifier"), S("Invalid qualifier."), qualifier);
  }
  if (val != 0) setTreeIds(val, this->context.getRoot());
  this->initializeObject(val);
}


void Factory::remove(Char const* qualifier)
{
  auto ref = createReference(qualifier, &this->referenceCache);
  if (!ref->removeValue(&this->context)) {
    throw EXCEPTION(InvalidArgumentException, S("qualifier"), S("Invalid qualifier."), qualifier);
  }
}


Bool Factory::tryRemove(Char const* qualifier)
{
  auto ref = createReference(qualifier, &this->referenceCache);
  return ref->removeValue(&this->context);
}


TiObject* Factory::get(Char const* qualifier)
{
  auto ref = createReference(qualifier, &this->referenceCache);
  TiObject *result = 0;
  if (!ref->getValue(&this->context, result)) {
    throw EXCEPTION(InvalidArgumentException, S("qualifier"), S("Invalid qualifier."), qualifier);
  }
  return result;
}


Bool Factory::tryGet(Char const* qualifier, TiObject *&result, Module **ownerModule)
{
  auto ref = createReference(qualifier, &this->referenceCache);
  return ref->getValue(&this->context, result, ownerModule);
}


void Factory::initializeObject(TiObject *obj)
{
  if (obj == 0) return;
  Initializable *initializable = obj->getInterface<Initializable>();
  if (initializable != 0) {
    auto oldModule = this->context.getModule();
    Node *node = ti_cast<Node>(obj);
    Module *ownerModule = (node == 0 ? 0 : node->findOwner<Module>());
    this->context.setModule(ownerModule);
    initializable->initialize(&this->context);
    this->context.setModule(oldModule);
  }
}


void Factory::generateConstTokenDefinitions(Containing<TiObject> *container)
{
  for (Int i = 0; static_cast<Word>(i) < container->getElementCount(); ++i) {
    TiObject *obj = container->getElement(i);
    if (obj == 0) continue;
    SymbolDefinition *def = ti_cast<SymbolDefinition>(obj);
    if (def != 0) {
      TiObject *term = def->getTerm().get();
      if (term->isDerivedFrom<Term>()) {
        this->generateConstTokenDefinitions(static_cast<Term*>(term));
      }
    } else {
      auto parseDim = ti_cast<ParsingDimension>(obj);
      if (parseDim != 0) {
        Word id = this->addConstToken(parseDim->getEntryTokenText()->get());
        parseDim->setEntryTokenId(TiInt::create(id));
      }
    }
    auto childContainer = obj->getInterface<Containing<TiObject>>();
    if (childContainer != 0) {
      this->generateConstTokenDefinitions(childContainer);
    }
  }
}


void Factory::generateConstTokenDefinitions(Term *term)
{
  if (term->isDerivedFrom<TokenTerm>()) {
    TokenTerm *tokenTerm = static_cast<TokenTerm*>(term);
    TiInt *pid = tokenTerm->getTokenId().ti_cast_get<TiInt>();
    if (pid != 0 && pid->get() == this->constTokenId) {
      TiObject *text = tokenTerm->getTokenText().get();
      if (text == 0) {
        throw EXCEPTION(GenericException, S("Token term has null id and text."));
      }
      this->generateConstTokensForStrings(text);
    }
  } else if (term->isDerivedFrom<MultiplyTerm>()) {
    this->generateConstTokenDefinitions(static_cast<MultiplyTerm*>(term)->getTerm().get());
  } else if (term->isDerivedFrom<ListTerm>()) {
    TiObject *terms = static_cast<ListTerm*>(term)->getTerms().get();
    if (terms->isDerivedFrom<Term>()) {
      this->generateConstTokenDefinitions(static_cast<Term*>(terms));
    } else if (terms->isDerivedFrom<List>()) {
      for (Int i = 0; static_cast<Word>(i) < static_cast<List*>(terms)->getCount(); ++i) {
        TiObject *child = static_cast<List*>(terms)->getElement(i);
        if (!child->isDerivedFrom<Term>()) {
          throw EXCEPTION(GenericException, S("ListTerm has a non-Term child."));
        }
        this->generateConstTokenDefinitions(static_cast<Term*>(child));
      }
    }
  }
}


void Factory::generateConstTokensForStrings(TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isA<TiStr>()) {
    this->addConstToken(static_cast<TiStr*>(obj)->get());
  } else if (obj->isA<Map>()) {
    Map *map = static_cast<Map*>(obj);
    for (Word i = 0; i < map->getCount(); ++i) {
      this->addConstToken(map->getKey(i).c_str());
      this->generateConstTokensForStrings(map->getElement(i));
    }
  } else {
    auto container = obj->getInterface<Containing<TiObject>>();
    if (container != 0) {
      for (Word i = 0; i < container->getElementCount(); ++i) {
        this->generateConstTokensForStrings(container->getElement(i));
      }
    }
  }
}


Word Factory::addConstToken(Char const *text)
{
  // Generate a name for the token.
  Str key;
  Factory::generateKey(text, key);
  // Compute full path.
  Str path = S("root.");
  path += this->constTokenPrefix;
  path += S(".");
  path += key;
  // If the same constant is already created, skip.
  TiObject *dummyObj;
  if (this->tryGet(path.c_str(), dummyObj) == false) {
    // Create the token definition.
    auto constTokenDef = this->createConstTokenDef(text);
    this->set(path.c_str(), constTokenDef.get());
    return constTokenDef->getId();
  } else {
    auto idHolder = ti_cast<IdHaving>(dummyObj);
    return idHolder->getId();
  }
}


SharedPtr<SymbolDefinition> Factory::createConstTokenDef(Char const *text)
{
  return SymbolDefinition::create({
    {S("flags"), TiInt::create(SymbolFlags::ROOT_TOKEN)}
  }, {
    {S("term"), ConstTerm::create({{ S("matchString"), TiStr(text) }})}
  });
}


void Factory::generateKey(Char const *text, Str &result)
{
  StrStream stream;
  stream << S("__");
  while (*text != C('\0')) {
    if ((*text >= C('a') && *text <= C('z')) ||
        (*text >= C('A') && *text <= C('Z')) ||
        (*text >= C('0') && *text <= C('9')) ||
        *text == C('_')) {
      stream << *text;
    } else {
      stream << C('_') << std::hex << static_cast<Int>(static_cast<Byte>(*text));
    }
    ++text;
  }
  result = stream.str();
}

} // namespace
