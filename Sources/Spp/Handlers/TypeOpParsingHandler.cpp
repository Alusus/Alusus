/**
 * @file Spp/Handlers/TypeOpParsingHandler.cpp
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

void TypeOpParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto data = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(data != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(data);
  ASSERT(exprMetadata != 0);

  if (data->getCount() != 3) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  for (Int i = 1; i < data->getCount(); ++i) {
    if (data->get(i) == 0) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidForStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  auto body = this->prepareBody(data->get(2));

  SharedPtr<Core::Data::Ast::Definition> def;

  auto expr = data->getElement(1);
  if (expr->isDerivedFrom<Core::Data::Ast::AssignmentOperator>()) {
    this->createAssignmentHandler(state, static_cast<Core::Data::Ast::AssignmentOperator*>(expr), body);
    return;
  } else if (expr->isDerivedFrom<Core::Data::Ast::ComparisonOperator>()) {
    this->createComparisonHandler(state, static_cast<Core::Data::Ast::ComparisonOperator*>(expr), body);
    return;
  } else if (expr->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Data::Ast::LinkOperator*>(expr);
    if (linkOp->getType() == S(":") || linkOp->getType() == S("=>")) {
      auto first = linkOp->getFirst().get();
      if (first->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
        auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(first);
        if (paramPass->getType() == Core::Data::Ast::BracketType::ROUND) {
          this->createParensOpHandler(state, paramPass, body, linkOp->getSecond());
          return;
        }
      } else if (first->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
        auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(first);
        this->createInfixOpHandler(state, infixOp, body, linkOp->getSecond());
        return;
      }
    }
  } else if (expr->isDerivedFrom<Spp::Ast::InitOp>()) {
    this->createInitOpHandler(state, static_cast<Spp::Ast::InitOp*>(expr), body);
    return;
  } else if (expr->isDerivedFrom<Spp::Ast::TerminateOp>()) {
    this->createTerminateOpHandler(state, static_cast<Spp::Ast::TerminateOp*>(expr), body);
    return;
  } else if (expr->isDerivedFrom<Spp::Ast::CastOp>()) {
    this->createCastHandler(state, static_cast<Spp::Ast::CastOp*>(expr), body);
    return;
  } else if (expr->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(expr);
    if (paramPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      this->createParensOpHandler(state, paramPass, body, TioSharedPtr::null);
      return;
    }
  }

  state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(exprMetadata->findSourceLocation()));
  state->setData(SharedPtr<TiObject>(0));
}


SharedPtr<Core::Data::Ast::Scope> TypeOpParsingHandler::prepareBody(TioSharedPtr const &stmt)
{
  SharedPtr<Core::Data::Ast::Scope> body;
  if (stmt->isDerivedFrom<Core::Data::Ast::Scope>()) {
    body = stmt.s_cast<Core::Data::Ast::Scope>();
  } else {
    body = Core::Data::Ast::Scope::create();
    body->setSourceLocation(Core::Data::Ast::findSourceLocation(stmt.get()));
    body->add(stmt);
    body = body;
  }
  return body;
}


void TypeOpParsingHandler::createAssignmentHandler(
  Processing::ParserState *state, Core::Data::Ast::AssignmentOperator *assignmentOp,
  SharedPtr<Core::Data::Ast::Scope> const &body
) {
  Char const *funcName = assignmentOp->getType();

  auto first = assignmentOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (first == 0 || first->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(assignmentOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Attach a return statement to body.
  body->add(Spp::Ast::ReturnStatement::create({}, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("value"), TiStr(S("this"))}
    })}
  }));

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, assignmentOp->getSecond(), inputName, inputType)) return;

  // Prepare this type.
  auto thisType = this->prepareThisType(first->findSourceLocation());

  // Prepare ret type.
  auto retType = this->prepareAssignmentRetType(assignmentOp->findSourceLocation());

  auto def = this->createBinaryOpFunction(
    funcName, thisType, inputName,  inputType, retType, body, assignmentOp->findSourceLocation()
  );
  state->setData(def);
}


void TypeOpParsingHandler::createComparisonHandler(
  Processing::ParserState *state, Core::Data::Ast::ComparisonOperator *comparisonOp,
  SharedPtr<Core::Data::Ast::Scope> const &body
) {
  Char const *funcName = comparisonOp->getType();

  auto first = comparisonOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (first == 0 || first->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(comparisonOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, comparisonOp->getSecond(), inputName, inputType)) return;

  // Prepare this type.
  auto thisType = this->prepareThisType(first->findSourceLocation());

  // Prepare ret type.
  auto retType = this->prepareComparisonRetType(comparisonOp->findSourceLocation());

  auto def = this->createBinaryOpFunction(
    funcName, thisType, inputName,  inputType, retType, body, comparisonOp->findSourceLocation()
  );
  state->setData(def);
}


void TypeOpParsingHandler::createInfixOpHandler(
  Processing::ParserState *state, Core::Data::Ast::InfixOperator *infixOp,
  SharedPtr<Core::Data::Ast::Scope> const &body, TioSharedPtr const &retType
) {
  Char const *funcName = infixOp->getType();

  auto first = infixOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (first == 0 || first->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(infixOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, infixOp->getSecond(), inputName, inputType)) return;

  // Prepare this type.
  auto thisType = this->prepareThisType(first->findSourceLocation());

  auto def = this->createBinaryOpFunction(
    funcName, thisType, inputName,  inputType, retType, body, infixOp->findSourceLocation()
  );
  state->setData(def);
}


void TypeOpParsingHandler::createInitOpHandler(
  Processing::ParserState *state, Spp::Ast::InitOp *initOp,
  SharedPtr<Core::Data::Ast::Scope> const &body
) {
  // Verify operand.
  auto operand = initOp->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(initOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare params.
  auto param = initOp->getParam();
  Core::Data::Ast::List tempParams;
  auto params = ti_cast<Core::Data::Ast::List>(param.get());
  if (params == 0) {
    if (param != 0) tempParams.add(param);
    params = &tempParams;
  }
  auto argTypes = Core::Data::Ast::Map::create();
  auto thisType = this->prepareThisType(operand->findSourceLocation());
  argTypes->add(S("this"), thisType);
  for (Int i = 0; i < params->getCount(); ++i) {
    Char const *inputName;
    TioSharedPtr inputType;
    auto inputDef = params->get(i);
    if (!this->prepareInputArg(state, inputDef, inputName, inputType)) return;
    if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Data::Ast::findSourceLocation(inputDef.get())
      ));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
    argTypes->add(inputName, inputType);
  }

  auto def = this->createFunction(
    S("~init"), argTypes, TioSharedPtr::null, body, initOp->findSourceLocation()
  );
  state->setData(def);
}


void TypeOpParsingHandler::createTerminateOpHandler(
  Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
  SharedPtr<Core::Data::Ast::Scope> const &body
) {
  // Verify operand.
  auto operand = terminateOp->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(terminateOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare params.
  auto argTypes = Core::Data::Ast::Map::create();
  auto thisType = this->prepareThisType(operand->findSourceLocation());
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    S("~terminate"), argTypes, TioSharedPtr::null, body, terminateOp->findSourceLocation()
  );
  state->setData(def);
}


void TypeOpParsingHandler::createCastHandler(
  Processing::ParserState *state, Spp::Ast::CastOp *castOp,
  SharedPtr<Core::Data::Ast::Scope> const &body
) {
  // Verify operand.
  auto operand = castOp->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(castOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare return type.
  auto retType = castOp->getTargetType();
  if (retType == 0) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(castOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare params.
  auto argTypes = Core::Data::Ast::Map::create();
  auto thisType = this->prepareThisType(operand->findSourceLocation());
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(S("~cast"), argTypes, retType, body, castOp->findSourceLocation());
  state->setData(def);
}


void TypeOpParsingHandler::createParensOpHandler(
  Processing::ParserState *state, Core::Data::Ast::ParamPass *parensOp,
  SharedPtr<Core::Data::Ast::Scope> const &body, TioSharedPtr const &retType
) {
  // Verify operand.
  auto operand = parensOp->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(parensOp->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Prepare params.
  auto param = parensOp->getParam();
  Core::Data::Ast::List tempParams;
  auto params = ti_cast<Core::Data::Ast::List>(param.get());
  if (params == 0) {
    if (param != 0) tempParams.add(param);
    params = &tempParams;
  }
  auto argTypes = Core::Data::Ast::Map::create();
  auto thisType = this->prepareThisType(operand->findSourceLocation());
  argTypes->add(S("this"), thisType);
  for (Int i = 0; i < params->getCount(); ++i) {
    Char const *inputName;
    TioSharedPtr inputType;
    auto inputDef = params->get(i);
    if (!this->prepareInputArg(state, inputDef, inputName, inputType)) return;
    if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Data::Ast::findSourceLocation(inputDef.get())
      ));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
    argTypes->add(inputName, inputType);
  }

  auto def = this->createFunction(
    S("()"), argTypes, retType, body, parensOp->findSourceLocation()
  );
  state->setData(def);
}


SharedPtr<Core::Data::Ast::Definition> TypeOpParsingHandler::createBinaryOpFunction(
  Char const *funcName, TioSharedPtr const &thisType, Char const *inputName, TioSharedPtr const &inputType,
  TioSharedPtr const &retType, TioSharedPtr const &body, SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  // Prepare arg types map.
  auto argTypes = Core::Data::Ast::Map::create();
  argTypes->add(S("this"), thisType);
  argTypes->add(inputName, inputType);

  return this->createFunction(funcName, argTypes, retType, body, sourceLocation);
}


SharedPtr<Core::Data::Ast::Definition> TypeOpParsingHandler::createFunction(
  Char const *funcName, SharedPtr<Core::Data::Ast::Map> const argTypes, TioSharedPtr const &retType,
  TioSharedPtr const &body, SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  // Create the function type.
  auto funcType = Spp::Ast::FunctionType::create({}, {
    {S("argTypes"), argTypes},
    {S("retType"), retType}
  });

  // Create the function.
  auto func = Spp::Ast::Function::create({}, {
    {S("type"), funcType},
    {S("body"), body}
  });

  // Create the definition.
  return this->createDefinition(funcName, func, sourceLocation);
}


Bool TypeOpParsingHandler::prepareInputArg(
  Processing::ParserState *state, TioSharedPtr input, Char const *&inputName, TioSharedPtr &inputType
) {
  if (input->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = input.s_cast_get<Core::Data::Ast::Bracket>();
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
      input = bracket->getOperand();
    } else {
      state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(
        Core::Data::Ast::findSourceLocation(input.get())
      ));
      state->setData(SharedPtr<TiObject>(0));
      return false;
    }
  }
  if (input->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOperator = input.s_cast_get<Core::Data::Ast::LinkOperator>();
    auto inputNameId = linkOperator->getFirst().ti_cast<Core::Data::Ast::Identifier>();
    if (inputNameId == 0 || inputNameId->getValue() == S("this")) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidHandlerStatementNotice>(
        Core::Data::Ast::findSourceLocation(input.get())
      ));
      state->setData(SharedPtr<TiObject>(0));
      return false;
    }
    inputName = inputNameId->getValue().get();
    inputType = linkOperator->getSecond();
  } else {
    inputName = S("value");
    inputType = input;
  }
  return true;
}


SharedPtr<Core::Data::Ast::ParamPass> TypeOpParsingHandler::prepareThisType(
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  return Core::Data::Ast::ParamPass::create({
    {S("sourceLocation"), sourceLocation},
    {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("ref"))}
    })},
    {S("param"), Spp::Ast::ThisTypeRef::create()}
  });
}


SharedPtr<Core::Data::Ast::ParamPass> TypeOpParsingHandler::prepareAssignmentRetType(
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  return Core::Data::Ast::ParamPass::create({
    {S("sourceLocation"), sourceLocation},
    {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("ref"))}
    })},
    {S("param"), Spp::Ast::ThisTypeRef::create()}
  });
}


SharedPtr<Core::Data::Ast::ParamPass> TypeOpParsingHandler::prepareComparisonRetType(
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  return Core::Data::Ast::ParamPass::create({
    {S("sourceLocation"), sourceLocation},
    {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("Word"))}
    })},
    {S("param"), Core::Data::Ast::IntegerLiteral::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("1"))}
    })}
  });
}


SharedPtr<Core::Data::Ast::Definition> TypeOpParsingHandler::createDefinition(
  Char const *funcName, SharedPtr<Spp::Ast::Function> func, SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  return Core::Data::Ast::Definition::create({
    {S("name"), TiStr(funcName)}
  }, {
    {S("target"), func},
    {S("modifiers"), Core::Data::Ast::List::create({}, {
      Core::Data::Ast::Identifier::create({
        {S("sourceLocation"), sourceLocation},
        {S("value"), TiStr(S("shared"))}
      })
    })}
  });
}

} // namespace
