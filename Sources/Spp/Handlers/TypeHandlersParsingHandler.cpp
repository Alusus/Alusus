/**
 * @file Spp/Handlers/TypeHandlersParsingHandler.cpp
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

void TypeHandlersParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto data = state->getData().ti_cast_get<Core::Ast::List>();
  ASSERT(data != 0);

  // Is this a template?
  SharedPtr<Core::Ast::List> tmpltArgs;
  if (data->getCount() > 1) {
    auto bracket = data->get(1).ti_cast_get<Core::Ast::Bracket>();
    if (bracket != 0 && bracket->getType() == Core::Ast::BracketType::SQUARE) {
      if (!parseTemplateArgs(state, bracket, tmpltArgs)) {
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
      data->remove(1);
    }
  }

  if (data->getCount() < 2) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(data->findSourceLocation()));
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }
  for (Int i = 1; i < data->getCount(); ++i) {
    if (data->get(i) == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(data->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
  }

  Int bodyIndex = 2;
  Mode mode = Mode::FUNCTION;

  if (data->getCount() > bodyIndex) {
    auto obj = data->getElement(bodyIndex);
    auto token = ti_cast<Core::Ast::Token>(obj);
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
      newSrdObj<Spp::Notices::InvalidTemplateHandlerStatementNotice>(data->findSourceLocation())
    );
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  SharedPtr<Core::Ast::Node> rawBody = 0;
  if (data->getCount() > bodyIndex) {
    rawBody = data->get(bodyIndex);
  }

  SharedPtr<Spp::Ast::Block> body;
  if (rawBody != 0) {
    body = this->prepareBody(rawBody);
  }

  SharedPtr<Core::Ast::Definition> def;

  auto expr = data->getElement(1);
  SharedPtr<Core::Ast::Node> retType;
  auto linkOp = ti_cast<Core::Ast::LinkOperator>(expr);
  if (linkOp != 0 && (linkOp->getType() == S(":") || linkOp->getType() == S("=>"))) {
    expr = linkOp->getFirst().get();
    retType = linkOp->getSecond();
  }

  Bool success = true;
  if (expr->isDerivedFrom<Core::Ast::AssignmentOperator>()) {
    success = this->createAssignmentHandler(
      state, static_cast<Core::Ast::AssignmentOperator*>(expr), body, retType, mode
    );
  } else if (expr->isDerivedFrom<Core::Ast::ComparisonOperator>()) {
    success = this->createComparisonHandler(
      state, static_cast<Core::Ast::ComparisonOperator*>(expr), body, retType, mode
    );
  } else if (expr->isDerivedFrom<Core::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Ast::ParamPass*>(expr);
    if (paramPass->getType() == Core::Ast::BracketType::ROUND) {
      success = this->createParensOpHandler(state, paramPass, body, retType, mode);
    } else {
      state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(data->findSourceLocation()));
      success = false;
    }
  } else if (expr->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Ast::LinkOperator*>(expr);
    success = this->createReadHandler(state, linkOp, body, retType, mode);
  } else if (expr->isDerivedFrom<Core::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Ast::InfixOperator*>(expr);
    success = this->createInfixOpHandler(state, infixOp, body, retType, mode);
  } else if (expr->isDerivedFrom<Spp::Ast::InitOp>()) {
    if (mode == Mode::FUNCTION) {
      success = this->createInitOpHandler(state, static_cast<Spp::Ast::InitOp*>(expr), body);
    } else {
      state->addNotice(newSrdObj<Spp::Notices::PtrBasedInitOpNotice>(data->findSourceLocation()));
      success = false;
    }
  } else if (expr->isDerivedFrom<Spp::Ast::TerminateOp>()) {
    success = this->createTerminateOpHandler(state, static_cast<Spp::Ast::TerminateOp*>(expr), body, mode);
  } else if (expr->isDerivedFrom<Spp::Ast::CastOp>()) {
    success = this->createCastHandler(state, static_cast<Spp::Ast::CastOp*>(expr), body, mode);
  } else {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(data->findSourceLocation()));
    success = false;
  }

  if (success) {
    if (tmpltArgs != 0) {
      auto def = state->getData().s_cast_get<Core::Ast::Definition>();
      def->setTarget(Ast::Template::create({}, {
        { S("varDefs"), tmpltArgs },
        { S("body"), def->getTarget() }
      }));
    }
  } else {
    state->setData(SharedPtr<Core::Ast::Node>(0));
  }
}


SharedPtr<Spp::Ast::Block> TypeHandlersParsingHandler::prepareBody(SharedPtr<Core::Ast::Node> const &stmt)
{
  SharedPtr<Spp::Ast::Block> body;
  if (stmt->isDerivedFrom<Spp::Ast::Block>()) {
    body = stmt.s_cast<Spp::Ast::Block>();
  } else {
    body = Spp::Ast::Block::create();
    body->setSourceLocation(Core::Ast::findSourceLocation(stmt.get()));
    body->add(stmt);
  }
  return body;
}


Bool TypeHandlersParsingHandler::createAssignmentHandler(
  Processing::ParserState *state, Core::Ast::AssignmentOperator *assignmentOp,
  SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  Core::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, assignmentOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = assignmentOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  SharedPtr<Core::Ast::Node> inputType;
  if (!this->prepareInputArg(state, assignmentOp->getSecond(), inputName, inputType)) return false;

  // Prepare ret type.
  SharedPtr<Core::Ast::Node> returnType = retType;
  if (returnType == 0) {
    if (propIdentifier != 0) {
      // By default we'll consider the return type to be of the same type as the input.
      returnType = Core::Ast::clone(inputType.get());
    } else {
      returnType = Ast::Ast::clone(thisType.get(), assignmentOp->findSourceLocation().get());
      // Attach a return statement automatically.
      if (body != 0) {
        body->add(Spp::Ast::ReturnStatement::create({}, {
          {S("operand"), Core::Ast::Identifier::create({
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
  Processing::ParserState *state, Core::Ast::ComparisonOperator *comparisonOp,
  SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  Core::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, comparisonOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = comparisonOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  SharedPtr<Core::Ast::Node> inputType;
  if (!this->prepareInputArg(state, comparisonOp->getSecond(), inputName, inputType)) return false;

  // Prepare ret type.
  SharedPtr<Core::Ast::Node> returnType = retType;
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
  Processing::ParserState *state, Core::Ast::InfixOperator *infixOp,
  SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  Core::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, infixOp->getFirst().get(), false, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = infixOp->getType();
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare name and type of input var.
  Char const *inputName;
  SharedPtr<Core::Ast::Node> inputType;
  if (!this->prepareInputArg(state, infixOp->getSecond(), inputName, inputType)) return false;

  auto def = this->createBinaryOpFunction(
    state, funcName, op, thisType, inputName,  inputType, retType, body, infixOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createReadHandler(
  Processing::ParserState *state, Core::Ast::LinkOperator *linkOp,
  SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
) {
  if (linkOp->getType() != S(".")) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(linkOp->findSourceLocation()));
    return false;
  }
  Core::Ast::Identifier *thisIdentifier;
  Core::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
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
  auto argTypes = Core::Ast::Map::create();
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
  auto operand = initOp->getOperand().ti_cast_get<Core::Ast::Identifier>();
  if (operand == 0 || operand->getValue() != S("this")) {
    state->addNotice(newSrdObj<Spp::Notices::OpTargetNotThisNotice>(initOp->findSourceLocation()));
    return false;
  }

  // Prepare params.
  auto param = initOp->getParam();
  Core::Ast::List tempParams;
  auto params = ti_cast<Core::Ast::List>(param.get());
  if (params == 0) {
    if (param != 0) tempParams.add(param);
    params = &tempParams;
  }
  auto argTypes = Core::Ast::Map::create();
  auto thisType = this->prepareThisType(operand->findSourceLocation());
  argTypes->add(S("this"), thisType);
  for (Int i = 0; i < params->getCount(); ++i) {
    Char const *inputName;
    SharedPtr<Core::Ast::Node> inputType;
    auto inputDef = params->get(i);
    if (!this->prepareInputArg(state, inputDef, inputName, inputType)) return false;
    if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Ast::findSourceLocation(inputDef.get())
      ));
      return false;
    }
    argTypes->add(inputName, inputType);
  }

  auto def = this->createFunction(
    state, S("~init"), S("~init"), true, argTypes, SharedPtr<Core::Ast::Node>::null, body, initOp->findSourceLocation(),
    Mode::FUNCTION
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createTerminateOpHandler(
  Processing::ParserState *state, Spp::Ast::TerminateOp *terminateOp,
  SharedPtr<Spp::Ast::Block> const &body, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
  if (!this->getThisIdentifierAndType(state, terminateOp->getOperand().get(), false, thisIdentifier, thisType)) {
    return false;
  }

  // Prepare params.
  auto argTypes = Core::Ast::Map::create();
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    state, S("~terminate"), S("~terminate"), true, argTypes, SharedPtr<Core::Ast::Node>::null, body,
    terminateOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createCastHandler(
  Processing::ParserState *state, Spp::Ast::CastOp *castOp,
  SharedPtr<Spp::Ast::Block> const &body, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
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
  auto argTypes = Core::Ast::Map::create();
  argTypes->add(S("this"), thisType);

  auto def = this->createFunction(
    state, S("~cast"), S("~cast"), true, argTypes, retType, body, castOp->findSourceLocation(), mode
  );
  state->setData(def);
  return true;
}


Bool TypeHandlersParsingHandler::createParensOpHandler(
  Processing::ParserState *state, Core::Ast::ParamPass *parensOp,
  SharedPtr<Spp::Ast::Block> const &body, SharedPtr<Core::Ast::Node> const &retType, Mode mode
) {
  Core::Ast::Identifier *thisIdentifier;
  Core::Ast::Identifier *propIdentifier;
  SharedPtr<Core::Ast::ParamPass> thisType;
  if (!this->getThisAndPropIdentifiers(
    state, parensOp->getOperand().get(), true, thisIdentifier, thisType, propIdentifier
  )) {
    return false;
  }

  Char const *op = S("()");
  Char const *funcName = propIdentifier == 0 ? op : propIdentifier->getValue().get();

  // Prepare params.
  auto param = parensOp->getParam();
  Core::Ast::List tempParams;
  auto params = ti_cast<Core::Ast::List>(param.get());
  if (params == 0) {
    if (param != 0) tempParams.add(param);
    params = &tempParams;
  }
  auto argTypes = Core::Ast::Map::create();
  if (thisType != 0) argTypes->add(S("this"), thisType);
  for (Int i = 0; i < params->getCount(); ++i) {
    Char const *inputName;
    SharedPtr<Core::Ast::Node> inputType;
    auto inputDef = params->get(i);
    if (!this->prepareInputArg(state, inputDef, inputName, inputType, params->getCount() == 1 ? S("value") : S(""))) {
      return false;
    }
    if (inputName == S("")) {
      inputName = S("__");
      inputName += (LongInt)i;
    } else if (argTypes->findIndex(inputName) != -1) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidFunctionArgNameNotice>(
        Core::Ast::findSourceLocation(inputDef.get())
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


SharedPtr<Core::Ast::Node> TypeHandlersParsingHandler::createBinaryOpFunction(
  Processing::ParserState *state, Char const *funcName, Char const *op, SharedPtr<Core::Ast::Node> const &thisType,
  Char const *inputName, SharedPtr<Core::Ast::Node> const &inputType, SharedPtr<Core::Ast::Node> const &retType, SharedPtr<Core::Ast::Node> const &body,
  SharedPtr<Core::Ast::SourceLocation> const &sourceLocation, Mode mode
) {
  // Prepare arg types map.
  auto argTypes = Core::Ast::Map::create();
  argTypes->add(S("this"), thisType);
  argTypes->add(inputName, inputType);

  return this->createFunction(state, funcName, op, true, argTypes, retType, body, sourceLocation, mode);
}


SharedPtr<Core::Ast::Node> TypeHandlersParsingHandler::createFunction(
  Processing::ParserState *state, Char const *funcName, Char const *op, Bool member,
  SharedPtr<Core::Ast::Map> const argTypes, SharedPtr<Core::Ast::Node> const &retType, SharedPtr<Core::Ast::Node> const &body,
  SharedPtr<Core::Ast::SourceLocation> const &sourceLocation, Mode mode
) {
  // Create the function type.
  auto funcType = Spp::Ast::FunctionType::create({
    {S("member"), TiBool(member)}
  }, {
    {S("argTypes"), argTypes},
    {S("retType"), retType}
  });
  if (!processFunctionArgPacks(funcType.get(), state->getNoticeStore())) {
    return SharedPtr<Core::Ast::Definition>::null;
  }

  if (mode != Mode::FUNCTION) {
    // Create the function ptr.
    auto funcPtrType = Core::Ast::ParamPass::create({
      {S("sourceLocation"), sourceLocation},
      {S("type"), Core::Ast::BracketType(Core::Ast::BracketType::SQUARE)}
    }, {
      {S("operand"), Core::Ast::Identifier::create({
        {S("sourceLocation"), sourceLocation},
        {S("value"), TiStr(S("ptr"))}
      })},
      {S("param"), funcType}
    });
    auto mergeList = Core::Ast::MergeList::create();
    if (mode == Mode::PTR_DEF) {
      mergeList->add(this->createDefinition(funcName, op, funcPtrType, sourceLocation));
    }
    if (body != 0) {
      // Create the function.
      auto funcTypeClone = Core::Ast::clone(funcType.get(), sourceLocation.get());
      funcTypeClone->getArgTypes()->set(0, this->prepareThisType(sourceLocation));
      auto func = Spp::Ast::Function::create({}, {
        {S("type"), funcTypeClone},
        {S("body"), body}
      });
      // Assign the function pointer.
      mergeList->add(Core::Ast::AssignmentOperator::create({
        {S("sourceLocation"), sourceLocation},
        {S("type"), TiStr(S("="))}
      }, {
        {S("first"), Core::Ast::Identifier::create({
          {S("sourceLocation"), sourceLocation},
          {S("value"), TiStr(funcName)}
        })},
        {S("second"), Spp::Ast::CastOp::create({
          {S("sourceLocation"), sourceLocation},
        }, {
          {S("operand"), func},
          {S("targetType"), Core::Ast::ParamPass::create({
            {S("sourceLocation"), sourceLocation},
            {S("type"), Core::Ast::BracketType(Core::Ast::BracketType::SQUARE)}
          }, {
            {S("operand"), Core::Ast::Identifier::create({
              {S("sourceLocation"), sourceLocation},
              {S("value"), TiStr(S("ptr"))}
            })},
            {S("param"), Core::Ast::clone(funcType.get(), sourceLocation.get())}
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
  Processing::ParserState *state, SharedPtr<Core::Ast::Node> input, Char const *&inputName, SharedPtr<Core::Ast::Node> &inputType,
  Char const *defaultName
) {
  if (input->isDerivedFrom<Core::Ast::Bracket>()) {
    auto bracket = input.s_cast_get<Core::Ast::Bracket>();
    if (bracket->getType() == Core::Ast::BracketType::ROUND) {
      input = bracket->getOperand();
    } else {
      state->addNotice(newSrdObj<Spp::Notices::InvalidUseOfSquareBracketNotice>(
        Core::Ast::findSourceLocation(input.get())
      ));
      return false;
    }
  }
  if (input->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOperator = input.s_cast_get<Core::Ast::LinkOperator>();
    auto inputNameId = linkOperator->getFirst().ti_cast<Core::Ast::Identifier>();
    if (inputNameId == 0 || inputNameId->getValue() == S("this")) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(
        Core::Ast::findSourceLocation(input.get())
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
  Processing::ParserState *state, Core::Ast::Node *astNode, Bool allowThisType,
  Core::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Ast::ParamPass> &thisType,
  Core::Ast::Identifier *&propIdentifier
) {
  if (astNode->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Ast::LinkOperator*>(astNode);
    if (linkOp->getType() == S(".")) {
      if (!this->getThisIdentifierAndType(state, linkOp->getFirst().get(), false, thisIdentifier, thisType)) return false;
      propIdentifier = linkOp->getSecond().ti_cast_get<Core::Ast::Identifier>();
      if (propIdentifier == 0) {
        state->addNotice(
          newSrdObj<Spp::Notices::InvalidPropIdentifierNotice>(Core::Ast::findSourceLocation(astNode))
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
  Processing::ParserState *state, Core::Ast::Node *astNode, Bool allowThisType,
  Core::Ast::Identifier *&thisIdentifier, SharedPtr<Core::Ast::ParamPass> &thisType
) {
  if (astNode->isDerivedFrom<Core::Ast::Identifier>()) {
    thisIdentifier = static_cast<Core::Ast::Identifier*>(astNode);
    if (thisIdentifier->getValue() != S("this")) {
      state->addNotice(
        newSrdObj<Spp::Notices::OpTargetNotThisNotice>(Core::Ast::findSourceLocation(astNode))
      );
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return false;
    }
    thisType = this->prepareThisType(thisIdentifier->findSourceLocation());
  } else if (astNode->isDerivedFrom<Core::Ast::Bracket>()) {
    auto linkOp = static_cast<Core::Ast::Bracket*>(astNode)->getOperand()
      .ti_cast_get<Core::Ast::LinkOperator>();
    if (linkOp != 0 && linkOp->getType() == S(":")) {
      thisIdentifier = linkOp->getFirst().ti_cast_get<Core::Ast::Identifier>();
      if (thisIdentifier == 0 || thisIdentifier->getValue() != S("this")) {
        state->addNotice(
          newSrdObj<Spp::Notices::OpTargetNotThisNotice>(Core::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      thisType = Core::Ast::ParamPass::create({
        {S("sourceLocation"), Core::Ast::findSourceLocation(linkOp->getSecond().get())},
        {S("type"), Core::Ast::BracketType(Core::Ast::BracketType::SQUARE)}
      }, {
        {S("operand"), Core::Ast::Identifier::create({
          {S("sourceLocation"), Core::Ast::findSourceLocation(linkOp->getSecond().get())},
          {S("value"), TiStr(S("ref"))}
        })},
        {S("param"), linkOp->getSecond()}
      });
    } else {
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(Core::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::ThisTypeRef>() && allowThisType) {
    thisIdentifier = 0;
    thisType.reset();
  } else {
      state->addNotice(
        newSrdObj<Spp::Notices::InvalidHandlerStatementNotice>(Core::Ast::findSourceLocation(astNode))
      );
      return false;
  }
  return true;
}


SharedPtr<Core::Ast::ParamPass> TypeHandlersParsingHandler::prepareThisType(
  SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
) {
  return Core::Ast::ParamPass::create({
    {S("sourceLocation"), sourceLocation},
    {S("type"), Core::Ast::BracketType(Core::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Ast::Identifier::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("ref"))}
    })},
    {S("param"), Spp::Ast::ThisTypeRef::create()}
  });
}


SharedPtr<Core::Ast::ParamPass> TypeHandlersParsingHandler::prepareComparisonRetType(
  SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
) {
  return Core::Ast::ParamPass::create({
    {S("sourceLocation"), sourceLocation},
    {S("type"), Core::Ast::BracketType(Core::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Ast::Identifier::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("Word"))}
    })},
    {S("param"), Core::Ast::IntegerLiteral::create({
      {S("sourceLocation"), sourceLocation},
      {S("value"), TiStr(S("1"))}
    })}
  });
}


SharedPtr<Core::Ast::Definition> TypeHandlersParsingHandler::createDefinition(
  Char const *name, Char const *op, SharedPtr<Core::Ast::Node> target,
  SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
) {
  if (op != 0) {
    auto def = Core::Ast::Definition::create({
      {S("name"), TiStr(name)}
    }, {
      {S("target"), target}
    });
    def->setMetadata(S("operation"), Core::Ast::StringLiteral::create({ {S("value"), TiStr(op)} }));
    return def;
  } else {
    return Core::Ast::Definition::create({
      {S("name"), TiStr(name)}
    }, {
      {S("target"), target}
    });
  }
}


Bool TypeHandlersParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  return this->processExpnameModifier(state, modifierData);
}


Bool TypeHandlersParsingHandler::processExpnameModifier(
  Core::Processing::ParserState *state, SharedPtr<Core::Ast::Node> const &modifierData
) {
  // Look for expname modifier.
  auto paramPass = modifierData.ti_cast_get<Core::Ast::ParamPass>();
  if (paramPass == 0) return false;
  if (paramPass->getType() != Core::Ast::BracketType::SQUARE) return false;
  auto operand = paramPass->getOperand().ti_cast_get<Core::Ast::Identifier>();
  if (operand == 0) return false;
  if (operand->getValue() != S("expname")) return false;
  auto param = paramPass->getParam().ti_cast_get<Core::Ast::Text>();
  if (param == 0) return false;

  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  if (data == 0) return false;

  // Find the function to update.
  Spp::Ast::Function *function = 0;
  if (data->isDerivedFrom<Core::Ast::Definition>()) {
    auto def = static_cast<Core::Ast::Definition*>(data);
    function = def->getTarget().ti_cast_get<Spp::Ast::Function>();
  } else if (data->isDerivedFrom<Core::Ast::MergeList>()) {
    auto mergeList = static_cast<Core::Ast::MergeList*>(data);
    for (Int i = 0; i < mergeList->getCount(); ++i) {
      auto def = ti_cast<Core::Ast::Definition>(mergeList->getElement(i));
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

} // namespace
