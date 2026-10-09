/**
 * @file Spp/Handlers/MacroParsingHandler.cpp
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

void MacroParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Ast::List>();
  ASSERT(expr != 0);

  if (expr->getElementCount() < 2) {
    throw EXCEPTION(GenericException, S("Invalid macro parsed data."));
  }

  // Prepare macro signature and body.
  Core::Ast::Identifier *defName = 0;
  SharedPtr<Core::Ast::Map> args;
  Core::Ast::Node *body;
  if (expr->getElementCount() == 2) {
    // We have just a body.
    body = expr->getElement(1);
    args = Core::Ast::Map::create();
  } else if (expr->getElementCount() == 3) {
    // We have a body plus either a name or a signature.
    body = expr->getElement(2);
    defName = ti_cast<Core::Ast::Identifier>(expr->getElement(1));
    if (defName == 0) {
      auto signature = ti_cast<Core::Ast::Bracket>(expr->getElement(1));
      if (signature == 0) {
        throw EXCEPTION(GenericException, S("Invalid macro signature."));
      }
      if (!this->parseArgs(state, signature, args)) {
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
    } else {
      args = Core::Ast::Map::create();
    }
  } else {
    // We have a name, a signature, and a body.
    defName = ti_cast<Core::Ast::Identifier>(expr->getElement(1));
    body = expr->getElement(3);
    auto signature = ti_cast<Core::Ast::Bracket>(expr->getElement(2));
    if (signature == 0) {
      throw EXCEPTION(GenericException, S("Invalid macro signature."));
    }
    if (!this->parseArgs(state, signature, args)) {
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
  }

  // Create the macro.
  auto macro = Spp::Ast::Macro::create({
    { S("prodId"), expr->getProdId() },
    { S("sourceLocation"), expr->findSourceLocation() }
  }, {
    { S("argTypes"), args },
    { S("body"), body }
  });

  // Do we have just a macro, or a full definition?
  if (defName == 0) {
    state->setData(macro);
  } else {
    auto def = Core::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), expr->getProdId() },
      { S("sourceLocation"), expr->findSourceLocation() }
    }, {
      { S("target"), macro }
    });
    state->setData(def);
  }
}


Bool MacroParsingHandler::parseArgs(
  Processing::ParserState *state, Core::Ast::Bracket *bracket, SharedPtr<Core::Ast::Map> &result
) {
  auto args = bracket->getOperand().get();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Ast::List>()) {
    result = newSrdObj<Core::Ast::Map>();
    auto argsList = static_cast<Core::Ast::List*>(args);
    result = newSrdObj<Core::Ast::Map>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidMacroArgDefNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, argsList->getElement(i), result)) return false;
    }
    return true;
  } else {
    result = newSrdObj<Core::Ast::Map>();
    return this->parseArg(state, args, result);
  }
}


Bool MacroParsingHandler::parseArg(
  Core::Processing::ParserState *state, Core::Ast::Node *arg, SharedPtr<Core::Ast::Map> const &result
) {
  ASSERT(arg != 0);
  if (arg->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto link = static_cast<Core::Ast::LinkOperator*>(arg);
    if (link->getType() != S(":")) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    auto name = link->getFirst().ti_cast_get<Core::Ast::Identifier>();
    if (name == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    if (!link->getSecond()->isDerivedFrom<Core::Ast::Identifier>()) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidMacroArgDefNotice>(link->findSourceLocation()));
      return false;
    }
    result->add(name->getValue().get(), link->getSecond());
    return true;
  } else if (arg->isDerivedFrom<Core::Ast::Identifier>()) {
    auto name = static_cast<Core::Ast::Identifier*>(arg);
    result->add(name->getValue().get(), SharedPtr<Core::Ast::Node>::null);
    return true;
  } else {
    state->addNotice(newSrdObj<Spp::Notices::InvalidMacroArgDefNotice>(Core::Ast::findSourceLocation(arg)));
    return false;
  }
}


Bool MacroParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Ast::Definition>(data);

  // Is this a @member modifier? Its keyword may have already been translated by the call to the parent
  // implementation above.
  auto identifier = modifierData.ti_cast_get<Core::Ast::Identifier>();
  if (identifier != 0 && identifier->getValue() == S("member")) {
    Spp::Ast::Macro *macro;
    if (definition != 0) macro = definition->getTarget().ti_cast_get<Spp::Ast::Macro>();
    else macro = ti_cast<Spp::Ast::Macro>(data);
    if (macro == 0) {
      throw EXCEPTION(GenericException, S("Unexpected data type while parsing macro modifier."));
    }
    macro->setMember(true);
    return true;
  }

  // The modifier is unknown to us, leave it for an outer parsing handler to deal with.
  return false;
}

} // namespace
