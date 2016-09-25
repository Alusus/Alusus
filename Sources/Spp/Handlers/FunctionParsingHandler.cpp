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
  auto exprMetadata = ti_cast<Core::Data::Ast::MetadataHolder>(expr);
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
        auto metadata = ti_cast<Core::Data::Ast::MetadataHolder>(argsList->get(i));
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
                                      SharedPtr<SharedMap> const &result)
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
  result->set(name->getValue().c_str(), type);
  return true;
}

} } // namespace
