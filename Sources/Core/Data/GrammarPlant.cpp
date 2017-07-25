/**
 * @file Core/Data/GrammarPlant.cpp
 * Contains the implementation of class Core::Data::GrammarPlant.
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

void GrammarPlant::generateConstTokenDefinitions(Container *container)
{
  for (Int i = 0; static_cast<Word>(i) < container->getCount(); ++i) {
    TiObject *obj = container->get(i);
    if (obj == 0) continue;
    SymbolDefinition *def = tio_cast<SymbolDefinition>(obj);
    if (def != 0) {
      TiObject *term = def->getTerm().get();
      if (term->isDerivedFrom<Term>()) {
        this->generateConstTokenDefinitions(static_cast<Term*>(term));
      }
    }
    Container *childContainer = obj->getInterface<Container>();
    if (childContainer != 0) {
      this->generateConstTokenDefinitions(childContainer);
    }
  }
}


void GrammarPlant::generateConstTokenDefinitions(Term *term)
{
  if (term->isDerivedFrom<TokenTerm>()) {
    TokenTerm *tokenTerm = static_cast<TokenTerm*>(term);
    Integer *pid = tokenTerm->getTokenId().tio_cast_get<Integer>();
    if (pid != 0 && pid->get() == this->constTokenId) {
      TiObject *text = tokenTerm->getTokenText().get();
      if (text == 0) {
        throw EXCEPTION(GenericException, STR("Token term has null id and text."));
      }
      this->generateConstTokensForStrings(text);
    }
  } else if (term->isDerivedFrom<MultiplyTerm>()) {
    this->generateConstTokenDefinitions(static_cast<MultiplyTerm*>(term)->getTerm().get());
  } else if (term->isDerivedFrom<ListTerm>()) {
    TiObject *terms = static_cast<ListTerm*>(term)->getTerms().get();
    if (terms->isDerivedFrom<Term>()) {
      this->generateConstTokenDefinitions(static_cast<Term*>(terms));
    } else if (terms->isDerivedFrom<SharedList>()) {
      for (Int i = 0; static_cast<Word>(i) < static_cast<SharedList*>(terms)->getCount(); ++i) {
        TiObject *child = static_cast<SharedList*>(terms)->get(i);
        if (!child->isDerivedFrom<Term>()) {
          throw EXCEPTION(GenericException, STR("ListTerm has a non-Term child."));
        }
        this->generateConstTokenDefinitions(static_cast<Term*>(child));
      }
    }
  }
}


void GrammarPlant::generateConstTokensForStrings(TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isA<String>()) {
    this->addConstToken(static_cast<String*>(obj)->get());
  } else if (obj->isA<SharedMap>()) {
    SharedMap *map = static_cast<SharedMap*>(obj);
    for (Word i = 0; i < map->getCount(); ++i) {
      this->addConstToken(map->getKey(i).c_str());
      this->generateConstTokensForStrings(map->get(i));
    }
  } else {
    Container *container = obj->getInterface<Container>();
    if (container != 0) {
      for (Word i = 0; i < container->getCount(); ++i) {
        this->generateConstTokensForStrings(container->get(i));
      }
    }
  }
}


Word GrammarPlant::addConstToken(Char const *text)
{
  // Generate a name for the token.
  Str key;
  GrammarPlant::generateKey(text, key);
  // Compute full path.
  Str path = STR("root:");
  path += this->constTokenPrefix;
  path += STR(".");
  path += key;
  // If the same constant is already created, skip.
  TiObject *dummyObj;
  if (this->repository.tryGet(path.c_str(), dummyObj) == false) {
    // Create the token definition.
    this->repository.set(path.c_str(), this->createConstTokenDef(text).get());
  }
  return ID_GENERATOR->getId(path.c_str());
}


SharedPtr<SymbolDefinition> GrammarPlant::createConstTokenDef(Char const *text)
{
  return SymbolDefinition::create({
        {SymbolDefElement::TERM, ConstTerm::create(0, text)},
        {SymbolDefElement::FLAGS, Integer::create(SymbolFlags::ROOT_TOKEN)}});
}


void GrammarPlant::generateKey(Char const *text, Str &result)
{
  StrStream stream;
  stream << STR("__");
  while (*text != CHR('\0')) {
    if ((*text >= CHR('a') && *text <= CHR('z')) ||
        (*text >= CHR('A') && *text <= CHR('Z')) ||
        (*text >= CHR('0') && *text <= CHR('9')) ||
        *text == CHR('_')) {
      stream << *text;
    } else {
      stream << CHR('_') << std::hex << static_cast<Int>(static_cast<Byte>(*text));
    }
    ++text;
  }
  result = stream.str();
}

} } // namespace
