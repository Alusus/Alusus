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

void GrammarPlant::generateConstTokenDefinitions(SharedContainer *container)
{
  for (Int i = 0; static_cast<Word>(i) < container->getCount(); ++i) {
    IdentifiableObject *obj = container->get(i).get();
    if (obj == 0) continue;
    SymbolDefinition *def = io_cast<SymbolDefinition>(obj);
    if (def != 0) {
      IdentifiableObject *term = def->getTerm().get();
      if (term->isDerivedFrom<Term>()) {
        this->generateConstTokenDefinitions(static_cast<Term*>(term));
      }
    }
    SharedContainer *childContainer = obj->getInterface<SharedContainer>();
    if (childContainer != 0) {
      this->generateConstTokenDefinitions(childContainer);
    }
  }
}


void GrammarPlant::generateConstTokenDefinitions(Term *term)
{
  if (term->isDerivedFrom<TokenTerm>()) {
    TokenTerm *tokenTerm = static_cast<TokenTerm*>(term);
    if (tokenTerm->getTokenId() == 0) {
      IdentifiableObject *text = tokenTerm->getTokenText().get();
      if (text == 0) {
        throw GeneralException(STR("Token term has null id and text."),
                               STR("Core::Data::GrammarPlant::generateConstTokenDefinitions"));
      }
      if (text->isDerivedFrom<String>()) {
        Word id = this->addConstToken(static_cast<String*>(text)->get());
        tokenTerm->setTokenId(std::make_shared<Integer>(id));
      }
    }
  } else if (term->isDerivedFrom<MultiplyTerm>()) {
    this->generateConstTokenDefinitions(static_cast<MultiplyTerm*>(term)->getTerm().get());
  } else if (term->isDerivedFrom<ListTerm>()) {
    IdentifiableObject *terms = static_cast<ListTerm*>(term)->getTerms().get();
    if (terms->isDerivedFrom<Term>()) {
      this->generateConstTokenDefinitions(static_cast<Term*>(terms));
    } else if (terms->isDerivedFrom<SharedList>()) {
      for (Int i = 0; static_cast<Word>(i) < static_cast<SharedList*>(terms)->getCount(); ++i) {
        IdentifiableObject *child = static_cast<SharedList*>(terms)->get(i).get();
        if (!child->isDerivedFrom<Term>()) {
          throw GeneralException(STR("ListTerm has a non-Term child."),
                                 STR("Core::Data::GrammarPlant::generateConstTokenDefinitions"));
        }
        this->generateConstTokenDefinitions(static_cast<Term*>(child));
      }
    }
  }
}


void GrammarPlant::generateConstTokensForStrings(IdentifiableObject *obj)
{
  if (obj == 0) return;
  if (obj->isA<String>()) {
    this->addConstToken(static_cast<String*>(obj)->get());
  } else if (obj->isA<SharedMap>()) {
    SharedMap *map = static_cast<SharedMap*>(obj);
    for (Word i = 0; i < map->getCount(); ++i) {
      this->addConstToken(map->getKey(i).c_str());
    }
  } else {
    SharedContainer *container = obj->getInterface<SharedContainer>();
    if (container != 0) {
      for (Word i = 0; i < container->getCount(); ++i) {
        this->generateConstTokensForStrings(container->get(i).get());
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
  // Create the token definition.
  this->repository.setSharedValue(path.c_str(), SymbolDefinition::create({
    {SymbolDefElement::TERM, ConstTerm::create(0, text)},
    {SymbolDefElement::FLAGS, SymbolFlags::ROOT_TOKEN}
  }));
  return ID_GENERATOR->getId(path.c_str());
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
      stream << CHR('_') << static_cast<Int>(*text);
    }
    ++text;
  }
  result = stream.str();
}

} } // namespace
