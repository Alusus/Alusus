/**
 * @file Spp/Handlers/TypeParsingHandler.cpp
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

void TypeParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto currentList = state->getData().ti_cast_get<Core::Ast::List>();

  Core::Ast::Identifier *defName = 0;
  SharedPtr<Core::Ast::List> tmpltArgs;
  Core::Ast::Scope *body = 0;

  for (Int i = 1; i < currentList->getElementCount(); ++i) {
    auto obj = currentList->getElement(i);
    if (obj->isDerivedFrom<Core::Ast::Scope>()) {
      body = static_cast<Core::Ast::Scope*>(obj);
    } else if (obj->isDerivedFrom<Core::Ast::Identifier>()) {
      defName = static_cast<Core::Ast::Identifier*>(obj);
    } else if (obj->isDerivedFrom<Core::Ast::Bracket>()) {
      auto bracket = static_cast<Core::Ast::Bracket*>(obj);
      if (bracket->getType() != Core::Ast::BracketType::SQUARE) {
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidTypeElementNotice>(Core::Ast::findSourceLocation(obj))
        );
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
      if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
    } else {
      // Raise an error.
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidTypeElementNotice>(Core::Ast::findSourceLocation(obj))
      );
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
  }

  if (body == 0) {
    throw EXCEPTION(GenericException, S("Type body is missing."));
  }

  auto userType = Ast::UserType::create({
    { "prodId", currentList->getProdId()},
    { "sourceLocation", currentList->findSourceLocation() }
  }, {
    { "body", body }
  });

  SharedPtr<Core::Ast::Node> type;
  if (tmpltArgs != 0) {
    type = Ast::Template::create({}, {
      { S("varDefs"), tmpltArgs },
      { S("body"), userType }
    });
  } else {
    type = userType;
  }

  if (defName != 0) {
    state->setData(Core::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), currentList->getProdId() },
      { S("sourceLocation"), currentList->findSourceLocation() }
    }, {
      { S("target"), type }
    }));
  } else {
    state->setData(type);
  }
}


Bool TypeParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Ast::Definition>(data);

  if (definition == 0) return false;

  // Look for merge modifier. Its keyword may have already been translated by the call to the parent
  // implementation above.
  auto identifier = modifierData.ti_cast_get<Core::Ast::Identifier>();
  if (identifier != 0 && identifier->getValue() == S("merge")) {
    // Set toMerge in the definition.
    definition->setToMerge(true);
    return true;
  }

  // The modifier is unknown to us, leave it for an outer parsing handler to deal with.
  return false;
}

} // namespace
