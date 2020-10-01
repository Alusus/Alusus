/**
 * @file Spp/Handlers/TypeParsingHandler.cpp
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
      if (!this->parseTemplateArgs(state, bracket, tmpltArgs)) {
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


Bool TypeParsingHandler::parseTemplateArgs(
  Processing::ParserState *state, Core::Data::Ast::Bracket *bracket, SharedPtr<Core::Data::Ast::List> &result
) {
  auto args = bracket->getOperand().get();
  if (args == 0) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgNotice>(bracket->findSourceLocation()));
    return false;
  } else if (args->isDerivedFrom<Core::Data::Ast::List>()) {
    auto argsList = static_cast<Core::Data::Ast::List*>(args);
    result = newSrdObj<Core::Data::Ast::List>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i).get();
      if (arg == 0) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (!this->parseTemplateArg(state, arg, result)) return false;
    }
    return true;
  } else {
    result = newSrdObj<Core::Data::Ast::List>();
    if (!this->parseTemplateArg(state, args, result)) return false;
    return true;
  }
}


Bool TypeParsingHandler::parseTemplateArg(
  Core::Processing::ParserState *state, TiObject *astNode, SharedPtr<Core::Data::Ast::List> const &result
) {
  Str name;
  Ast::TemplateVarType type;
  auto link = ti_cast<Core::Data::Ast::LinkOperator>(astNode);
  if (link != 0 && link->getType() == S(":")) {
    auto identifier = link->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
    if (identifier == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgNameNotice>(link->findSourceLocation()));
      return false;
    }
    name = identifier->getValue().get();

    identifier = link->getSecond().ti_cast_get<Core::Data::Ast::Identifier>();
    if (identifier == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgTypeNotice>(link->findSourceLocation()));
      return false;
    }
    if (identifier->getValue() == S("type")) type = Ast::TemplateVarType::TYPE;
    else if (identifier->getValue() == S("function")) type = Ast::TemplateVarType::FUNCTION;
    else if (identifier->getValue() == S("integer")) type = Ast::TemplateVarType::INTEGER;
    else if (identifier->getValue() == S("string")) type = Ast::TemplateVarType::STRING;
    else {
      state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgTypeNotice>(link->findSourceLocation()));
      return false;
    }
  } else {
    state->addNotice(
      newSrdObj<Spp::Notices::InvalidTemplateArgNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  // Check if arg name is already used.
  for (Int i = 0; i < result->getCount(); ++i) {
    if (result->get(i).ti_cast_get<Ast::TemplateVarDef>()->getName() == name) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidTemplateArgNameNotice>(link->findSourceLocation()));
      return false;
    }
  }

  result->add(newSrdObj<Ast::TemplateVarDef>(name, type));
  return true;
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
