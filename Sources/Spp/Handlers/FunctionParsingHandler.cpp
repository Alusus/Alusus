/**
 * @file Spp/Handlers/FunctionParsingHandler.cpp
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Handlers
{

using namespace Core;
using namespace Core::Data;

void FunctionParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(expr != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::Metadata>(expr);
  ASSERT(exprMetadata != 0);

  if (expr->getCount() < 2) {
    state->addNotice(std::make_shared<Spp::Notices::MissingFunctionSigNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare function signature.
  auto signature = expr->getElement(1);
  if (signature == 0) {
    state->addNotice(
      std::make_shared<Spp::Notices::InvalidFunctionSignatureNotice>(exprMetadata->findSourceLocation())
    );
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  Core::Data::Ast::Bracket *bracket;
  TioSharedPtr retType;
  auto linkOp = ti_cast<Core::Data::Ast::LinkOperator>(signature);
  if (linkOp != 0) {
    retType = linkOp->getSecond();
    bracket = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Bracket>();
  } else {
    bracket = ti_cast<Core::Data::Ast::Bracket>(signature);
  }
  if (bracket == 0) {
    state->addNotice(
      std::make_shared<Spp::Notices::InvalidFunctionSignatureNotice>(exprMetadata->findSourceLocation())
    );
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  SharedPtr<Core::Data::Ast::Map> args;
  if (!this->parseArgs(state, bracket, args)) {
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare function body.
  Spp::Ast::Block *body = 0;
  if (expr->getCount() == 3) {
    body = ti_cast<Spp::Ast::Block>(expr->getElement(2));
    if (body == 0) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionBodyNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  auto function = std::make_shared<Spp::Ast::Function>();
  function->setArgTypes(args);
  function->setRetType(retType);
  function->setBody(getSharedPtr(body));
  function->setSourceLocation(exprMetadata->findSourceLocation());
  function->setProdId(exprMetadata->getProdId());

  state->setData(function);
}


Bool FunctionParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  Int levelOffset = -state->getTopProdTermLevelCount();
  auto function = state->getData(levelOffset).ti_cast_get<Spp::Ast::Function>();
  ASSERT(function != 0);
  this->prepareToModifyData(state, levelOffset);
  function = state->getData(levelOffset).ti_cast_get<Spp::Ast::Function>();
  ASSERT(function != 0);

  // Set the function name.
  auto paramPass = modifierData.ti_cast_get<Core::Data::Ast::ParamPass>();
  if (paramPass == 0) return false;
  if (paramPass->getType() != Core::Data::Ast::BracketType::SQUARE) return false;
  auto operand = paramPass->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0) return false;
  if (operand->getValue() != STR("expname")) return false;
  auto param = paramPass->getParam().ti_cast_get<Core::Data::Ast::Identifier>();
  if (param == 0) return false;
  function->setName(param->getValue());

  return true;
}


Bool FunctionParsingHandler::parseArgs(Processing::ParserState *state,
                                       Core::Data::Ast::Bracket *bracket,
                                       SharedPtr<Core::Data::Ast::Map> &result)
{
  auto args = bracket->getOperand().get();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::ExpressionList>()) {
    auto argsList = static_cast<Core::Data::Ast::ExpressionList*>(args);
    result = std::make_shared<Core::Data::Ast::Map>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNotice>(bracket->findSourceLocation()));
        return false;
      }
      auto link = ti_cast<Core::Data::Ast::LinkOperator>(argsList->getElement(i));
      if (link == 0) {
        auto metadata = ti_cast<Core::Data::Ast::Metadata>(argsList->getElement(i));
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNotice>(
          metadata == 0 ? bracket->findSourceLocation() : metadata->findSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, link, result)) false;
    }
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto link = static_cast<Core::Data::Ast::LinkOperator*>(args);
    result = std::make_shared<Core::Data::Ast::Map>();
    if (!this->parseArg(state, link, result)) return false;
    return true;
  } else {
    state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionSignatureNotice>(bracket->findSourceLocation()));
    return false;
  }
}


Bool FunctionParsingHandler::parseArg(Core::Processing::ParserState *state,
                                      Core::Data::Ast::LinkOperator *link,
                                      SharedPtr<Core::Data::Ast::Map> const &result)
{
  auto name = link->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (name == 0) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNameNotice>(link->findSourceLocation()));
    return false;
  }
  auto type = link->getSecond();
  if (type == 0) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(link->findSourceLocation()));
    return false;
  }
  if (type->isA<Core::Data::Ast::PrefixOperator>()) {
    auto prefixOp = type.s_cast<Core::Data::Ast::PrefixOperator>();
    if (prefixOp->getType() != STR("...")) {
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
      if (bracketOperand->isA<Core::Data::Ast::ExpressionList>()) {
        auto bracketList = bracketOperand.s_cast<Core::Data::Ast::ExpressionList>();
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
      if (packIdentifier->getValue() == STR("any")) {
        packType = 0;
      }
    }
    type = Ast::ArgPack::create({
      { STR("min"), &packMin },
      { STR("max"), &packMax }
    }, {
      { STR("argType"), packType }
    });
  } else {
    // Do we have an arg pack prior to this arg?
    for (Int i = 0; i < result->getCount(); ++i) {
      if (result->get(i)->isDerivedFrom<Ast::ArgPack>()) {
        // We cannot have a normal argument following an arg pack.
        state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgTypeNotice>(link->findSourceLocation()));
        return false;
      }
    }
  }
  result->set(name->getValue().get(), type);
  return true;
}


Bool FunctionParsingHandler::parseNumber(Core::Processing::ParserState *state, TiObject *ast, TiWord &result,
                                         Core::Data::Ast::Metadata *parentMetadata)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(ast);
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

} } // namespace
