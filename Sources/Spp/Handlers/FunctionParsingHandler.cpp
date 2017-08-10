/**
 * @file Spp/Handlers/FunctionParsingHandler.cpp
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

  if (expr->getCount() != 3) {
    state->addBuildMsg(std::make_shared<MissingFunctionSigOrBody>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare function signature.
  auto signature = expr->get(1);
  if (signature == 0) {
    state->addBuildMsg(std::make_shared<InvalidFunctionSignature>(exprMetadata->getSourceLocation()));
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
    state->addBuildMsg(std::make_shared<InvalidFunctionSignature>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  SharedPtr<Core::Data::SharedMap> args;
  if (!this->parseArgs(state, bracket, args)) {
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare function body.
  auto body = ti_cast<Spp::Ast::Block>(expr->get(2));
  if (body == 0) {
    state->addBuildMsg(std::make_shared<InvalidFunctionBody>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  auto function = std::make_shared<Spp::Ast::Function>();
  function->setArgTypes(args);
  function->setRetType(retType);
  function->setBody(getSharedPtr(body));
  function->setSourceLocation(exprMetadata->getSourceLocation());
  function->setProdId(exprMetadata->getProdId());

  state->setData(function);
}


Bool FunctionParsingHandler::parseArgs(Processing::ParserState *state,
                                       Core::Data::Ast::Bracket *bracket,
                                       SharedPtr<Core::Data::SharedMap> &result)
{
  auto args = bracket->getOperand().get();
  if (args == 0) {
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::ExpressionList>()) {
    auto argsList = static_cast<Core::Data::Ast::ExpressionList*>(args);
    result = std::make_shared<Core::Data::SharedMap>();
    for (Int i = 0; i < argsList->getCount(); ++i) {
      auto arg = argsList->get(i);
      if (arg == 0) {
        state->addBuildMsg(std::make_shared<InvalidFunctionArg>(bracket->getSourceLocation()));
        return false;
      }
      auto link = ti_cast<Core::Data::Ast::LinkOperator>(argsList->get(i));
      if (link == 0) {
        auto metadata = ti_cast<Core::Data::Ast::Metadata>(argsList->get(i));
        state->addBuildMsg(std::make_shared<InvalidFunctionArg>(
          metadata == 0 ? bracket->getSourceLocation() : metadata->getSourceLocation()));
        return false;
      }
      if (!this->parseArg(state, link, result)) false;
    }
    return true;
  } else if (args->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto link = static_cast<Core::Data::Ast::LinkOperator*>(args);
    result = std::make_shared<Core::Data::SharedMap>();
    if (!this->parseArg(state, link, result)) return false;
    return true;
  } else {
    state->addBuildMsg(std::make_shared<InvalidFunctionSignature>(bracket->getSourceLocation()));
    return false;
  }
}


Bool FunctionParsingHandler::parseArg(Core::Processing::ParserState *state,
                                      Core::Data::Ast::LinkOperator *link,
                                      SharedPtr<Core::Data::SharedMap> const &result)
{
  auto name = link->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (name == 0) {
    state->addBuildMsg(std::make_shared<InvalidFunctionArgName>(link->getSourceLocation()));
    return false;
  }
  auto type = link->getSecond();
  if (type == 0) {
    state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(link->getSourceLocation()));
    return false;
  }
  if (type->isA<Core::Data::Ast::PrefixOperator>()) {
    auto prefixOp = type.s_cast<Core::Data::Ast::PrefixOperator>();
    if (prefixOp->getType() != STR("...")) {
      state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(prefixOp->getSourceLocation()));
      return false;
    }
    TioSharedPtr packType;
    TiWord packMin = 0;
    TiWord packMax = 0;
    auto operand = prefixOp->getOperand();
    if (operand->isA<Core::Data::Ast::Bracket>()) {
      auto bracket = operand.s_cast<Core::Data::Ast::Bracket>();
      if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
        state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(bracket->getSourceLocation()));
        return false;
      }
      auto bracketOperand = bracket->getOperand();
      if (bracketOperand == 0) {
        state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(bracket->getSourceLocation()));
        return false;
      }
      if (bracketOperand->isA<Core::Data::Ast::ExpressionList>()) {
        auto bracketList = bracketOperand.s_cast<Core::Data::Ast::ExpressionList>();
        if (bracketList->getCount() == 0 || bracketList->getCount() > 3) {
          state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(bracketList->getSourceLocation()));
          return false;
        }
        packType = bracketList->getShared(0);
        if (bracketList->getCount() > 1) {
          if (!this->parseNumber(state, bracketList->get(1), packMin, bracketList.get())) return false;
        }
        if (bracketList->getCount() > 2) {
          if (!this->parseNumber(state, bracketList->get(2), packMax, bracketList.get())) return false;
        }
      } else {
        packType = bracketOperand;
      }
    } else {
      packType = operand;
    }
    type = Ast::ArgPack::create({
      { STR("min"), &packMin },
      { STR("max"), &packMax }
    }, {
      { STR("argType"), packType }
    });
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
      state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(metadata->getSourceLocation()));
    } else {
      state->addBuildMsg(std::make_shared<InvalidFunctionArgType>(parentMetadata->getSourceLocation()));
    }
    return false;
  }
}

} } // namespace
