/**
 * @file Spp/Handlers/FunctionParsingHandler.cpp
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

void FunctionParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();

  if (expr == 0) {
    // The function type has no args and no body.
    auto functionType = std::make_shared<Spp::Ast::FunctionType>();
    auto metadata = state->getData().ti_cast_get<Core::Data::Ast::MetaHaving>();
    functionType->setArgTypes(SharedPtr<Core::Data::Ast::Map>::null);
    functionType->setRetType(TioSharedPtr::null);
    functionType->setSourceLocation(metadata->findSourceLocation());
    functionType->setProdId(metadata->getProdId());
    state->setData(functionType);
    return;
  }

  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(expr);
  ASSERT(exprMetadata != 0);

  // Prepare function signature.
  Core::Data::Ast::Identifier *defName = 0;
  SharedPtr<Core::Data::Ast::Map> args;
  TioSharedPtr retType;
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
          std::make_shared<Spp::Notices::InvalidFunctionSignatureNotice>(Core::Data::Ast::findSourceLocation(obj))
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
      if (!this->parseArgs(state, bracket, args)) {
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
    } else {
      // Raise an error.
      state->addNotice(
        std::make_shared<Spp::Notices::InvalidFunctionElementNotice>(Core::Data::Ast::findSourceLocation(obj))
      );
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  auto functionType = std::make_shared<Spp::Ast::FunctionType>();
  functionType->setArgTypes(args);
  functionType->setRetType(retType);
  functionType->setSourceLocation(exprMetadata->findSourceLocation());
  functionType->setProdId(exprMetadata->getProdId());

  TioSharedPtr stateData = functionType;

  if (body != 0) {
    auto function = std::make_shared<Spp::Ast::Function>();
    function->setType(functionType);
    function->setBody(getSharedPtr(body));
    function->setSourceLocation(exprMetadata->findSourceLocation());
    function->setProdId(exprMetadata->getProdId());
    stateData = function;
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
    result = std::make_shared<Core::Data::Ast::Map>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, arg, result)) return false;
    }
    return true;
  } else {
    result = std::make_shared<Core::Data::Ast::Map>();
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
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNameNotice>(link->findSourceLocation()));
      return false;
    }
    name = identifier->getValue().get();
    type = link->getSecond();
    if (type == 0) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(link->findSourceLocation()));
      return false;
    }
  } else {
    name = S("__");
    name += std::to_string(result->getCount());
    type = astNode;
  }
  if (result->findIndex(name.c_str()) != -1) {
    // This arg name is already in use.
    state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNameNotice>(link->findSourceLocation()));
    return false;
  }
  if (type->isA<Core::Data::Ast::PrefixOperator>()) {
    auto prefixOp = type.s_cast<Core::Data::Ast::PrefixOperator>();
    if (prefixOp->getType() != S("...")) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(prefixOp->findSourceLocation()));
      return false;
    }
    TioSharedPtr packType;
    TiWord packMin = 0;
    TiWord packMax = 0;
    auto operand = prefixOp->getOperand();
    if (operand->isA<Core::Data::Ast::Bracket>()) {
      auto bracket = operand.s_cast<Core::Data::Ast::Bracket>();
      if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(bracket->findSourceLocation()));
        return false;
      }
      auto bracketOperand = bracket->getOperand();
      if (bracketOperand == 0) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(bracket->findSourceLocation()));
        return false;
      }
      if (bracketOperand->isA<Core::Data::Ast::List>()) {
        auto bracketList = bracketOperand.s_cast<Core::Data::Ast::List>();
        if (bracketList->getCount() == 0 || bracketList->getCount() > 3) {
          state->addNotice(
            std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(bracketList->findSourceLocation())
          );
          return false;
        }
        packType = bracketList->get(0);
        if (bracketList->getCount() > 1) {
          if (!this->parseNumber(state, bracketList->getElement(1), packMin, bracketList.get())) return false;
        }
        if (bracketList->getCount() > 2) {
          if (!this->parseNumber(state, bracketList->getElement(2), packMax, bracketList.get())) return false;
        }
      } else {
        packType = bracketOperand;
      }
    } else {
      packType = operand;
    }
    if (packType->isA<Core::Data::Ast::Identifier>()) {
      auto packIdentifier = packType.s_cast<Core::Data::Ast::Identifier>();
      if (packIdentifier->getValue() == S("any")) {
        packType = 0;
      }
    }
    type = Ast::ArgPack::create({
      { S("min"), &packMin },
      { S("max"), &packMax }
    }, {
      { S("argType"), packType }
    });
  } else {
    // Do we have an arg pack prior to this arg?
    for (Int i = 0; i < result->getCount(); ++i) {
      if (result->get(i)->isDerivedFrom<Ast::ArgPack>()) {
        // We cannot have a normal argument following an arg pack.
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(
          Core::Data::Ast::findSourceLocation(astNode.get())
        ));
        return false;
      }
    }
  }
  result->set(name.c_str(), type);
  return true;
}


Bool FunctionParsingHandler::parseNumber(
  Core::Processing::ParserState *state, TiObject *ast, TiWord &result, Core::Data::Ast::MetaHaving *parentMetadata
) {
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(ast);
  if (ast->isA<Core::Data::Ast::IntegerLiteral>()) {
    result = std::stol(static_cast<Core::Data::Ast::IntegerLiteral*>(ast)->getValue().get());
    return true;
  } else {
    if (metadata) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(metadata->findSourceLocation()));
    } else {
      state->addNotice(
        std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(parentMetadata->findSourceLocation())
      );
    }
    return false;
  }
}


Bool FunctionParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  if (this->processExpnameModifier(state, modifierData)) return true;
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
  auto param = paramPass->getParam().ti_cast_get<Core::Data::Ast::Identifier>();
  if (param == 0) return false;

  // If we have generated a definition object in this state, clone it then get the clone.
  Int levelOffset = -state->getTopProdTermLevelCount();
  Core::Data::Ast::Definition *definition = 0;
  if (state->getData(levelOffset)->isDerivedFrom<Core::Data::Ast::Definition>()) {
    definition = state->getData(levelOffset).s_cast_get<Core::Data::Ast::Definition>();
  }

  // Grab the data from the definition, if any, otherwise grab it from the state itself.
  Spp::Ast::Function *function;
  TioSharedPtr data;
  if (definition != 0) {
    data = definition->getTarget();
  } else {
    data = state->getData(levelOffset);
  }
  function = data.ti_cast_get<Spp::Ast::Function>();

  // Set the function name.
  if (function == 0) {
    // The data isn't a function, so it must be a FunctionType.
    auto functionType = ti_cast<Spp::Ast::FunctionType>(data);
    ASSERT(functionType != 0);
    auto newFunction = std::make_shared<Spp::Ast::Function>();
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
