/**
 * @file Spp/Handlers/FunctionParsingHandler.cpp
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

void FunctionParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Ast::List>();

  if (expr == 0) {
    // The function type has no args and no body.
    auto functionType = newSrdObj<Spp::Ast::FunctionType>();
    auto node = state->getData().get();
    functionType->setArgTypes(SharedPtr<Core::Ast::Map>::null);
    functionType->setRetType(SharedPtr<Core::Ast::Node>::null);
    functionType->setSourceLocation(node->findSourceLocation());
    functionType->setProdId(node->getProdId());
    if (!processFunctionArgPacks(functionType.get(), state->getNoticeStore())) {
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
    state->setData(functionType);
    return;
  }

  // Prepare function signature.
  Core::Ast::Identifier *defName = 0;
  SharedPtr<Core::Ast::Map> args;
  SharedPtr<Core::Ast::Node> retType;
  SharedPtr<Core::Ast::List> tmpltArgs;
  Core::Ast::Scope *body = 0;

  for (Int i = 1; i < expr->getElementCount(); ++i) {
    auto obj = expr->getElement(i);
    if (obj->isDerivedFrom<Core::Ast::Scope>()) {
      body = static_cast<Core::Ast::Scope*>(obj);
    } else if (obj->isDerivedFrom<Core::Ast::Identifier>()) {
      defName = static_cast<Core::Ast::Identifier*>(obj);
    } else if (obj->isDerivedFrom<Core::Ast::LinkOperator>()) {
      auto linkOp = static_cast<Core::Ast::LinkOperator*>(obj);
      retType = linkOp->getSecond();
      auto bracket = linkOp->getFirst().ti_cast_get<Core::Ast::Bracket>();
      if (bracket == 0) {
        // Raise an error.
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidFunctionSignatureNotice>(Core::Ast::findSourceLocation(obj))
        );
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
      if (!this->parseArgs(state, bracket, args)) {
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
    } else if (obj->isDerivedFrom<Core::Ast::Bracket>()) {
      auto bracket = static_cast<Core::Ast::Bracket*>(obj);
      if (bracket->getType() == Core::Ast::BracketType::ROUND) {
        if (!this->parseArgs(state, bracket, args)) {
          state->setData(SharedPtr<Core::Ast::Node>(0));
          return;
        }
      } else {
        if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
          state->setData(SharedPtr<Core::Ast::Node>(0));
          return;
        }
      }
    } else {
      // Raise an error.
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidFunctionElementNotice>(Core::Ast::findSourceLocation(obj))
      );
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
  }

  auto functionType = newSrdObj<Spp::Ast::FunctionType>();
  functionType->setArgTypes(args);
  functionType->setRetType(retType);
  functionType->setSourceLocation(expr->findSourceLocation());
  functionType->setProdId(expr->getProdId());
  if (!processFunctionArgPacks(functionType.get(), state->getNoticeStore())) {
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  SharedPtr<Core::Ast::Node> stateData = functionType;

  if (body != 0) {
    auto function = newSrdObj<Spp::Ast::Function>();
    function->setType(functionType);
    function->setBody(getSharedPtr(body));
    function->setSourceLocation(expr->findSourceLocation());
    function->setProdId(expr->getProdId());

    if (tmpltArgs != 0) {
      stateData = Ast::Template::create({}, {
        { S("varDefs"), tmpltArgs },
        { S("body"), function }
      });
    } else {
      stateData = function;
    }
  } else if (tmpltArgs != 0) {
    state->addNotice(
      newSrdObj<Spp::Notices::TemplateFunctionLacksBodyNotice>(expr->findSourceLocation())
    );
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  if (defName != 0) {
    auto def = Core::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), expr->getProdId() },
      { S("sourceLocation"), expr->findSourceLocation() }
    }, {
      { S("target"), stateData }
    });
    stateData = def;
  }

  state->setData(stateData);
}


Bool FunctionParsingHandler::parseArgs(
  Processing::ParserState *state, Core::Ast::Bracket *bracket, SharedPtr<Core::Ast::Map> &result
) {
  auto args = bracket->getOperand();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Ast::List>()) {
    auto argsList = args.s_cast<Core::Ast::List>();
    result = newSrdObj<Core::Ast::Map>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, arg, result)) return false;
    }
    return true;
  } else {
    result = newSrdObj<Core::Ast::Map>();
    if (!this->parseArg(state, args, result)) return false;
    return true;
  }
}


Bool FunctionParsingHandler::parseArg(
  Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> astNode,
  SharedPtr<Core::Ast::Map> const &result
) {
  Str name;
  SharedPtr<Core::Ast::Node> type;
  auto link = astNode.ti_cast_get<Core::Ast::LinkOperator>();
  if (link != 0 && link->getType() == S(":")) {
    auto identifier = link->getFirst().ti_cast_get<Core::Ast::Identifier>();
    if (identifier == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(link->findSourceLocation()));
      return false;
    }
    name = identifier->getValue().get();
    type = link->getSecond();
    if (type == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(link->findSourceLocation()));
      return false;
    }
  } else {
    name = S("__");
    name += (LongInt)result->getCount();
    type = astNode;
  }
  if (result->findIndex(name) != -1) {
    // This arg name is already in use.
    state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(link->findSourceLocation()));
    return false;
  }
  result->set(name, type);
  return true;
}


Bool FunctionParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  if (this->processExpnameModifier(state, modifierData)) return true;
  else if (this->processMemberModifier(state, modifierData)) return true;
  else return this->processUnknownModifier(state, modifierData);
}


Bool FunctionParsingHandler::processExpnameModifier(
  Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> const &modifierData
) {
  // Look for expname modifier.
  auto paramPass = modifierData.ti_cast_get<Core::Ast::ParamPass>();
  if (paramPass == 0) return false;
  if (paramPass->getType() != Core::Ast::BracketType::SQUARE) return false;
  auto operand = paramPass->getOperand().ti_cast_get<Core::Ast::Identifier>();
  if (operand == 0) return false;
  auto symbolDef = state->refTopProdLevel().getProd();
  if (symbolDef->getTranslatedModifierKeyword(operand->getValue().get()) != S("expname")) return false;
  auto param = paramPass->getParam().ti_cast_get<Core::Ast::Text>();
  if (param == 0) return false;

  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset);
  if (data == 0) return false;

  // Grab the data from the definition, if any, otherwise use the data from the state level.
  Core::Ast::Definition *definition = 0;
  if (data->isDerivedFrom<Core::Ast::Definition>()) {
    definition = data.s_cast_get<Core::Ast::Definition>();
    data = definition->getTarget();
  }
  Spp::Ast::Function *function = data.ti_cast_get<Spp::Ast::Function>();

  // Set the function name.
  if (function == 0) {
    // The data isn't a function, so check if it's a FunctionType.
    auto functionType = ti_cast<Spp::Ast::FunctionType>(data);

    // If it's not a function type then we can't accept this modifier. This could be because the object is a template
    // which shouldn't accept expname modifiers.
    if (functionType == 0) return false;

    auto newFunction = newSrdObj<Spp::Ast::Function>();
    newFunction->setType(functionType);
    newFunction->setSourceLocation(functionType->findSourceLocation());
    newFunction->setProdId(functionType->getProdId());
    newFunction->setName(param->getValue());
    // If a definition exists, update its target with the new function, otherwise set the new function to the state.
    if (definition != 0) {
      definition->setTarget(newFunction);
    } else {
      state->setData(newFunction, levelOffset);
    }
  } else {
    function->setName(param->getValue());
  }

  return true;
}


Bool FunctionParsingHandler::processMemberModifier(
  Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> const &modifierData
) {
  // Look for member modifier.
  auto identifier = modifierData.ti_cast_get<Core::Ast::Identifier>();
  if (identifier == 0) return false;
  auto symbolDef = state->refTopProdLevel().getProd();
  auto keyword = symbolDef->getTranslatedModifierKeyword(identifier->getValue().get());

  if (keyword != S("member")) return false;

  // Find the funciton type to update.
  Int levelOffset = -state->getTopProdTermLevelCount();

  Core::Ast::Node *data = state->getData(levelOffset).get();
  if (data == 0) return false;

  // Grab the data from the definition, if any, otherwise use the data from the state level.
  Core::Ast::Definition *definition = 0;
  if (data->isDerivedFrom<Core::Ast::Definition>()) {
    definition = static_cast<Core::Ast::Definition*>(data);
    data = definition->getTarget().get();
  }

  if (data->isDerivedFrom<Spp::Ast::Template>()) {
    auto tpl = static_cast<Spp::Ast::Template*>(data);
    data = tpl->getBody().get();
  }

  Spp::Ast::FunctionType *funcType;
  if (data->isDerivedFrom<Spp::Ast::Function>()) {
    auto function = static_cast<Spp::Ast::Function*>(data);
    funcType = function->getType().get();
  } else if (data->isDerivedFrom<Spp::Ast::FunctionType>()) {
    funcType = static_cast<Spp::Ast::FunctionType*>(data);
  } else {
    throw EXCEPTION(GenericException, S("Unexpected data type found."));
  }

  funcType->setMember(true);

  return true;
}


Bool FunctionParsingHandler::processUnknownModifier(
  Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> const &modifierData
) {
  // Add an unknown modifier to the definition.
  auto symbolDef = state->refTopProdLevel().getProd();
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto definition = state->getData(levelOffset).ti_cast_get<Core::Ast::Definition>();
  if (definition != 0) {
    Core::Ast::translateModifier(symbolDef, modifierData.get());
    definition->addModifier(modifierData);
    return true;
  } else {
    return false;
  }
}

} // namespace
