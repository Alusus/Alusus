/**
 * @file Spp/Handlers/MacroParsingHandler.cpp
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Handlers
{

void MacroParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(expr != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(expr);
  ASSERT(exprMetadata != 0);

  if (expr->getElementCount() < 2) {
    throw EXCEPTION(GenericException, S("Invalid macro parsed data."));
  }

  // Prepare macro signature and body.
  Core::Data::Ast::Identifier *defName = 0;
  SharedPtr<Core::Data::Ast::Map> args;
  TiObject *body;
  if (expr->getElementCount() == 2) {
    // We have just a body.
    body = expr->getElement(1);
    args = Core::Data::Ast::Map::create();
  } else if (expr->getElementCount() == 3) {
    // We have a body plus either a name or a signature.
    body = expr->getElement(2);
    defName = ti_cast<Core::Data::Ast::Identifier>(expr->getElement(1));
    if (defName == 0) {
      auto signature = ti_cast<Core::Data::Ast::Bracket>(expr->getElement(1));
      if (signature == 0) {
        throw EXCEPTION(GenericException, S("Invalid macro signature."));
      }
      if (!this->parseArgs(state, signature, args)) {
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
    } else {
      args = Core::Data::Ast::Map::create();
    }
  } else {
    // We have a name, a signature, and a body.
    defName = ti_cast<Core::Data::Ast::Identifier>(expr->getElement(1));
    body = expr->getElement(3);
    auto signature = ti_cast<Core::Data::Ast::Bracket>(expr->getElement(2));
    if (signature == 0) {
      throw EXCEPTION(GenericException, S("Invalid macro signature."));
    }
    if (!this->parseArgs(state, signature, args)) {
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  // Create the macro.
  auto macro = Spp::Ast::Macro::create({
    { S("prodId"), exprMetadata->getProdId() },
    { S("sourceLocation"), exprMetadata->findSourceLocation() }
  }, {
    { S("argTypes"), args },
    { S("body"), body }
  });

  // Do we have just a macro, or a full definition?
  if (defName == 0) {
    state->setData(macro);
  } else {
    auto def = Core::Data::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), exprMetadata->getProdId() },
      { S("sourceLocation"), exprMetadata->findSourceLocation() }
    }, {
      { S("target"), macro }
    });
    state->setData(def);
  }
}


Bool MacroParsingHandler::parseArgs(
  Processing::ParserState *state, Core::Data::Ast::Bracket *bracket, SharedPtr<Core::Data::Ast::Map> &result
) {
  auto args = bracket->getOperand().get();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::List>()) {
    result = std::make_shared<Core::Data::Ast::Map>();
    auto argsList = static_cast<Core::Data::Ast::List*>(args);
    result = std::make_shared<Core::Data::Ast::Map>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidMacroArgDefNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, argsList->getElement(i), result)) return false;
    }
    return true;
  } else {
    result = std::make_shared<Core::Data::Ast::Map>();
    return this->parseArg(state, args, result);
  }
}


Bool MacroParsingHandler::parseArg(
  Core::Processing::ParserState *state, TiObject *arg, SharedPtr<Core::Data::Ast::Map> const &result
) {
  ASSERT(arg != 0);
  if (arg->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto link = static_cast<Core::Data::Ast::LinkOperator*>(arg);
    if (link->getType() != S(":")) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    auto name = link->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
    if (name == 0) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    if (!link->getSecond()->isDerivedFrom<Core::Data::Ast::Identifier>()) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    result->add(name->getValue().get(), link->getSecond());
    return true;
  } else if (arg->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto name = static_cast<Core::Data::Ast::Identifier*>(arg);
    result->add(name->getValue().get(), TioSharedPtr::null);
    return true;
  } else {
    state->addNotice(std::make_shared<Spp::Notices::InvalidMacroArgDefNotice>(Core::Data::Ast::findSourceLocation(arg)));
    return false;
  }
}


Bool MacroParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Data::Ast::Definition>(data);

  if (definition == 0) return false;

  // Add an unknown modifier.
  auto symbolDef = state->refTopProdLevel().getProd();
  Core::Data::Ast::translateModifier(symbolDef, modifierData.get());
  definition->addModifier(modifierData);

  return true;
}

} // namespace
