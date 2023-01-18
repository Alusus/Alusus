/**
 * @file Spp/Handlers/TypeHandlersParsingHandler.cpp
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Handlers
{

void TypeHandlersParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto data = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(data != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(data);
  ASSERT(exprMetadata != 0);

  // Is this a template?
  SharedPtr<Core::Data::Ast::List> tmpltArgs;
  if (data->getCount() > 1) {
    auto bracket = data->get(1).ti_cast_get<Core::Data::Ast::Bracket>();
    if (bracket != 0 && bracket->getType() == Core::Data::Ast::BracketType::SQUARE) {
      if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
      data->remove(1);
    }
  }

  if (data->getCount() < 2) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  for (Int i = 1; i < data->getCount(); ++i) {
    if (data->get(i) == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
  }

  Int bodyIndex = 2;
  Mode mode = Mode::FUNCTION;

  if (data->getCount() > bodyIndex) {
    auto obj = data->getElement(bodyIndex);
    auto token = ti_cast<Core::Data::Ast::Token>(obj);
    if (token != 0) {
      if (token->getText() == S("as_ptr")) {
        mode = Mode::PTR_DEF;
        ++bodyIndex;
      } else if (token->getText() == S("set_ptr")) {
        mode = Mode::PTR_SET;
        ++bodyIndex;
      }
    }
  }

  if (tmpltArgs != 0 && mode != Mode::FUNCTION) {
    state->addNotice(
      newSrdObj<Spp::Notices::InvalidTemplateHandlerStatementNotice>(exprMetadata->findSourceLocation())
    );
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  TioSharedPtr rawBody = 0;
  if (data->getCount() > bodyIndex) {
    rawBody = data->get(bodyIndex);
  }

  SharedPtr<Spp::Ast::Block> body;
  if (rawBody != 0) {
    body = this->prepareBody(rawBody);
  }

  SharedPtr<Core::Data::Ast::Definition> def;

  auto expr = data->getElement(1);
  TioSharedPtr retType;
  auto linkOp = ti_cast<Core::Data::Ast::LinkOperator>(expr);
  if (linkOp != 0 && (linkOp->getType() == S(":") || linkOp->getType() == S("=>"))) {
    expr = linkOp->getFirst().get();
    retType = linkOp->getSecond();
  }

  Bool success = true;
  if (expr->isDerivedFrom<Core::Data::Ast::AssignmentOperator>()) {
    success = this->createAssignmentHandler(
      state, static_cast<Core::Data::Ast::AssignmentOperator*>(expr), body, retType, mode
    );
  } else if (expr->isDerivedFrom<Core::Data::Ast::ComparisonOperator>()) {
    success = this->createComparisonHandler(
      state, static_cast<Core::Data::Ast::ComparisonOperator*>(expr), body, retType, mode
    );
  } else if (expr->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(expr);
    if (paramPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      success = this->createParensOpHandler(state, paramPass, body, retType, mode);
    } else {
      state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(exprMetadata->findSourceLocation()));
      success = false;
    }
  } else if (expr->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Data::Ast::LinkOperator*>(expr);
    success = this->createReadHandler(state, linkOp, body, retType, mode);
  } else if (expr->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(expr);
    success = this->createInfixOpHandler(state, infixOp, body, retType, mode);
  } else if (expr->isDerivedFrom<Spp::Ast::InitOp>()) {
    if (mode == Mode::FUNCTION) {
      success = this->createInitOpHandler(state, static_cast<Spp::Ast::InitOp*>(expr), body);
    } else {
      state->addNotice(newSrdObj<Spp::Notices::PtrBasedInitOpNotice>(exprMetadata->findSourceLocation()));
      success = false;
    }
  } else if (expr->isDerivedFrom<Spp::Ast::TerminateOp>()) {
    success = this->createTerminateOpHandler(state, static_cast<Spp::Ast::TerminateOp*>(expr), body, mode);
  } else if (expr->isDerivedFrom<Spp::Ast::CastOp>()) {
    success = this->createCastHandler(state, static_cast<Spp::Ast::CastOp*>(expr), body, mode);
  } else {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(exprMetadata->findSourceLocation()));
    success = false;
  }

  if (success) {
    if (tmpltArgs != 0) {
      auto def = state->getData().s_cast_get<Core::Data::Ast::Definition>();
      def->setTarget(Ast::Template::create({}, {
        { S("varDefs"), tmpltArgs },
        { S("body"), def->getTarget() }
      }));
    }
  } else {
    state->setData(SharedPtr<TiObject>(0));
  }
}


SharedPtr<Spp::Ast::Block> TypeHandlersParsingHandler::prepareBody(TioSharedPtr const &stmt)
{
  SharedPtr<Spp::Ast::Block> body;
  if (stmt->isDerivedFrom<Spp::Ast::Block>()) {
    body = stmt.s_cast<Spp::Ast::Block>();
  } else {
    body = Spp::Ast::Block::create();
    body->setSourceLocation(Core::Data::Ast::findSourceLocation(stmt.get()));
    body->add(stmt);
  }
  return body;
}


Bool TypeHandlersParsingHandler::createAssignmentHandler(
  Processing::ParserState *state, Core::Data::Ast::AssignmentOperator *assignmentOp,
  SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  Core::Data::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, assignmentOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = assignmentOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, assignmentOp->getSecond(), inputName, inputType)) return false;

  // Prepare ret type.
  TioSharedPtr returnType = retType;
  if (returnType == 0) {
    if (propIdentifier != 0) {
      // By default we'll consider the return type to be of the same type as the input.
      returnType = Core::Data::Ast::clone(inputType.get());
    } else {
      returnType = Ast::Data::Ast::clone(thisType.get(), assignmentOp->findSourceLocation().get());
      // Attach a return statement automatically.
      if (body != 0) {
        body->add(Spp::Ast::ReturnStatement::create({}, {
          {S("operand"), Core::Data::Ast::Identifier::create({
            {S("value"), TiStr(S("this"))}
          })}
        }));
      }
    }
  }

  auto def = this->createBinaryOpFunction(
    state, funcName, op, thisType, inputName,  inputType,
    returnType, body, assignmentOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createComparisonHandler(
  Processing::ParserState *state, Core::Data::Ast::ComparisonOperator *comparisonOp,
  SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  Core::Data::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, comparisonOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = comparisonOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, comparisonOp->getSecond(), inputName, inputType)) return false;

  // Prepare ret type.
  TioSharedPtr returnType = retType;
  if (returnType == 0) {
    returnType = this->prepareComparisonRetType(comparisonOp->findSourceLocation());
  }

  auto def = this->createBinaryOpFunction(
    state, funcName, op, thisType, inputName,  inputType, returnType, body, comparisonOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createInfixOpHandler(
  Processing::ParserState *state, Core::Data::Ast::InfixOperator *infixOp,
  SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  Core::Data::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, infixOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = infixOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  TioSharedPtr inputType;
  if (!this->prepareInputArg(state, infixOp->getSecond(), inputName, inputType)) return false;

  auto def = this->createBinaryOpFunction(
    state, funcName, op, thisType, inputName,  inputType, retType, body, infixOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createReadHandler(
  Processing::ParserState *state, Core::Data::Ast::LinkOperator *linkOp,
  SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
) {
  if (linkOp->getType() != S(".")) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(linkOp->findSourceLocation()));
    return false;
  }
  Core::Data::Ast::Identifier *thisIdentifier;
  Core::Data::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(state, linkOp, false, thisIdentifier, thisType, propIdentifier)) {
    return false;
  }

  if (retType == 0) {
    state->addNotice(newSrdObj<Spp::Notices::PropGetterMissingRetTypeNotice>(linkOp->findSourceLocation()));
    return false;
  }

  Char const *op = S("");
  Char const *funcName = propIdentifier->getValue().get();

  // Prepare params.
  auto argTypes = Core::Data::Ast::Map::create();
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    state, funcName, op, true, argTypes, retType, body, linkOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createInitOpHandler(
  Processing::ParserState *state, Spp::Ast::InitOp *initOp,
  SharedPtr<Spp::Ast::Block> const &body
) {
  // Verify operand.
  auto operand = initOp->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(newSrdObj<Spp::Notices::OpTargetNotThisNotice>(initOp->findSourceLocation()));
    return false;
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
    if (!this->prepareInputArg(state, inputDef, inputName, inputType)) return false;
    if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Data::Ast::findSourceLocation(inputDef.get())
      ));
      return false;
    }
    argTypes->add(inputName, inputType);
  }

  auto def = this->createFunction(
    state, S("~init"), S("~init"), true, argTypes, TioSharedPtr::null, body, initOp->findSourceLocation(),
    Mode::FUNCTION
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createTerminateOpHandler(
  Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
  SharedPtr<Spp::Ast::Block> const &body, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisIdentifierAndType(state, terminateOp->getOperand().get(), false, thisIdentifier, thisType)) {
    return false;
  }

  // Prepare params.
  auto argTypes = Core::Data::Ast::Map::create();
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    state, S("~terminate"), S("~terminate"), true, argTypes, TioSharedPtr::null, body,
    terminateOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createCastHandler(
  Processing::ParserState *state, Spp::Ast::CastOp *castOp,
  SharedPtr<Spp::Ast::Block> const &body, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisIdentifierAndType(state, castOp->getOperand().get(), false, thisIdentifier, thisType)) {
    return false;
  }

  // Prepare return type.
  auto retType = castOp->getTargetType();
  if (retType == 0) {
    state->addNotice(newSrdObj<Spp::Notices::CastOpMissingTypeNotice>(castOp->findSourceLocation()));
    return false;
  }

  // Prepare params.
  auto argTypes = Core::Data::Ast::Map::create();
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    state, S("~cast"), S("~cast"), true, argTypes, retType, body, castOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createParensOpHandler(
  Processing::ParserState *state, Core::Data::Ast::ParamPass *parensOp,
  SharedPtr<Spp::Ast::Block> const &body, TioSharedPtr const &retType, Mode mode
) {
  Core::Data::Ast::Identifier *thisIdentifier;
  Core::Data::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Data::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, parensOp->getOperand().get(), true, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = S("()");
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare params.
  auto param = parensOp->getParam();
  Core::Data::Ast::List tempParams;
  auto params = ti_cast<Core::Data::Ast::List>(param.get());
  if (params == 0) {
    if (param != 0) tempParams.add(param);
    params = &tempParams;
  }
  auto argTypes = Core::Data::Ast::Map::create();
  if (thisType != 0) argTypes->add(S("this"), thisType);
  for (Int i = 0; i < params->getCount(); ++i) {
    Char const *inputName;
    TioSharedPtr inputType;
    auto inputDef = params->get(i);
    if (!this->prepareInputArg(state, inputDef, inputName, inputType, params->getCount() == 1 ? S("value") : S(""))) {
      return false;
    }
    if (inputName == S("")) {
      inputName = S("__");
      inputName += (LongInt)i;
    } else if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Data::Ast::findSourceLocation(inputDef.get())
      ));
      return false;
    }
    argTypes->add(inputName, inputType);
  }

  auto def = this->createFunction(
    state, funcName, op, thisType != 0, argTypes, retType, body, parensOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


TioSharedPtr TypeHandlersParsingHandler::createBinaryOpFunction(
  Processing::ParserState *state, Char const *funcName, Char const *op, TioSharedPtr const &thisType,
  Char const *inputName, TioSharedPtr const &inputType, TioSharedPtr const &retType, TioSharedPtr const &body,
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation, Mode mode
) {
  // Prepare arg types map.
  auto argTypes = Core::Data::Ast::Map::create();
  argTypes->add(S("this"), thisType);
  argTypes->add(inputName, inputType);

  return this->createFunction(state, funcName, op, true, argTypes, retType, body, sourceLocation, mode);
}


TioSharedPtr TypeHandlersParsingHandler::createFunction(
  Processing::ParserState *state, Char const *funcName, Char const *op, Bool member,
  SharedPtr<Core::Data::Ast::Map> const argTypes, TioSharedPtr const &retType, TioSharedPtr const &body,
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation, Mode mode
) {
  // Create the function type.
  auto funcType = Spp::Ast::FunctionType::create({
    {S("member"), TiBool(member)}
  }, {
    {S("argTypes"), argTypes},
    {S("retType"), retType}
  });
  if (!processFunctionArgPacks(funcType.get(), state->getNoticeStore())) {
    return SharedPtr<Core::Data::Ast::Definition>::null;
  }

  if (mode != Mode::FUNCTION) {
    // Create the function ptr.
    auto funcPtrType = Core::Data::Ast::ParamPass::create({
      {S("sourceLocation"), sourceLocation},
      {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
    }, {
      {S("operand"), Core::Data::Ast::Identifier::create({
        {S("sourceLocation"), sourceLocation},
        {S("value"), TiStr(S("ptr"))}
      })},
      {S("param"), funcType}
    });
    auto mergeList = Core::Data::Ast::MergeList::create();
    if (mode == Mode::PTR_DEF) {
      mergeList->add(this->createDefinition(funcName, op, funcPtrType, sourceLocation));
    }
    if (body != 0) {
      // Create the function.
      auto funcTypeClone = Core::Data::Ast::clone(funcType.get(), sourceLocation.get());
      funcTypeClone->getArgTypes()->set(0, this->prepareThisType(sourceLocation));
      auto func = Spp::Ast::Function::create({}, {
        {S("type"), funcTypeClone},
        {S("body"), body}
      });
      // Assign the function pointer.
      mergeList->add(Core::Data::Ast::AssignmentOperator::create({
        {S("sourceLocation"), sourceLocation},
        {S("type"), TiStr(S("="))}
      }, {
        {S("first"), Core::Data::Ast::Identifier::create({
          {S("sourceLocation"), sourceLocation},
          {S("value"), TiStr(funcName)}
        })},
        {S("second"), Spp::Ast::CastOp::create({
          {S("sourceLocation"), sourceLocation},
        }, {
          {S("operand"), func},
          {S("targetType"), Core::Data::Ast::ParamPass::create({
            {S("sourceLocation"), sourceLocation},
            {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
          }, {
            {S("operand"), Core::Data::Ast::Identifier::create({
              {S("sourceLocation"), sourceLocation},
              {S("value"), TiStr(S("ptr"))}
            })},
            {S("param"), Core::Data::Ast::clone(funcType.get(), sourceLocation.get())}
          })}
        })}
      }));
    }
    return mergeList;
  } else {
    // Create the function.
    auto func = Spp::Ast::Function::create({}, {
      {S("type"), funcType},
      {S("body"), body}
    });
    // Create the definition.
    return this->createDefinition(funcName, op, func, sourceLocation);
  }
}


Bool TypeHandlersParsingHandler::prepareInputArg(
  Processing::ParserState *state, TioSharedPtr input, Char const *&inputName, TioSharedPtr &inputType,
  Char const *defaultName
) {
  if (input->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = input.s_cast_get<Core::Data::Ast::Bracket>();
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
      input = bracket->getOperand();
    } else {
      state->addNotice(newSrdObj<Spp::Notices::InvalidUseOfSquareBracketNotice>(
        Core::Data::Ast::findSourceLocation(input.get())
      ));
      return false;
    }
  }
  if (input->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOperator = input.s_cast_get<Core::Data::Ast::LinkOperator>();
    auto inputNameId = linkOperator->getFirst().ti_cast<Core::Data::Ast::Identifier>();
    if (inputNameId == 0 || inputNameId->getValue() == S("this")) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(
        Core::Data::Ast::findSourceLocation(input.get())
      ));
      return false;
    }
    inputName = inputNameId->getValue().get();
    inputType = linkOperator->getSecond();
  } else {
    inputName = defaultName;
    inputType = input;
  }
  return true;
}


Bool TypeHandlersParsingHandler::getThisAndPropIdentifiers(
  Processing::ParserState *state, TiObject *astNode, Bool allowThisType,
  Core::Data::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Data::Ast::ParamPass> &thisType,
  Core::Data::Ast::Identifier *&propIdentifier
) {
  if (astNode->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Data::Ast::LinkOperator*>(astNode);
    if (linkOp->getType() == S(".")) {
      if (!this->getThisIdentifierAndType(state, linkOp->getFirst().get(), false, thisIdentifier, thisType)) return false;
      propIdentifier = linkOp->getSecond().ti_cast_get<Core::Data::Ast::Identifier>();
      if (propIdentifier == 0) {
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidPropIdentifierNotice>(Core::Data::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      return true;
    }
  }
  propIdentifier = 0;
  return this->getThisIdentifierAndType(state, astNode, allowThisType, thisIdentifier, thisType);
}


Bool TypeHandlersParsingHandler::getThisIdentifierAndType(
  Processing::ParserState *state, TiObject *astNode, Bool allowThisType,
  Core::Data::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Data::Ast::ParamPass> &thisType
) {
  if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    thisIdentifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    if (thisIdentifier->getValue() != S("this")) {
      state->addNotice(
        newSrdObj<Spp::Notices::OpTargetNotThisNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      state->setData(SharedPtr<TiObject>(0));
      return false;
    }
    thisType = this->prepareThisType(thisIdentifier->findSourceLocation());
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto linkOp = static_cast<Core::Data::Ast::Bracket*>(astNode)->getOperand()
      .ti_cast_get<Core::Data::Ast::LinkOperator>();
    if (linkOp != 0 && linkOp->getType() == S(":")) {
      thisIdentifier = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
      if (thisIdentifier == 0 || thisIdentifier->getValue() != S("this")) {
        state->addNotice(
          newSrdObj<Spp::Notices::OpTargetNotThisNotice>(Core::Data::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      thisType = Core::Data::Ast::ParamPass::create({
        {S("sourceLocation"), Core::Data::Ast::findSourceLocation(linkOp->getSecond().get())},
        {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
      }, {
        {S("operand"), Core::Data::Ast::Identifier::create({
          {S("sourceLocation"), Core::Data::Ast::findSourceLocation(linkOp->getSecond().get())},
          {S("value"), TiStr(S("ref"))}
        })},
        {S("param"), linkOp->getSecond()}
      });
    } else {
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::ThisTypeRef>() && allowThisType) {
    thisIdentifier = 0;
    thisType.reset();
  } else {
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
  }
  return true;
}


SharedPtr<Core::Data::Ast::ParamPass> TypeHandlersParsingHandler::prepareThisType(
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


SharedPtr<Core::Data::Ast::ParamPass> TypeHandlersParsingHandler::prepareComparisonRetType(
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


SharedPtr<Core::Data::Ast::Definition> TypeHandlersParsingHandler::createDefinition(
  Char const *name, Char const *op, TioSharedPtr target,
  SharedPtr<Core::Data::SourceLocation> const &sourceLocation
) {
  if (op != 0) {
    return Core::Data::Ast::Definition::create({
      {S("name"), TiStr(name)}
    }, {
      {S("target"), target},
      {S("modifiers"), Core::Data::Ast::List::create({}, {
        Core::Data::Ast::ParamPass::create({
          {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
        }, {
          {S("operand"), Core::Data::Ast::Identifier::create({ {S("value"), TiStr(S("operation"))} })},
          {S("param"), Core::Data::Ast::StringLiteral::create({ {S("value"), TiStr(op)} })}
        })
      })}
    });
  } else {
    return Core::Data::Ast::Definition::create({
      {S("name"), TiStr(name)}
    }, {
      {S("target"), target}
    });
  }
}


Bool TypeHandlersParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  if (this->processExpnameModifier(state, modifierData)) return true;
  else return this->processUnknownModifier(state, modifierData);
}


Bool TypeHandlersParsingHandler::processExpnameModifier(
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
  auto data = state->getData(levelOffset).get();
  if (data == 0) return false;

  // Find the function to update.
  Spp::Ast::Function *function = 0;
  if (data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(data);
    function = def->getTarget().ti_cast_get<Spp::Ast::Function>();
  } else if (data->isDerivedFrom<Core::Data::Ast::MergeList>()) {
    auto mergeList = static_cast<Core::Data::Ast::MergeList*>(data);
    for (Int i = 0; i < mergeList->getCount(); ++i) {
      auto def = ti_cast<Core::Data::Ast::Definition>(mergeList->getElement(i));
      if (def != 0) {
        function = def->getTarget().ti_cast_get<Spp::Ast::Function>();
        if (function != 0) break;
      }
    }
  }
  if (function == 0) return false;

  function->setName(param->getValue());
  return true;
}


Bool TypeHandlersParsingHandler::processUnknownModifier(
  Core::Processing::ParserState *state, TioSharedPtr const &modifierData
) {
  // Add an unknown modifier to the definition.
  auto symbolDef = state->refTopProdLevel().getProd();
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  if (data == 0) return false;

  Core::Data::Ast::Definition *def = 0;
  if (data->isDerivedFrom<Core::Data::Ast::Definition>()) {
    def = static_cast<Core::Data::Ast::Definition*>(data);
  } else if (data->isDerivedFrom<Core::Data::Ast::MergeList>()) {
    auto mergeList = static_cast<Core::Data::Ast::MergeList*>(data);
    for (Int i = 0; i < mergeList->getCount(); ++i) {
      def = ti_cast<Core::Data::Ast::Definition>(mergeList->getElement(i));
      if (def != 0) break;
    }
  }

  if (def != 0) {
    Core::Data::Ast::translateModifier(symbolDef, modifierData.get());
    def->addModifier(modifierData);
    return true;
  } else {
    return false;
  }
}

} // namespace
