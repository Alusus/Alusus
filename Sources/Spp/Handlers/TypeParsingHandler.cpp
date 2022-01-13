/**
 * @file Spp/Handlers/TypeParsingHandler.cpp
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Handlers
{

using namespace Core;
using namespace Core::Data;

void TypeParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto currentList = state->getData().ti_cast_get<Core::Data::Ast::List>();
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(currentList);

  Core::Data::Ast::Identifier *defName = 0;
  SharedPtr<Core::Data::Ast::List> tmpltArgs;
  Core::Data::Ast::Scope *body = 0;

  for (Int i = 1; i < currentList->getElementCount(); ++i) {
    auto obj = currentList->getElement(i);
    if (obj->isDerivedFrom<Core::Data::Ast::Scope>()) {
      body = static_cast<Core::Data::Ast::Scope*>(obj);
    } else if (obj->isDerivedFrom<Core::Data::Ast::Identifier>()) {
      defName = static_cast<Core::Data::Ast::Identifier*>(obj);
    } else if (obj->isDerivedFrom<Core::Data::Ast::Bracket>()) {
      auto bracket = static_cast<Core::Data::Ast::Bracket*>(obj);
      if (bracket->getType() != Core::Data::Ast::BracketType::SQUARE) {
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidTypeElementNotice>(Core::Data::Ast::findSourceLocation(obj))
        );
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
      if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
    } else {
      // Raise an error.
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidTypeElementNotice>(Core::Data::Ast::findSourceLocation(obj))
      );
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  if (body == 0) {
    throw EXCEPTION(GenericException, S("Type body is missing."));
  }

  auto userType = Ast::UserType::create({
    { "prodId", metadata->getProdId()},
    { "sourceLocation", metadata->findSourceLocation() }
  }, {
    { "body", body }
  });

  TioSharedPtr type;
  if (tmpltArgs != 0) {
    type = Ast::Template::create({}, {
      { S("varDefs"), tmpltArgs },
      { S("body"), userType }
    });
  } else {
    type = userType;
  }

  if (defName != 0) {
    state->setData(Core::Data::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), metadata->getProdId() },
      { S("sourceLocation"), metadata->findSourceLocation() }
    }, {
      { S("target"), type }
    }));
  } else {
    state->setData(type);
  }
}


Bool TypeParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Data::Ast::Definition>(data);

  if (definition == 0) return false;

  auto symbolDef = state->refTopProdLevel().getProd();

  // Look for merge modifier.
  auto identifier = modifierData.ti_cast_get<Core::Data::Ast::Identifier>();
  if (identifier != 0) {
    if (symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()) == S("merge")) {
      // Set toMerge in the definition.
      definition->setToMerge(true);
      return true;
    }
  }

  // Add an unknown modifier.
  Core::Data::Ast::translateModifier(symbolDef, modifierData.get());
  definition->addModifier(modifierData);
  return true;
}

} // namespace
