/**
 * @file Spp/CodeGen/CommandGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  TiObject *self, Spp::Ast::ReturnStatement *astNode, Generation *g, Session *session
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get this function's return type.
  Ast::Function *function = Core::Data::findOwner<Ast::Function>(astNode);
  if (function == 0) {
    throw EXCEPTION(GenericException, S("Return statement does not belong to a function."));
  }
  auto retTypeRef = function->getType()->getRetType().ti_cast_get<Core::Data::Node>();
  Ast::Type *retType = function->getType()->traceRetType(cmdGenerator->astHelper);

  auto operand = astNode->getOperand().get();
  if (operand != 0) {
    // Generate the expression to return.
    GenResult operandResult;
    if (!g->generateExpression(operand, session, operandResult)) return false;
    // Generate the return statement.
    if (retType->hasCustomInitialization(cmdGenerator->astHelper, session->getTg()->getExecutionContext())) {
      // Assign value to ret reference.
      TiObject *retTgType;
      if (!g->getGeneratedType(retType, session, retTgType, 0)) return false;

      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      PlainList<TiObject> initAstNodes;
      initTgVals.add(operandResult.targetData);
      initAstTypes.add(operandResult.astType);
      initAstNodes.add(ti_cast<Core::Data::Node>(operand));
      if (!g->generateVarInitialization(
        retType, session->getEda()->getCodeGenData<TiObject>(retTypeRef), astNode, &initAstNodes, &initAstTypes, &initTgVals,
        session
      )) {
        return false;
      }
      // Destruct variables.
      if (!g->generateVarGroupDestruction(session, 0)) return false;
      // Return
      session->getTg()->generateReturn(session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(retType), 0);
    } else {
      // Return the value itself.

      // Cast the returned value, if needed.
      TioSharedPtr tgCastedValue;
      if (!g->generateCast(
        session, operandResult.astType, retType, astNode, operandResult.targetData.get(), true, tgCastedValue)
      ) {
        cmdGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidReturnValueNotice>(astNode->findSourceLocation())
        );
        return false;
      }
      // Destruct variables.
      if (!g->generateVarGroupDestruction(session, 0)) return false;
      // Return
      session->getTg()->generateReturn(
        session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(retType), tgCastedValue.get()
      );
    }
  } else {
    // Make sure return type is void.
    if (!retType->isA<Ast::VoidType>()) {
      cmdGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    // Destruct variables.
    if (!g->generateVarGroupDestruction(session, 0)) return false;
    // Generate a void return statement.
    session->getTg()->generateReturn(session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(retType), 0);
  }

  return true;
}


Bool CommandGenerator::_generateIfStatement(
  TiObject *self, Spp::Ast::IfStatement *astNode, Generation *g, Session *session,
  TerminalStatement &terminal
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  auto ifBody = astNode->getIfBody().get();
  auto elseBody = astNode->getElseBody().get();

  terminal = TerminalStatement::UNKNOWN;

  SharedPtr<IfTgContext> ifTgContext;
  if (!session->getTg()->prepareIfStatement(session->getTgContext(), elseBody != 0, ifTgContext)) return false;
  session->getEda()->setCodeGenData(astNode, ifTgContext);

  // Generate condition.
  GenResult conditionResult;
  Session condSession(session, ifTgContext->getConditionContext());
  if (!g->generateExpression(astNode->getCondition().get(), &condSession, conditionResult)) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, &condSession, astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;

  // Generate ifBody.
  TerminalStatement terminalBody = TerminalStatement::UNKNOWN;
  Session bodySession(session, ifTgContext->getBodyContext());
  if (ifBody->isDerivedFrom<Core::Data::Ast::Scope>()) {
    session->getEda()->setCodeGenData(
      static_cast<Core::Data::Ast::Scope*>(ifBody), getSharedPtr(ifTgContext->getBodyContext())
    );
    if (!g->generateStatements(
      static_cast<Core::Data::Ast::Scope*>(ifBody), &bodySession, terminalBody)
    ) {
      return false;
    }
  } else {
    session->getDestructionStack()->pushScope();
    if (!g->generateStatement(ifBody, &bodySession, terminalBody)) {
      return false;
    }
    Bool result = true;
    if (terminalBody != TerminalStatement::YES) {
      if (!g->generateVarGroupDestruction(
        &bodySession, session->getDestructionStack()->getScopeStartIndex(-1)
      )) result = false;
    }
    session->getDestructionStack()->popScope();
    if (!result) return false;
  }

  // Generate elseBody, if needed.
  TerminalStatement terminalElse = TerminalStatement::UNKNOWN;
  Session elseSession(session, ifTgContext->getElseContext());
  if (elseBody != 0) {
    if (elseBody->isDerivedFrom<Core::Data::Ast::Scope>()) {
      session->getEda()->setCodeGenData(static_cast<Core::Data::Ast::Scope*>(elseBody), getSharedPtr(ifTgContext->getElseContext()));
      if (!g->generateStatements(
        static_cast<Core::Data::Ast::Scope*>(elseBody), &elseSession, terminalElse
      )) {
        return false;
      }
    } else {
      session->getDestructionStack()->pushScope();
      if (!g->generateStatement(elseBody, &elseSession, terminalElse)) {
        return false;
      }
      Bool result = true;
      if (terminalBody != TerminalStatement::YES) {
        if (!g->generateVarGroupDestruction(
          &elseSession, session->getDestructionStack()->getScopeStartIndex(-1)
        )) result = false;
      }
      session->getDestructionStack()->popScope();
      if (!result) return false;
    }
  }

  terminal = terminalBody == TerminalStatement::YES && terminalElse == TerminalStatement::YES ?
    TerminalStatement::YES :
    TerminalStatement::NO;

  return session->getTg()->finishIfStatement(session->getTgContext(), ifTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateWhileStatement(
  TiObject *self, Spp::Ast::WhileStatement *astNode, Generation *g, Session *session
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  SharedPtr<LoopTgContext> loopTgContext;
  if (!session->getTg()->prepareWhileStatement(session->getTgContext(), loopTgContext)) return false;
  session->getEda()->setCodeGenData(astNode, loopTgContext);

  // Generate the condition.
  session->getDestructionStack()->pushScope();
  GenResult conditionResult;
  Session condSession(session, loopTgContext->getConditionContext());
  if (!g->generateExpression(
    astNode->getCondition().get(), &condSession, conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, &condSession, astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;
  Bool result = true;
  if (!g->generateVarGroupDestruction(
    &condSession, session->getDestructionStack()->getScopeStartIndex(-1)
  )) result = false;
  session->getDestructionStack()->popScope();
  if (!result) return false;

  // Generate body.
  auto body = astNode->getBody().get();
  Session bodySession(session, loopTgContext->getBodyContext());
  if (body->isDerivedFrom<Core::Data::Ast::Scope>()) {
    session->getEda()->setCodeGenData(
      static_cast<Core::Data::Ast::Scope*>(body), getSharedPtr(loopTgContext->getBodyContext())
    );
    TerminalStatement terminal;
    if (!g->generateStatements(
      static_cast<Core::Data::Ast::Scope*>(body), &bodySession, terminal)
    ) {
      return false;
    }
  } else {
    session->getDestructionStack()->pushScope();
    TerminalStatement terminal;
    if (!g->generateStatement(body, &bodySession, terminal)) {
      return false;
    }
    if (terminal != TerminalStatement::YES) {
      if (!g->generateVarGroupDestruction(
        &bodySession, session->getDestructionStack()->getScopeStartIndex(-1)
      )) result = false;
    }
    session->getDestructionStack()->popScope();
    if (!result) return false;
  }

  return session->getTg()->finishWhileStatement(session->getTgContext(), loopTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateForStatement(
  TiObject *self, Spp::Ast::ForStatement *astNode, Generation *g, Session *session
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  if (astNode->getInitializer().get() != 0) {
    TerminalStatement terminal;
    if (!g->generateStatement(astNode->getInitializer().get(), session, terminal)) return false;
  }

  SharedPtr<LoopTgContext> loopTgContext;
  if (!session->getTg()->prepareForStatement(session->getTgContext(), loopTgContext)) return false;
  session->getEda()->setCodeGenData(astNode, loopTgContext);

  // Generate the condition.
  session->getDestructionStack()->pushScope();
  GenResult conditionResult;
  Session condSession(session, loopTgContext->getConditionContext());
  if (!g->generateExpression(
    astNode->getCondition().get(), &condSession, conditionResult
  )) return false;
  TioSharedPtr castResult;
  if (!cmdGenerator->castCondition(
    g, &condSession, astNode->getCondition().get(), conditionResult.astType,
    conditionResult.targetData.get(), castResult
  )) return false;
  Bool result = true;
  if (!g->generateVarGroupDestruction(
    &condSession, session->getDestructionStack()->getScopeStartIndex(-1)
  )) result = false;
  session->getDestructionStack()->popScope();
  if (!result) return false;

  // Generate the updater.
  session->getDestructionStack()->pushScope();
  GenResult updateResult;
  Session updaterSession(session, loopTgContext->getUpdaterContext());
  if (!g->generateExpression(
    astNode->getUpdater().get(), &updaterSession, updateResult
  )) return false;
  if (!g->generateVarGroupDestruction(
    &updaterSession, session->getDestructionStack()->getScopeStartIndex(-1)
  )) result = false;
  session->getDestructionStack()->popScope();
  if (!result) return false;

  // Generate body.
  auto body = astNode->getBody().get();
  Session bodySession(session, loopTgContext->getBodyContext());
  if (body->isDerivedFrom<Core::Data::Ast::Scope>()) {
    session->getEda()->setCodeGenData(
      static_cast<Core::Data::Ast::Scope*>(body), getSharedPtr(loopTgContext->getBodyContext())
    );
    TerminalStatement terminal;
    if (!g->generateStatements(
      static_cast<Core::Data::Ast::Scope*>(body), &bodySession, terminal)
    ) {
      return false;
    }
  } else {
    session->getDestructionStack()->pushScope();
    TerminalStatement terminal;
    if (!g->generateStatement(body, &bodySession, terminal)) {
      return false;
    }
    if (terminal != TerminalStatement::YES) {
      if (!g->generateVarGroupDestruction(
        &bodySession, session->getDestructionStack()->getScopeStartIndex(-1)
      )) result = false;
    }
    session->getDestructionStack()->popScope();
    if (!result) return false;
  }

  return session->getTg()->finishForStatement(session->getTgContext(), loopTgContext.get(), castResult.get());
}


Bool CommandGenerator::_generateContinueStatement(
  TiObject *self, Spp::Ast::ContinueStatement *astNode, Generation *g, Session *session
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
  Int scopeCount = 0;
  Core::Data::Node *loopNode = astNode;
  while (steps > 0) {
    if (loopNode->isDerivedFrom<Spp::Ast::WhileStatement>() || loopNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
      if (--steps == 0) break;
    } else if (loopNode->isDerivedFrom<Spp::Ast::Function>()) {
      cmdGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidContinueStepsNotice>(astNode->findSourceLocation())
      );
      return false;
    } else if (loopNode->isDerivedFrom<Core::Data::Ast::Scope>()) {
      ++scopeCount;
    } else {
      // if it's a non-scope body of a control statement we still want to treat it as if it's a scope.
      if (
        ti_cast<Spp::Ast::WhileStatement>(loopNode->getOwner()) != 0 ||
        ti_cast<Spp::Ast::ForStatement>(loopNode->getOwner()) != 0 ||
        ti_cast<Spp::Ast::IfStatement>(loopNode->getOwner()) != 0
      ) {
        ++scopeCount;
      }
    }
    loopNode = loopNode->getOwner();
  }

  if (!g->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-scopeCount))) return false;

  auto loopTgContext = session->getEda()->getCodeGenData<LoopTgContext>(loopNode);
  return session->getTg()->generateContinue(session->getTgContext(), loopTgContext);
}


Bool CommandGenerator::_generateBreakStatement(
  TiObject *self, Spp::Ast::BreakStatement *astNode, Generation *g, Session *session
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
  Int scopeCount = 0;
  Core::Data::Node *loopNode = astNode;
  while (steps > 0) {
    if (loopNode->isDerivedFrom<Spp::Ast::WhileStatement>() || loopNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
      if (--steps == 0) break;
    } else if (loopNode->isDerivedFrom<Spp::Ast::Function>()) {
      cmdGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidBreakStepsNotice>(astNode->findSourceLocation())
      );
      return false;
    } else if (loopNode->isDerivedFrom<Core::Data::Ast::Scope>()) {
      ++scopeCount;
    } else {
      // if it's a non-scope body of a control statement we still want to treat it as if it's a scope.
      if (
        ti_cast<Spp::Ast::WhileStatement>(loopNode->getOwner()) != 0 ||
        ti_cast<Spp::Ast::ForStatement>(loopNode->getOwner()) != 0 ||
        ti_cast<Spp::Ast::IfStatement>(loopNode->getOwner()) != 0
      ) {
        ++scopeCount;
      }
    }
    loopNode = loopNode->getOwner();
  }

  if (!g->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-scopeCount))) return false;

  auto loopTgContext = session->getEda()->getCodeGenData<LoopTgContext>(loopNode);
  return session->getTg()->generateBreak(session->getTgContext(), loopTgContext);
}


//==============================================================================
// Helper Functions

Bool CommandGenerator::castCondition(
  Generation *g, Session *session, TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  auto boolType = this->astHelper->getBoolType();
  if (!g->generateCast(session, astType, boolType, ti_cast<Core::Data::Node>(astNode), tgValue, true, result)) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidConditionValueNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} } // namespace
