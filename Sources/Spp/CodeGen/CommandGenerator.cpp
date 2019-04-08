/**
 * @file Spp/CodeGen/CommandGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
  Basic::initBindingCaches(this, {
    &this->generateReturnStatement,
    &this->generateIfStatement,
    &this->generateWhileStatement,
    &this->generateForStatement,
    &this->generateContinueStatement,
    &this->generateBreakStatement
  });
}


void CommandGenerator::initBindings()
{
  this->generateReturnStatement = &CommandGenerator::_generateReturnStatement;
  this->generateIfStatement = &CommandGenerator::_generateIfStatement;
  this->generateWhileStatement = &CommandGenerator::_generateWhileStatement;
  this->generateForStatement = &CommandGenerator::_generateForStatement;
  this->generateContinueStatement = &CommandGenerator::_generateContinueStatement;
  this->generateBreakStatement = &CommandGenerator::_generateBreakStatement;
}


//==============================================================================
// Code Generation Functions

Bool CommandGenerator::_generateReturnStatement(
  TiObject *self, Spp::Ast::ReturnStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get this function's return type.
  Ast::Function *function = Core::Data::findOwner<Ast::Function>(astNode);
  if (function == 0) {
    throw EXCEPTION(GenericException, S("Return statement does not belong to a function."));
  }
  Ast::Type *retType = function->getType()->traceRetType(cmdGenerator->astHelper);

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
        std::make_shared<Spp::Notices::InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    TioSharedPtr tgCastedValue;
    if (!g->generateCast(
      tg, tgContext, operandResult.astType, retType, operandResult.targetData.get(), tgCastedValue)
    ) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
    }
    // Generate the return statement.
    tg->generateReturn(tgContext, getCodeGenData<TiObject>(retType), tgCastedValue.get());
  } else {
    // Make sure return type is void.
    if (!retType->isA<Ast::VoidType>()) {
      cmdGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidReturnValueNotice>(astNode->findSourceLocation())
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

  SharedPtr<IfTgContext> ifTgContext;
  if (!tg->prepareIfStatement(tgContext, elseBody != 0, ifTgContext)) return false;
  setCodeGenData(astNode, ifTgContext);

  // Generate condition.
  GenResult conditionResult;
  if (!g->generateExpression(
    astNode->getCondition().get(), tg, ifTgContext->getConditionContext(), conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, tg, ifTgContext->getConditionContext(), astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate ifBody.
  if (ifBody->isDerivedFrom<Ast::Block>()) {
    if (!g->generateStatements(static_cast<Ast::Block*>(ifBody), tg, ifTgContext->getBodyContext())) {
      return false;
    }
  } else {
    if (!g->generateStatement(ifBody, tg, ifTgContext->getBodyContext())) {
      return false;
    }
  }

  // Generate elseBody, if needed.
  if (elseBody != 0) {
    if (elseBody->isDerivedFrom<Ast::Block>()) {
      if (!g->generateStatements(static_cast<Ast::Block*>(elseBody), tg, ifTgContext->getElseContext())) {
        return false;
      }
    } else {
      if (!g->generateStatement(elseBody, tg, ifTgContext->getElseContext())) {
        return false;
      }
    }
  }

  return tg->finishIfStatement(tgContext, ifTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateWhileStatement(
  TiObject *self, Spp::Ast::WhileStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  SharedPtr<LoopTgContext> loopTgContext;
  if (!tg->prepareWhileStatement(tgContext, loopTgContext)) return false;
  setCodeGenData(astNode, loopTgContext);

  // Generate the condition.
  GenResult conditionResult;
  if (!g->generateExpression(
    astNode->getCondition().get(), tg, loopTgContext->getConditionContext(), conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, tg, loopTgContext->getConditionContext(), astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate body.
  auto body = astNode->getBody().get();
  if (body->isDerivedFrom<Ast::Block>()) {
    if (!g->generateStatements(static_cast<Ast::Block*>(body), tg, loopTgContext->getBodyContext())) {
      return false;
    }
  } else {
    if (!g->generateStatement(body, tg, loopTgContext->getBodyContext())) {
      return false;
    }
  }

  return tg->finishWhileStatement(tgContext, loopTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateForStatement(
  TiObject *self, Spp::Ast::ForStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  if (astNode->getInitializer().get() != 0) {
    if (!g->generateStatement(astNode->getInitializer().get(), tg, tgContext)) return false;
  }

  SharedPtr<LoopTgContext> loopTgContext;
  if (!tg->prepareForStatement(tgContext, loopTgContext)) return false;
  setCodeGenData(astNode, loopTgContext);

  // Generate the condition.
  GenResult conditionResult;
  if (!g->generateExpression(
    astNode->getCondition().get(), tg, loopTgContext->getConditionContext(), conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, tg, loopTgContext->getConditionContext(), astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate the updater.
  GenResult updateResult;
  if (!g->generateExpression(
    astNode->getUpdater().get(), tg, loopTgContext->getUpdaterContext(), updateResult
  )) return false;

  // Generate body.
  auto body = astNode->getBody().get();
  if (body->isDerivedFrom<Ast::Block>()) {
    if (!g->generateStatements(static_cast<Ast::Block*>(body), tg, loopTgContext->getBodyContext())) {
      return false;
    }
  } else {
    if (!g->generateStatement(body, tg, loopTgContext->getBodyContext())) {
      return false;
    }
  }

  return tg->finishForStatement(tgContext, loopTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateContinueStatement(
  TiObject *self, Spp::Ast::ContinueStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get the steps.
  auto stepsNode = ti_cast<Core::Data::Ast::IntegerLiteral>(astNode->getSteps().get());
  auto steps = stepsNode == 0 ? 1 : std::stoi(stepsNode->getValue().get());
  if (steps <= 0) {
    cmdGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidContinueStepsNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Find the loop statement.
  Core::Data::Node *loopNode = astNode;
  while (steps > 0) {
    loopNode = loopNode->getOwner();
    while (!loopNode->isDerivedFrom<Spp::Ast::WhileStatement>() && !loopNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
      if (loopNode->isDerivedFrom<Spp::Ast::Function>()) {
        cmdGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidContinueStepsNotice>(astNode->findSourceLocation())
        );
        return false;
      }
      loopNode = loopNode->getOwner();
    }
    --steps;
  }

  auto loopTgContext = getCodeGenData<LoopTgContext>(loopNode);
  return tg->generateContinue(tgContext, loopTgContext);
}


Bool CommandGenerator::_generateBreakStatement(
  TiObject *self, Spp::Ast::BreakStatement *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get the steps.
  auto stepsNode = ti_cast<Core::Data::Ast::IntegerLiteral>(astNode->getSteps().get());
  auto steps = stepsNode == 0 ? 1 : std::stoi(stepsNode->getValue().get());
  if (steps <= 0) {
    cmdGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidBreakStepsNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Find the loop statement.
  Core::Data::Node *loopNode = astNode;
  while (steps > 0) {
    loopNode = loopNode->getOwner();
    while (!loopNode->isDerivedFrom<Spp::Ast::WhileStatement>() && !loopNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
      if (loopNode->isDerivedFrom<Spp::Ast::Function>()) {
        cmdGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidBreakStepsNotice>(astNode->findSourceLocation())
        );
        return false;
      }
      loopNode = loopNode->getOwner();
    }
    --steps;
  }

  auto loopTgContext = getCodeGenData<LoopTgContext>(loopNode);
  return tg->generateBreak(tgContext, loopTgContext);
}


//==============================================================================
// Helper Functions

Bool CommandGenerator::castCondition(
  Generation *g, TargetGeneration *tg, TiObject *tgContext, TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  auto boolType = this->astHelper->getBoolType();
  if (!astType->isImplicitlyCastableTo(boolType, this->astHelper, tg->getExecutionContext())) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidConditionValueNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }
  if (!g->generateCast(tg, tgContext, astType, this->astHelper->getBoolType(), tgValue, result)) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidConditionValueNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} } // namespace
