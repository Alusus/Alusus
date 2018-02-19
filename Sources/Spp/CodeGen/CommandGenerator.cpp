/**
 * @file Spp/CodeGen/CommandGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace CodeGen
{

//==============================================================================
// Initialization Functions

void CommandGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateReturn,
    &this->generateIfStatement,
    &this->generateWhileStatement
  });
}


void CommandGenerator::initBindings()
{
  this->generateReturn = &CommandGenerator::_generateReturn;
  this->generateIfStatement = &CommandGenerator::_generateIfStatement;
  this->generateWhileStatement = &CommandGenerator::_generateWhileStatement;
}


//==============================================================================
// Code Generation Functions

Bool CommandGenerator::_generateReturn(
  TiObject *self, Spp::Ast::ReturnStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get this function's return type.
  Ast::Function *function = Core::Data::findOwner<Ast::Function>(astNode);
  if (function == 0) {
    throw EXCEPTION(GenericException, STR("Return statement does not belong to a function."));
  }
  Ast::Type *retType = function->traceRetType(cmdGenerator->astHelper);

  auto operand = astNode->getOperand().get();
  if (operand != 0) {
    // Generate the expression to return.
    GenResult operandResult;
    if (!g->generateExpression(operand, tg, tgContext, operandResult)) return false;
    // Cast the returned value, if needed.
    if (!operandResult.astType->isImplicitlyCastableTo(
      retType, cmdGenerator->astHelper, tg->getExecutionContext()
    )) {
      cmdGenerator->noticeStore->add(
        std::make_shared<InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    TioSharedPtr tgCastedValue;
    if (!g->generateCast(
      tg, tgContext, operandResult.astType, retType, operandResult.targetData.get(), tgCastedValue)
    ) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, STR("Invalid cast was unexpectedly found."));
    }
    // Generate the return statement.
    tg->generateReturn(tgContext, getCodeGenData<TiObject>(retType), tgCastedValue.get());
  } else {
    // Make sure return type is void.
    if (!retType->isA<Ast::VoidType>()) {
      cmdGenerator->noticeStore->add(
        std::make_shared<InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    // Generate a void return statement.
    tg->generateReturn(tgContext, getCodeGenData<TiObject>(retType), 0);
  }

  return true;
}


Bool CommandGenerator::_generateIfStatement(
  TiObject *self, Spp::Ast::IfStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  auto ifBody = astNode->getIfBody().get();
  auto elseBody = astNode->getElseBody().get();

  TioSharedPtr conditionTgContext;
  TioSharedPtr ifTgContext;
  TioSharedPtr elseTgContext;
  if (elseBody != 0) {
    if (!tg->prepareIfStatement(tgContext, conditionTgContext, ifTgContext, &elseTgContext)) return false;
  } else {
    if (!tg->prepareIfStatement(tgContext, conditionTgContext, ifTgContext, 0)) return false;
  }

  // Generate condition.
  GenResult conditionResult;
  if (!g->generateExpression(
    astNode->getCondition().get(), tg, conditionTgContext.get(), conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, tg, conditionTgContext.get(), astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate ifBody.
  if (ifBody->isDerivedFrom<Ast::Block>()) {
    setCodeGenData(ifBody, ifTgContext);
    if (!g->generateStatements(static_cast<Ast::Block*>(ifBody), tg, ifTgContext.get())) {
      return false;
    }
  } else {
    if (!g->generateStatement(ifBody, tg, ifTgContext.get())) {
      return false;
    }
  }

  // Generate elseBody, if needed.
  if (elseBody != 0) {
    if (elseBody->isDerivedFrom<Ast::Block>()) {
      setCodeGenData(elseBody, elseTgContext);
      if (!g->generateStatements(static_cast<Ast::Block*>(elseBody), tg, elseTgContext.get())) {
        return false;
      }
    } else {
      if (!g->generateStatement(elseBody, tg, elseTgContext.get())) {
        return false;
      }
    }
  }

  return tg->finishIfStatement(
    tgContext, conditionTgContext.get(), castResult.get(), ifTgContext.get(), elseTgContext.get()
  );
}


Bool CommandGenerator::_generateWhileStatement(
  TiObject *self, Spp::Ast::WhileStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  TioSharedPtr conditionTgContext;
  TioSharedPtr bodyTgContext;
  if (!tg->prepareWhileStatement(tgContext, conditionTgContext, bodyTgContext)) return false;

  // Generate the condition.
  GenResult conditionResult;
  if (!g->generateExpression(
    astNode->getCondition().get(), tg, conditionTgContext.get(), conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, tg, conditionTgContext.get(), astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate body.
  auto body = astNode->getBody().get();
  if (body->isDerivedFrom<Ast::Block>()) {
    setCodeGenData(body, bodyTgContext);
    if (!g->generateStatements(static_cast<Ast::Block*>(body), tg, bodyTgContext.get())) {
      return false;
    }
  } else {
    if (!g->generateStatement(body, tg, bodyTgContext.get())) {
      return false;
    }
  }

  return tg->finishWhileStatement(
    tgContext, conditionTgContext.get(), castResult.get(), bodyTgContext.get()
  );
}


//==============================================================================
// Helper Functions

Bool CommandGenerator::castCondition(
  Generation *g, TargetGeneration *tg, TiObject *tgContext, Core::Basic::TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  if (!g->generateCast(tg, tgContext, astType, this->astHelper->getBoolType(), tgValue, result)) {
    this->noticeStore->add(
      std::make_shared<InvalidConditionValueNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} } // namespace
