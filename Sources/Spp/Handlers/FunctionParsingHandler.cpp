/**
 * @file Spp/Handlers/FunctionParsingHandler.cpp
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

void FunctionParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();

  if (expr == 0) {
    // The function type has no args and no body.
    auto functionType = newSrdObj<Spp::Ast::FunctionType>();
    auto metadata = state->getData().ti_cast_get<Core::Data::Ast::MetaHaving>();
    functionType->setArgTypes(SharedPtr<Core::Data::Ast::Map>::null);
    functionType->setRetType(TioSharedPtr::null);
    functionType->setSourceLocation(metadata->findSourceLocation());
    functionType->setProdId(metadata->getProdId());
    if (!processFunctionArgPacks(functionType.get(), state->getNoticeStore())) {
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
    state->setData(functionType);
    return;
  }

  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(expr);
  ASSERT(exprMetadata != 0);

  // Prepare function signature.
  Core::Data::Ast::Identifier *defName = 0;
  SharedPtr<Core::Data::Ast::Map> args;
  TioSharedPtr retType;
  SharedPtr<Core::Data::Ast::List> tmpltArgs;
  Core::Data::Ast::Scope *body = 0;

  for (Int i = 1; i < expr->getElementCount(); ++i) {
    auto obj = expr->getElement(i);
    if (obj->isDerivedFrom<Core::Data::Ast::Scope>()) {
      body = static_cast<Core::Data::Ast::Scope*>(obj);
    } else if (obj->isDerivedFrom<Core::Data::Ast::Identifier>()) {
      defName = static_cast<Core::Data::Ast::Identifier*>(obj);
    } else if (obj->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
      auto linkOp = static_cast<Core::Data::Ast::LinkOperator*>(obj);
      retType = linkOp->getSecond();
      auto bracket = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Bracket>();
      if (bracket == 0) {
        // Raise an error.
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidFunctionSignatureNotice>(Core::Data::Ast::findSourceLocation(obj))
        );
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
      if (!this->parseArgs(state, bracket, args)) {
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
    } else if (obj->isDerivedFrom<Core::Data::Ast::Bracket>()) {
      auto bracket = static_cast<Core::Data::Ast::Bracket*>(obj);
      if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
        if (!this->parseArgs(state, bracket, args)) {
          state->setData(SharedPtr<TiObject>(0));
          return;
        }
      } else {
        if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
          state->setData(SharedPtr<TiObject>(0));
          return;
        }
      }
    } else {
      // Raise an error.
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidFunctionElementNotice>(Core::Data::Ast::findSourceLocation(obj))
      );
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  auto functionType = newSrdObj<Spp::Ast::FunctionType>();
  functionType->setArgTypes(args);
  functionType->setRetType(retType);
  functionType->setSourceLocation(exprMetadata->findSourceLocation());
  functionType->setProdId(exprMetadata->getProdId());
  if (!processFunctionArgPacks(functionType.get(), state->getNoticeStore())) {
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  TioSharedPtr stateData = functionType;

  if (body != 0) {
    auto function = newSrdObj<Spp::Ast::Function>();
    function->setType(functionType);
    function->setBody(getSharedPtr(body));
    function->setSourceLocation(exprMetadata->findSourceLocation());
    function->setProdId(exprMetadata->getProdId());

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
      newSrdObj<Spp::Notices::TemplateFunctionLacksBodyNotice>(exprMetadata->findSourceLocation())
    );
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  if (defName != 0) {
    auto def = Core::Data::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), exprMetadata->getProdId() },
      { S("sourceLocation"), exprMetadata->findSourceLocation() }
    }, {
      { S("target"), stateData }
    });
    stateData = def;
  }

  state->setData(stateData);
}


Bool FunctionParsingHandler::parseArgs(
  Processing::ParserState *state, Core::Data::Ast::Bracket *bracket, SharedPtr<Core::Data::Ast::Map> &result
) {
  auto args = bracket->getOperand();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::List>()) {
    auto argsList = args.s_cast<Core::Data::Ast::List>();
    result = newSrdObj<Core::Data::Ast::Map>();
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
    result = newSrdObj<Core::Data::Ast::Map>();
    if (!this->parseArg(state, args, result)) return false;
    return true;
  }
}


Bool FunctionParsingHandler::parseArg(
  Core::Processing::ParserState *state, TioSharedPtr astNode, SharedPtr<Core::Data::Ast::Map> const &result
) {
  Str name;
  TioSharedPtr type;
  auto link = astNode.ti_cast_get<Core::Data::Ast::LinkOperator>();
  if (link != 0 && link->getType() == S(":")) {
    auto identifier = link->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
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
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  if (this->processExpnameModifier(state, modifierData)) return true;
  else if (this->processMemberModifier(state, modifierData)) return true;
  else return this->processUnknownModifier(state, modifierData);
}


Bool FunctionParsingHandler::processExpnameModifier(
  Core::Processing::ParserState *state, TioSharedPtr const &modifierData
) {
  // Look for expname modifier.
  auto paramPass = modifierData.ti_cast_get<Core::Data::Ast::ParamPass>();
  if (paramPass == 0) return false;
  if (paramPass->getType() != Core::Data::Ast::BracketType::SQUARE) return false;
  auto operand = paramPass->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0) return false;
  auto symbolDef = state->refTopProdLevel().getProd();
  if (symbolDef->getTranslatedModifierKeyword(operand->getValue().get()) != S("expname")) return false;
  auto param = paramPass->getParam().ti_cast_get<Core::Data::Ast::Text>();
  if (param == 0) return false;

  Int levelOffset = -state->getTopProdTermLevelCount();
  TioSharedPtr data = state->getData(levelOffset);
  if (data == 0) return false;

  // Grab the data from the definition, if any, otherwise use the data from the state level.
  Core::Data::Ast::Definition *definition = 0;
  if (data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    definition = data.s_cast_get<Core::Data::Ast::Definition>();
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
  Core::Processing::ParserState *state, TioSharedPtr const &modifierData
) {
  // Look for member modifier.
  auto identifier = modifierData.ti_cast_get<Core::Data::Ast::Identifier>();
  if (identifier == 0) return false;
  auto symbolDef = state->refTopProdLevel().getProd();
  auto keyword = symbolDef->getTranslatedModifierKeyword(identifier->getValue().get());

  if (keyword != S("member")) return false;

  // Find the funciton type to update.
  Int levelOffset = -state->getTopProdTermLevelCount();

  TiObject *data = state->getData(levelOffset).get();
  if (data == 0) return false;

  // Grab the data from the definition, if any, otherwise use the data from the state level.
  Core::Data::Ast::Definition *definition = 0;
  if (data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    definition = static_cast<Core::Data::Ast::Definition*>(data);
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
  Core::Processing::ParserState *state, TioSharedPtr const &modifierData
) {
  // Add an unknown modifier to the definition.
  auto symbolDef = state->refTopProdLevel().getProd();
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto definition = state->getData(levelOffset).ti_cast_get<Core::Data::Ast::Definition>();
  if (definition != 0) {
    Core::Data::Ast::translateModifier(symbolDef, modifierData.get());
    definition->addModifier(modifierData);
    return true;
  } else {
    return false;
  }
}

} // namespace
