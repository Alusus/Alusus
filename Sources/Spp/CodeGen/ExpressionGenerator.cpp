/**
 * @file Spp/CodeGen/ExpressionGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Initialization Functions

void ExpressionGenerator::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->generate,
    &this->generateList,
    &this->generateIdentifier,
    &this->generateLinkOperator,
    &this->generateParamPass,
    &this->generateRoundParamPass,
    &this->generateRoundParamPassOnCallee,
    &this->generateSquareParamPass,
    &this->generateOperator,
    &this->generateLogicalOp,
    &this->generateArithmeticOp,
    &this->generateBinaryOp,
    &this->generateComparisonOp,
    &this->generateAssignOp,
    &this->generateArithmeticAssignOp,
    &this->generateBinaryAssignOp,
    &this->generateUnaryValOp,
    &this->generateIntUnaryValOp,
    &this->generateUnaryVarOp,
    &this->generatePointerOp,
    &this->generateAstRefOp,
    &this->generateAstLiteralCommand,
    &this->generateContentOp,
    &this->generateDerefOp,
    &this->generateNoDerefOp,
    &this->generateCastOp,
    &this->generateSizeOp,
    &this->generateInitOp,
    &this->generateTerminateOp,
    &this->generateNextArgOp,
    &this->generateUseInOp,
    &this->generateStringLiteral,
    &this->generateCharLiteral,
    &this->generateIntegerLiteral,
    &this->generateFloatLiteral,
    &this->generateInnerFunction,
    &this->generateReferenceToNonObjectMember,
    &this->generateVarReference,
    &this->generateMemberVarReference,
    &this->generateArrayReference,
    &this->generateFunctionCall,
    &this->generateFunctionPtrCall,
    &this->prepareFunctionParams,
    &this->prepareCallee,
    &this->prepareCalleeLookupRequest,
    &this->generateCalleeReferenceChain,
    &this->referencifyThisIfNeeded,
    &this->generateParams
  });
}


void ExpressionGenerator::initBindings()
{
  this->generate = &ExpressionGenerator::_generate;
  this->generateList = &ExpressionGenerator::_generateList;
  this->generateIdentifier = &ExpressionGenerator::_generateIdentifier;
  this->generateLinkOperator = &ExpressionGenerator::_generateLinkOperator;
  this->generateParamPass = &ExpressionGenerator::_generateParamPass;
  this->generateRoundParamPass = &ExpressionGenerator::_generateRoundParamPass;
  this->generateRoundParamPassOnCallee = &ExpressionGenerator::_generateRoundParamPassOnCallee;
  this->generateSquareParamPass = &ExpressionGenerator::_generateSquareParamPass;
  this->generateOperator = &ExpressionGenerator::_generateOperator;
  this->generateLogicalOp = &ExpressionGenerator::_generateLogicalOp;
  this->generateArithmeticOp = &ExpressionGenerator::_generateArithmeticOp;
  this->generateBinaryOp = &ExpressionGenerator::_generateBinaryOp;
  this->generateComparisonOp = &ExpressionGenerator::_generateComparisonOp;
  this->generateAssignOp = &ExpressionGenerator::_generateAssignOp;
  this->generateArithmeticAssignOp = &ExpressionGenerator::_generateArithmeticAssignOp;
  this->generateBinaryAssignOp = &ExpressionGenerator::_generateBinaryAssignOp;
  this->generateUnaryValOp = &ExpressionGenerator::_generateUnaryValOp;
  this->generateIntUnaryValOp = &ExpressionGenerator::_generateIntUnaryValOp;
  this->generateUnaryVarOp = &ExpressionGenerator::_generateUnaryVarOp;
  this->generatePointerOp = &ExpressionGenerator::_generatePointerOp;
  this->generateAstRefOp = &ExpressionGenerator::_generateAstRefOp;
  this->generateAstLiteralCommand = &ExpressionGenerator::_generateAstLiteralCommand;
  this->generateContentOp = &ExpressionGenerator::_generateContentOp;
  this->generateDerefOp = &ExpressionGenerator::_generateDerefOp;
  this->generateNoDerefOp = &ExpressionGenerator::_generateNoDerefOp;
  this->generateCastOp = &ExpressionGenerator::_generateCastOp;
  this->generateSizeOp = &ExpressionGenerator::_generateSizeOp;
  this->generateInitOp = &ExpressionGenerator::_generateInitOp;
  this->generateTerminateOp = &ExpressionGenerator::_generateTerminateOp;
  this->generateNextArgOp = &ExpressionGenerator::_generateNextArgOp;
  this->generateUseInOp = &ExpressionGenerator::_generateUseInOp;
  this->generateStringLiteral = &ExpressionGenerator::_generateStringLiteral;
  this->generateCharLiteral = &ExpressionGenerator::_generateCharLiteral;
  this->generateIntegerLiteral = &ExpressionGenerator::_generateIntegerLiteral;
  this->generateFloatLiteral = &ExpressionGenerator::_generateFloatLiteral;
  this->generateInnerFunction = &ExpressionGenerator::_generateInnerFunction;
  this->generateReferenceToNonObjectMember = &ExpressionGenerator::_generateReferenceToNonObjectMember;
  this->generateVarReference = &ExpressionGenerator::_generateVarReference;
  this->generateMemberVarReference = &ExpressionGenerator::_generateMemberVarReference;
  this->generateArrayReference = &ExpressionGenerator::_generateArrayReference;
  this->generateFunctionCall = &ExpressionGenerator::_generateFunctionCall;
  this->generateFunctionPtrCall = &ExpressionGenerator::_generateFunctionPtrCall;
  this->prepareFunctionParams = &ExpressionGenerator::_prepareFunctionParams;
  this->prepareCallee = &ExpressionGenerator::_prepareCallee;
  this->prepareCalleeLookupRequest = &ExpressionGenerator::_prepareCalleeLookupRequest;
  this->generateCalleeReferenceChain = &ExpressionGenerator::_generateCalleeReferenceChain;
  this->referencifyThisIfNeeded = &ExpressionGenerator::_referencifyThisIfNeeded;
  this->generateParams = &ExpressionGenerator::_generateParams;
}


//==============================================================================
// Top Level Generation Functions

Bool ExpressionGenerator::_generate(
  TiObject *self, TiObject *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    auto expList = static_cast<Core::Data::Ast::List*>(astNode);
    return expGenerator->generateList(expList, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    return expGenerator->generateIdentifier(identifier, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOperator = static_cast<Core::Data::Ast::LinkOperator*>(astNode);
    return expGenerator->generateLinkOperator(linkOperator, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    return expGenerator->generateParamPass(paramPass, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    return expGenerator->generateOperator(infixOp, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::OutfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
    return expGenerator->generateOperator(outfixOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::PointerOp>()) {
    auto pointerOp = static_cast<Spp::Ast::PointerOp*>(astNode);
    return expGenerator->generatePointerOp(pointerOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::AstRefOp>()) {
    auto astRefOp = static_cast<Spp::Ast::AstRefOp*>(astNode);
    return expGenerator->generateAstRefOp(astRefOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::AstLiteralCommand>()) {
    auto astLiteralCommand = static_cast<Spp::Ast::AstLiteralCommand*>(astNode);
    return expGenerator->generateAstLiteralCommand(astLiteralCommand, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContentOp>()) {
    auto contentOp = static_cast<Spp::Ast::ContentOp*>(astNode);
    return expGenerator->generateContentOp(contentOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::DerefOp>()) {
    auto derefOp = static_cast<Spp::Ast::DerefOp*>(astNode);
    return expGenerator->generateDerefOp(derefOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::NoDerefOp>()) {
    auto noDerefOp = static_cast<Spp::Ast::NoDerefOp*>(astNode);
    return expGenerator->generateNoDerefOp(noDerefOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::CastOp>()) {
    auto castOp = static_cast<Spp::Ast::CastOp*>(astNode);
    return expGenerator->generateCastOp(castOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::SizeOp>()) {
    auto sizeOp = static_cast<Spp::Ast::SizeOp*>(astNode);
    return expGenerator->generateSizeOp(sizeOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::InitOp>()) {
    auto initOp = static_cast<Spp::Ast::InitOp*>(astNode);
    return expGenerator->generateInitOp(initOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::TerminateOp>()) {
    auto terminateOp = static_cast<Spp::Ast::TerminateOp*>(astNode);
    return expGenerator->generateTerminateOp(terminateOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::NextArgOp>()) {
    auto nextArgOp = static_cast<Spp::Ast::NextArgOp*>(astNode);
    return expGenerator->generateNextArgOp(nextArgOp, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::UseInOp>()) {
    auto useInOp = static_cast<Spp::Ast::UseInOp*>(astNode);
    return expGenerator->generateUseInOp(useInOp, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    return expGenerator->generateStringLiteral(stringLiteral, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::CharLiteral>()) {
    auto charLiteral = static_cast<Core::Data::Ast::CharLiteral*>(astNode);
    return expGenerator->generateCharLiteral(charLiteral, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    auto integerLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(astNode);
    return expGenerator->generateIntegerLiteral(integerLiteral, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    auto floatLiteral = static_cast<Core::Data::Ast::FloatLiteral*>(astNode);
    return expGenerator->generateFloatLiteral(floatLiteral, g, session, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::Function>()) {
    auto astFunc = static_cast<Spp::Ast::Function*>(astNode);
    return expGenerator->generateInnerFunction(astFunc, g, session, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(astNode);
    auto astFunc = def->getTarget().ti_cast_get<Spp::Ast::Function>();
    if (astFunc != 0) {
      return expGenerator->generateInnerFunction(astFunc, g, session, result);
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Data::Ast::Bracket*>(astNode);
    auto operand = bracket->getOperand().get();
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND && operand != 0) {
      return expGenerator->generate(operand, g, session, result);
    } else {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidOperationNotice>(bracket->findSourceLocation())
      );
      return false;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::PreGenTransformStatement>()) {
    auto preGenTransformStatement = static_cast<Ast::PreGenTransformStatement*>(astNode);
    if (!preGenTransformStatement->isTransformed()) {
      auto transformedBody = preGenTransformStatement->getTransformFunc()(preGenTransformStatement->getBody().get());
      preGenTransformStatement->setBody(transformedBody);
      preGenTransformStatement->setTransformed(true);
    }
    return expGenerator->generate(preGenTransformStatement->getBody().get(), g, session, result);
  }
  expGenerator->astHelper->getNoticeStore()->add(
    newSrdObj<Spp::Notices::UnsupportedOperationNotice>(Core::Data::Ast::findSourceLocation(astNode))
  );
  return false;
}


Bool ExpressionGenerator::_generateList(
  TiObject *self, Core::Data::Ast::List *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  GenResult innerResult;
  for (Int i = 0; i < astNode->getCount(); ++i) {
    auto innerNode = astNode->getElement(i);
    if (innerNode == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidExpressionListNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    if (!expGenerator->generate(innerNode, g, session, innerResult)) return false;
  }
  result = innerResult;
  return true;
}


Bool ExpressionGenerator::_generateIdentifier(
  TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  PlainList<TiObject> paramAstTypes;
  GenResult thisResult;
  return expGenerator->prepareCallee(astNode, &paramAstTypes, S(""), g, session, result, thisResult);
}


Bool ExpressionGenerator::_generateLinkOperator(
  TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (astNode->getType() != S(".")) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  PlainList<TiObject> paramAstTypes;
  GenResult thisResult;
  return expGenerator->prepareCallee(astNode, &paramAstTypes, S(""), g, session, result, thisResult);
}


Bool ExpressionGenerator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->getType() == Core::Data::Ast::BracketType::ROUND) {
    return expGenerator->generateRoundParamPass(astNode, g, session, result);
  } else {
    return expGenerator->generateSquareParamPass(astNode, g, session, result);
  }
}


Bool ExpressionGenerator::_generateRoundParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParams(param, g, session, &paramAstNodes, &paramAstTypes, &paramTgValues)) {
    return false;
  }

  auto operand = astNode->getOperand().ti_cast_get<Core::Data::Node>();
  if (operand == 0) throw EXCEPTION(GenericException, S("Invalid square param pass operand."));

  GenResult callee;
  GenResult thisArg;
  if (!expGenerator->prepareCallee(operand, &paramAstTypes, S("()"), g, session, callee, thisArg)) return false;

  // Make the actual call.
  return expGenerator->generateRoundParamPassOnCallee(
    astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, session, result
  );
}


Bool ExpressionGenerator::_generateRoundParamPassOnCallee(
  TiObject *self, Core::Data::Node *astNode, GenResult const &callee, GenResult const &thisArg,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee.astNode != 0 && callee.astNode->isDerivedFrom<Ast::Function>()) {
    //// Call a function.
    ////
    auto func = static_cast<Ast::Function*>(callee.astNode);
    // Prepare the arguments to send.
    if (thisArg.astType != 0 && func->getType()->isMember()) {
      GenResult refThisArg;
      if (!expGenerator->referencifyThisIfNeeded(astNode, thisArg, g, session, refThisArg)) return false;
      paramAstNodes->insert(0, astNode);
      paramAstTypes->insert(0, refThisArg.astType);
      paramTgValues->insert(0, refThisArg.targetData);
    }
    if (!expGenerator->prepareFunctionParams(
      func->getType().get(), g, session, paramAstNodes, paramAstTypes, paramTgValues
    )) return false;
    // Generate the function call.
    return expGenerator->generateFunctionCall(
      astNode, func, paramAstTypes, paramTgValues, g, session, result
    );
  } else if (callee.astNode != 0 && callee.astNode->isDerivedFrom<Ast::Type>()) {
    // Generate temp variable.
    Ast::Type *astType;
    if (session->getTgContext() != 0) {
      TiObject *tgType;
      if (!g->getGeneratedType(callee.astNode, session, tgType, &astType)) return false;
      if (!g->generateTempVar(astNode, astType, session, false)) return false;
      TioSharedPtr tempVarRef;
      if (!session->getTg()->generateVarReference(
        session->getTgContext(), tgType,
        session->getEda()->getCodeGenData<TiObject>(astNode), tempVarRef
      )) {
        return false;
      }
      if (!g->generateVarInitialization(
        astType, tempVarRef.get(), astNode, paramAstNodes, paramAstTypes, paramTgValues, session
      )) return false;
      // Register temp var for destruction.
      g->registerDestructor(astNode, astType, session->getExecutionContext(), session->getDestructionStack());
      result.targetData = tempVarRef;
    } else {
      astType = expGenerator->astHelper->traceType(callee.astNode);
    }
    result.astNode = 0;
    result.astType = expGenerator->astHelper->getReferenceTypeFor(astType, Ast::ReferenceMode::IMPLICIT);
    return true;
  } else {
    auto contentType = expGenerator->astHelper->tryGetDeepReferenceContentType(callee.astType);
    if (contentType != 0 && contentType->isDerivedFrom<Ast::PointerType>()) {
      //// Call a function pointer.
      ////
      // Get function type.
      auto astFuncType = expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(contentType);
      if (astFuncType == 0) {
        throw EXCEPTION(GenericException, S("Invalid callee type."));
      }
      // Prepare the arguments to send.
      if (thisArg.astType != 0 && astFuncType->isMember()) {
        GenResult refThisArg;
        if (!expGenerator->referencifyThisIfNeeded(astNode, thisArg, g, session, refThisArg)) return false;
        paramAstNodes->insert(0, astNode);
        paramAstTypes->insert(0, refThisArg.astType);
        paramTgValues->insert(0, refThisArg.targetData);
      }
      // Cast funciton args if needed.
      if (!expGenerator->prepareFunctionParams(
        astFuncType, g, session, paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      // Get the target data of the pointer and its type.
      GenResult derefCallee;
      if (!expGenerator->dereferenceIfNeeded(
        callee.astType, callee.targetData.get(), true, false, session, derefCallee
      )) return false;
      TiObject *tgFuncPtrType = session->getEda()->getCodeGenData<TiObject>(derefCallee.astType);
      // Generate the call.
      return expGenerator->generateFunctionPtrCall(
        astNode, astFuncType, derefCallee.targetData.get(), tgFuncPtrType, paramTgValues, g, session, result
      );
    } else if (contentType != 0 && contentType->isDerivedFrom<Ast::ArrayType>()) {
      //// Reference array element.
      ////
      // Get a reference to the array.
      GenResult derefCallee;
      if (!expGenerator->dereferenceIfNeeded(
        callee.astType, callee.targetData.get(), false, false, session, derefCallee
      )) return false;
      // Reference array element.
      return expGenerator->generateArrayReference(
        astNode, derefCallee.targetData.get(), derefCallee.astType,
        paramTgValues->getElement(0), static_cast<Ast::Type*>(paramAstTypes->getElement(0)), g, session, result
      );
    } else {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  }
}


Bool ExpressionGenerator::_generateSquareParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().ti_cast_get<Core::Data::Node>();
  if (operand == 0) throw EXCEPTION(GenericException, S("Invalid square param pass operand."));

  PlainList<TiObject> paramAstTypes;
  GenResult thisResult;
  GenResult operandResult;
  return expGenerator->prepareCallee(astNode, &paramAstTypes, S(""), g, session, result, thisResult);
}


Bool ExpressionGenerator::_generateOperator(
  TiObject *self, Core::Data::Node *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  s_enum(OpType,
    INVALID, ASSIGN, ARITHMETIC, BINARY, COMPARISON, ARITHMETIC_ASSIGN, BINARY_ASSIGN,
    UNARY_VAL, INT_UNARY_VAL, UNARY_VAR
  );

  // Determine operator function name.
  Char const *funcName;
  OpType opType = OpType::INVALID;
  if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    if (infixOp->getFirst() == 0 || infixOp->getSecond() == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IncompleteInfixOpNotice>(infixOp->findSourceLocation())
      );
      return false;
    }
    if (infixOp->getType() == S("+")) { funcName = S("+"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("-")) { funcName = S("-"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("*")) { funcName = S("*"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("/")) { funcName = S("/"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("%")) { funcName = S("%"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S(">>")) { funcName = S(">>"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("<<")) { funcName = S("<<"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("&")) { funcName = S("&"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("|")) { funcName = S("|"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("$")) { funcName = S("$"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("==")) { funcName = S("=="); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("!=")) { funcName = S("!="); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S(">")) { funcName = S(">"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S(">=")) { funcName = S(">="); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("<")) { funcName = S("<"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("<=")) { funcName = S("<="); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("=")) { funcName = S("="); opType = OpType::ASSIGN; }
    else if (infixOp->getType() == S("+=")) { funcName = S("+="); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("-=")) { funcName = S("-="); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("*=")) { funcName = S("*="); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("/=")) { funcName = S("/="); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("%=")) { funcName = S("%="); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S(">>=")) { funcName = S(">>="); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("<<=")) { funcName = S("<<="); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("&=")) { funcName = S("&="); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("|=")) { funcName = S("|="); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("$=")) { funcName = S("$="); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("||") || infixOp->getType() == S("or")) {
      return expGenerator->generateLogicalOp(infixOp, g, session, result);
    } else if (infixOp->getType() == S("&&") || infixOp->getType() == S("and")) {
      return expGenerator->generateLogicalOp(infixOp, g, session, result);
    } else {
      throw EXCEPTION(GenericException, S("Unexpected infix operator."));
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::PrefixOperator*>(astNode);
    if (outfixOp->getOperand() == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IncompleteOutfixOpNotice>(outfixOp->findSourceLocation())
      );
      return false;
    }
    if (outfixOp->getType() == S("+")) { funcName = S("__pos"); opType = OpType::UNARY_VAL; }
    else if (outfixOp->getType() == S("-")) { funcName = S("__neg"); opType = OpType::UNARY_VAL; }
    else if (outfixOp->getType() == S("!")) { funcName = S("__not"); opType = OpType::INT_UNARY_VAL; }
    else if (outfixOp->getType() == S("!!") || outfixOp->getType() == S("not")) {
      funcName = S("__logNot"); opType = OpType::INT_UNARY_VAL;
    } else if (outfixOp->getType() == S("++")) { funcName = S("__earlyInc"); opType = OpType::UNARY_VAR; }
    else if (outfixOp->getType() == S("--")) { funcName = S("__earlyDec"); opType = OpType::UNARY_VAR; }
    else {
      throw EXCEPTION(GenericException, S("Unexpected prefix operator."));
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::PostfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::PostfixOperator*>(astNode);
    if (outfixOp->getOperand() == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IncompleteOutfixOpNotice>(outfixOp->findSourceLocation())
      );
      return false;
    }
    if (outfixOp->getType() == S("++")) { funcName = S("__lateInc"); opType = OpType::UNARY_VAR; }
    else if (outfixOp->getType() == S("--")) { funcName = S("__lateDec"); opType = OpType::UNARY_VAR; }
    else {
      throw EXCEPTION(GenericException, S("Unexpected postfix operator."));
    }
  } else {
    throw EXCEPTION(GenericException, S("Unexpected operator type."));
  }

  // Generate parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto containing = ti_cast<Containing<TiObject>>(astNode);
  for (Int i = 1; i < containing->getElementCount(); ++i) {
    if (!expGenerator->generateParams(
      containing->getElement(i), g, session, &paramAstNodes, &paramAstTypes, &paramTgValues
    )) return false;
  }

  auto operand = ti_cast<Core::Data::Node>(containing->getElement(0));
  if (operand == 0) throw EXCEPTION(GenericException, S("Invalid operator operand."));

  GenResult callee;
  GenResult thisArg;
  if (!expGenerator->prepareCallee(operand, &paramAstTypes, funcName, g, session, callee, thisArg)) return false;

  if (callee.astNode == Ast::getDummyBuiltInOpFunction()) {
    // We have a built-in operation.
    paramTgValues.insertElement(0, thisArg.targetData.get());
    paramAstTypes.insertElement(0, thisArg.astType);
    paramAstNodes.insertElement(0, containing->getElement(0));

    if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::ARITHMETIC) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::BINARY) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::COMPARISON) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateComparisonOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::ARITHMETIC_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticAssignOp(
        infixOp, &paramTgValues, &paramAstTypes, g, session, result
      );
    } else if (opType == OpType::BINARY_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::INT_UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateIntUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else if (opType == OpType::UNARY_VAR) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryVarOp(outfixOp, &paramTgValues, &paramAstTypes, g, session, result);
    } else {
      expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::NoCalleeMatchNotice>(
        Core::Data::Ast::findSourceLocation(astNode)
      ));
      return false;
    }
  } else {
    // We have a function or a funciton pointer to call.
    return expGenerator->generateRoundParamPassOnCallee(
      astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, session, result
    );
  }
}


Bool ExpressionGenerator::_generateLogicalOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the operator.
  TioSharedPtr secondContext;
  if (session->getTgContext() != 0) {
    if (!session->getTg()->prepareLogicalOp(session->getTgContext(), secondContext)) return false;
  }

  // Generate 1st operand.
  GenResult firstResult;
  if (!expGenerator->generate(astNode->getFirst().get(), g, session, firstResult)) return false;

  // Cast 1st operand to boolean.
  TioSharedPtr firstCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, session, astNode->getFirst().get(), firstResult.astType, firstResult.targetData.get(), firstCastedResult
  )) return false;

  Session childSession(session, secondContext.get(), session->getTgAllocContext());

  // Generate 2nd operand.
  GenResult secondResult;
  if (!expGenerator->generate(astNode->getSecond().get(), g, &childSession, secondResult)) {
    return false;
  }

  // Cast 2nd operand to boolean.
  TioSharedPtr secondCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, &childSession, astNode->getSecond().get(), secondResult.astType,
    secondResult.targetData.get(), secondCastedResult
  )) return false;

  // Finalize the operator.
  if (astNode->getType() == S("||") || astNode->getType() == S("or")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->finishLogicalOr(
        session->getTgContext(), secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("&&") || astNode->getType() == S("and")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->finishLogicalAnd(
        session->getTgContext(), secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid logical operator."));
  }
}


Bool ExpressionGenerator::_generateArithmeticOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  GenResult param1, param2;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, false, session, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, false, session, param2
  )) return false;
  Ast::Type *astTargetType = 0;
  Ast::Type *astOp2CastType = 0;

  if (param1.astType->isDerivedFrom<Ast::FloatType>() && param2.astType->isDerivedFrom<Ast::FloatType>()) {
    // Two floats.
    auto floatType1 = static_cast<Ast::FloatType*>(param1.astType);
    auto floatType2 = static_cast<Ast::FloatType*>(param2.astType);
    auto bitCount1 = floatType1->getBitCount(expGenerator->astHelper);
    auto bitCount2 = floatType2->getBitCount(expGenerator->astHelper);
    astOp2CastType = astTargetType = bitCount1 >= bitCount2 ? floatType1 : floatType2;
  } else if (param1.astType->isDerivedFrom<Ast::FloatType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Float and int.
    astOp2CastType = astTargetType = param1.astType;
  } else if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::FloatType>()) {
    // Int and float.
    astOp2CastType = astTargetType = param2.astType;
  } else if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Two integers.
    auto integerType1 = static_cast<Ast::IntegerType*>(param1.astType);
    auto integerType2 = static_cast<Ast::IntegerType*>(param2.astType);
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (targetBitCount < 32) targetBitCount = 32;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astOp2CastType = astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else {
      astOp2CastType = astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    }
  } else if (
    param1.astType->isDerivedFrom<Ast::PointerType>() &&
    param2.astType->isDerivedFrom<Ast::IntegerType>() &&
    (*astNode->getType().get() == C('+') || *astNode->getType().get() == C('-'))
  ) {
    // Pointer and int.
    astTargetType = param1.astType;
    auto integerType2 = static_cast<Ast::IntegerType*>(param2.astType);
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    if (bitCount2 == 1) {
      // Adding a boolean to a pointer gives unexpected results since a boolean true may be treated as -1 rather than 1
      // so we need to cast it to 8 bit integer.
      astOp2CastType = expGenerator->astHelper->getIntType(8);
    } else {
      astOp2CastType = integerType2;
    }
  } else {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      session, param2.astType, astOp2CastType, astNode, param2.targetData.get(), false, param2
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, session, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (*astNode->getType().get() == C('+')) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateAdd(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (*astNode->getType().get() == C('-')) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateSub(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (*astNode->getType().get() == S('*')) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateMul(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (*astNode->getType().get() == S('/')) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateDiv(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (*astNode->getType().get() == S('%')) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateRem(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateBinaryOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  GenResult param1, param2;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, false, session, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, false, session, param2
  )) return false;
  Ast::Type *astTargetType = 0;

  if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Two integers.
    auto integerType1 = static_cast<Ast::IntegerType*>(param1.astType);
    auto integerType2 = static_cast<Ast::IntegerType*>(param2.astType);
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    }
  } else {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      session, param2.astType, astTargetType, astNode, param2.targetData.get(), false, param2
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, session, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateShr(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("<<")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateShl(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("&")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateAnd(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("|")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateOr(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("$")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateXor(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateComparisonOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  Bool equality = astNode->getType() == S("==") || astNode->getType() == S("!=");
  GenResult param1, param2;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, false, session, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, false, session, param2
  )) return false;
  Ast::Type *astTargetType = 0;

  if (param1.astType->isDerivedFrom<Ast::FloatType>() && param2.astType->isDerivedFrom<Ast::FloatType>()) {
    // Two floats.
    auto floatType1 = static_cast<Ast::FloatType*>(param1.astType);
    auto floatType2 = static_cast<Ast::FloatType*>(param2.astType);
    auto bitCount1 = floatType1->getBitCount(expGenerator->astHelper);
    auto bitCount2 = floatType2->getBitCount(expGenerator->astHelper);
    astTargetType = bitCount1 >= bitCount2 ? floatType1 : floatType2;
  } else if (param1.astType->isDerivedFrom<Ast::FloatType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Float and int.
    astTargetType = param1.astType;
  } else if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::FloatType>()) {
    // Int and float.
    astTargetType = param2.astType;
  } else if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Two integers.
    auto integerType1 = static_cast<Ast::IntegerType*>(param1.astType);
    auto integerType2 = static_cast<Ast::IntegerType*>(param2.astType);
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else if (integerType1->isSigned() && integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    } else if (!integerType1->isSigned() && bitCount1 < bitCount2) {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    } else if (integerType1->isSigned() && bitCount1 > bitCount2) {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    } else {
      // error
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
      );
      return false;
    }
  } else if (
    equality &&
    param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::PointerType>() &&
    static_cast<Ast::IntegerType*>(param1.astType)->isNullLiteral()
  ) {
    astTargetType = param2.astType;
  } else if (
    equality &&
    param2.astType->isDerivedFrom<Ast::IntegerType>() && param1.astType->isDerivedFrom<Ast::PointerType>() &&
    static_cast<Ast::IntegerType*>(param2.astType)->isNullLiteral()
  ) {
    astTargetType = param1.astType;
  } else if (
    equality && param1.astType->isDerivedFrom<Ast::PointerType>() && param2.astType->isDerivedFrom<Ast::PointerType>()
  ) {
    astTargetType = param1.astType;
  } else {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      session, param2.astType, astTargetType, astNode, param2.targetData.get(), false, param2
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, session, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("==")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateEqual(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("!=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateNotEqual(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateGreaterThan(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateGreaterThanOrEqual(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateLessThan(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateLessThanOrEqual(
        session->getTgContext(), tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent a comparison operator."));
  }
}


Bool ExpressionGenerator::_generateAssignOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, false, session, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  // Raise an error if custom initialization is enabled on this type to avoid possible resource mismanagement.
  if (astContentType->getInitializationMethod(
    expGenerator->astHelper, session->getExecutionContext()
  ) != Ast::TypeInitMethod::NONE) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::TypeMissingAssignOpNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Cast the param to the destination type.
  auto paramAstType = static_cast<Ast::Type*>(paramAstTypes->get(1));
  GenResult paramCastResult;
  Bool retVal;
  if (session->getTgContext() != 0) {
    retVal = g->generateCast(
      session, paramAstType, astContentType, astNode, paramTgValues->getElement(1), true, paramCastResult
    );
  } else {
    retVal = expGenerator->astHelper->isImplicitlyCastableTo(
      paramAstType, astContentType, session->getExecutionContext()
    );
  }
  if (!retVal) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (
    astNode->getType() == S("=") ||
    (astNode->getType().getStr().getLength() > 1 && astNode->getType().get()[1] == C('='))
  ) {
    if (session->getTgContext() != 0) {
      TiObject *tgContentType;
      if (!g->getGeneratedType(paramCastResult.astType, session, tgContentType, 0)) {
        throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
      }

      if (!session->getTg()->generateAssign(
        session->getTgContext(), tgContentType, paramCastResult.targetData.get(), target.targetData.get(),
        result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateArithmeticAssignOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  GenResult arithmeticResult;
  if (!expGenerator->generateArithmeticOp(astNode, paramTgValues, paramAstTypes, g, session, arithmeticResult)) {
    return false;
  }
  SharedList<TiObject> assignParamTgValues;
  assignParamTgValues.add(paramTgValues->get(0));
  assignParamTgValues.add(arithmeticResult.targetData);
  PlainList<TiObject> assignParamAstTypes;
  assignParamAstTypes.add(paramAstTypes->get(0));
  assignParamAstTypes.add(arithmeticResult.astType);
  return expGenerator->generateAssignOp(astNode, &assignParamTgValues, &assignParamAstTypes, g, session, result);
}


Bool ExpressionGenerator::_generateBinaryAssignOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, false, session, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, false, session, param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (!astContentType->isDerivedFrom<Ast::IntegerType>() || !param.astType->isDerivedFrom<Ast::IntegerType>()) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param.astType, astContentType, astNode, param.targetData.get(), false, param
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(param.astType, session, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateShrAssign(
        session->getTgContext(), tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("<<=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateShlAssign(
        session->getTgContext(), tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("&=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateAndAssign(
        session->getTgContext(), tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("|=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateOrAssign(
        session->getTgContext(), tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("$=")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateXorAssign(
        session->getTgContext(), tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateUnaryValOp(
  TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 1) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly one element."), paramAstTypes->getCount()
    );
  }

  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, false, session, param
  )) return false;
  if (astNode->getType() == S("+")) {
    // This is a no-op.
    result = param;
    return true;
  }

  Ast::Type *astTargetType = 0;
  if (param.astType->isDerivedFrom<Ast::FloatType>()) {
    astTargetType = static_cast<Ast::FloatType*>(param.astType);
  } else if (param.astType->isDerivedFrom<Ast::IntegerType>()) {
    auto integerType = static_cast<Ast::IntegerType*>(param.astType);
    auto bitCount = integerType->getBitCount(expGenerator->astHelper, session->getExecutionContext());
    astTargetType = expGenerator->astHelper->getIntType(bitCount);
  } else {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param.astType, astTargetType, astNode, param.targetData.get(), false, param
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(param.astType, session, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("-")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateNeg(
        session->getTgContext(), tgTargetType, param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = param.astType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateIntUnaryValOp(
  TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 1) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly one element."), paramAstTypes->getCount()
    );
  }

  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, false, session, param
  )) return false;

  Ast::IntegerType *astTargetType = 0;
  if (param.astType->isDerivedFrom<Ast::IntegerType>()) {
    astTargetType = static_cast<Ast::IntegerType*>(param.astType);
  } else {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Limit logical not to boolean types.
  if (
    (astNode->getType() == S("!!") || astNode->getType() == S("not")) &&
    astTargetType->getBitCount(expGenerator->astHelper, session->getExecutionContext()) != 1
  ) {
    // Error.
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, param.astType, astTargetType, astNode, param.targetData.get(), false, param
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(param.astType, session, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("!") || astNode->getType() == S("!!") || astNode->getType() == S("not")) {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateNot(
        session->getTgContext(), tgTargetType, param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = param.astType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateUnaryVarOp(
  TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 1) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly one element."), paramAstTypes->getCount()
    );
  }

  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, false, session, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (!astContentType->isDerivedFrom<Ast::IntegerType>() && !astContentType->isDerivedFrom<Ast::FloatType>()) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, session, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    if (astNode->getType() == S("--")) {
      if (session->getTgContext() != 0) {
        if (!session->getTg()->generateEarlyDec(
          session->getTgContext(), tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (session->getTgContext() != 0) {
        if (!session->getTg()->generateEarlyInc(
          session->getTgContext(), tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else {
      throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
    }
  } else {
    if (astNode->getType() == S("--")) {
      if (session->getTgContext() != 0) {
        if (!session->getTg()->generateLateDec(
          session->getTgContext(), tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (session->getTgContext() != 0) {
        if (!session->getTg()->generateLateInc(
          session->getTgContext(), tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else {
      throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
    }
  }
}


Bool ExpressionGenerator::_generatePointerOp(
  TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    // We have an AST node.
    // Check if the node is a function.
    auto astFunction = ti_cast<Ast::Function>(operandResult.astNode);
    if (astFunction == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
      );
      return false;
    }
    // Generate a function pointer.
    if (!g->generateFunctionDecl(astFunction, session)) return false;
    session->getFuncDeps()->add(astFunction);

    auto tgFunction = session->getEda()->getCodeGenData<TiObject>(astFunction);
    auto astFunctionPointerType = expGenerator->astHelper->getPointerTypeFor(astFunction->getType().get());
    TiObject *tgFunctionPointerType;
    if (!g->getGeneratedType(astFunctionPointerType, session, tgFunctionPointerType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate function pointer type."));
    }
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateFunctionPointer(
        session->getTgContext(), tgFunction, tgFunctionPointerType, result.targetData
      )) return false;
    }
    result.astType = astFunctionPointerType;
    return true;
  } else {
    // We have an expression.
    auto operandRefAstType = ti_cast<Ast::ReferenceType>(operandResult.astType);
    if (operandRefAstType == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
      );
      return false;
    }

    // Get the pointer type.
    result.astType = expGenerator->astHelper->swichInnerReferenceTypeWithPointerType(operandRefAstType);
    TiObject *tgType;
    if (!g->getGeneratedType(result.astType, session, tgType, 0)) return false;
    if (result.astType == 0) return false;
    result.targetData = operandResult.targetData;
    return true;
  }
}


Bool ExpressionGenerator::_generateAstRefOp(
  TiObject *self, Spp::Ast::AstRefOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("AstRefOp operand is missing."));
  }
  TiObject *targetAstNode;
  if (!expGenerator->astHelper->getSeeker()->tryGet(operand, astNode->getOwner(), targetAstNode)) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::UnknownSymbolNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Unbox if we have a box.
  auto box = ti_cast<TioWeakBox>(targetAstNode);
  if (box != 0) targetAstNode = box->get().get();

  // Generate pointer to void.
  auto tiObjType = expGenerator->astHelper->getTiObjectType();
  if (tiObjType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::MissingTypeNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  auto tiObjRefType = expGenerator->astHelper->getReferenceTypeFor(tiObjType, Ast::ReferenceMode::IMPLICIT);
  TiObject *tgTiObjRefType;
  if (!g->getGeneratedType(tiObjRefType, session, tgTiObjRefType, 0)) {
    return false;
  }
  // Capture the element in the repo so that it doesn't get freed while still needed by the generated code.
  expGenerator->astNodeRepo->addElement(targetAstNode);
  // Generate a pointer literal.
  if (!session->getTg()->generatePointerLiteral(
    session->getTgContext(), tgTiObjRefType, targetAstNode, result.targetData
  )) return false;
  result.astType = tiObjRefType;
  return true;
}


Bool ExpressionGenerator::_generateAstLiteralCommand(
  TiObject *self, Spp::Ast::AstLiteralCommand *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto body = astNode->getBody();
  if (body == 0) {
    throw EXCEPTION(GenericException, S("AstLiteralCommand body is missing."));
  }

  // Generate pointer to void.
  auto tiObjType = expGenerator->astHelper->getTiObjectType();
  if (tiObjType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::MissingTypeNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  auto tiObjRefType = expGenerator->astHelper->getReferenceTypeFor(tiObjType, Ast::ReferenceMode::IMPLICIT);
  TiObject *tgTiObjRefType;
  if (!g->getGeneratedType(tiObjRefType, session, tgTiObjRefType, 0)) {
    return false;
  }
  // Capture the body in the repo so that it doesn't get freed while still needed by the generated code.
  expGenerator->astNodeRepo->add(body);
  // Generate a pointer literal.
  if (!session->getTg()->generatePointerLiteral(
    session->getTgContext(), tgTiObjRefType, body.get(), result.targetData
  )) {
    return false;
  }
  result.astType = tiObjRefType;
  return true;
}


Bool ExpressionGenerator::_generateContentOp(
  TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Switch the pointer type to implicit reference type.
  result.astType = expGenerator->astHelper->swichOuterPointerTypeWithReferenceType(
    operandResult.astType, Ast::ReferenceMode::IMPLICIT
  );
  if (result.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  TiObject *tgType;
  if (!g->getGeneratedType(result.astType, session, tgType, 0)) return false;
  result.targetData = operandResult.targetData;
  return true;
}


Bool ExpressionGenerator::_generateDerefOp(
  TiObject *self, Spp::Ast::DerefOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("DerefOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidDerefOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference and get content type.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), true, true, session, target
  )) return false;
  auto refType = ti_cast<Spp::Ast::ReferenceType>(target.astType);
  if (refType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidDerefOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  Spp::Ast::Type *cntAstType = refType->getContentType(expGenerator->astHelper);

  // Use an implicit reference type instead of the explicit reference type currently in use.
  result.astType = expGenerator->astHelper->getReferenceTypeFor(cntAstType, Ast::ReferenceMode::IMPLICIT);
  result.targetData = target.targetData;
  return true;
}


Bool ExpressionGenerator::_generateNoDerefOp(
  TiObject *self, Spp::Ast::NoDerefOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("NoDerefOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType != 0) {
    // Dereference and get content type.
    GenResult target;
    if (!expGenerator->dereferenceIfNeeded(
      operandResult.astType, operandResult.targetData.get(), false, true, session, target
    )) return false;
    auto refType = ti_cast<Spp::Ast::ReferenceType>(target.astType);
    if (refType == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidNoDerefOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
      );
      return false;
    }
    Spp::Ast::Type *cntAstType = refType->getContentType(expGenerator->astHelper);

    // Replace the explicit ref type with an ndref type.
    if (cntAstType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
      refType = static_cast<Spp::Ast::ReferenceType*>(cntAstType);
      cntAstType = expGenerator->astHelper->getReferenceTypeFor(
        refType->getContentType(expGenerator->astHelper), Ast::ReferenceMode::NO_DEREF
      );
    }
    result.astType = expGenerator->astHelper->getReferenceTypeFor(cntAstType, Ast::ReferenceMode::IMPLICIT);
    result.targetData = target.targetData;
    return true;
  } else if (operandResult.astNode->isDerivedFrom<Ast::Type>()) {
    auto type = static_cast<Ast::Type*>(operandResult.astNode);
    auto refType = ti_cast<Spp::Ast::ReferenceType>(type);
    if (refType != 0 && refType->isAutoDeref()) {
      result.astNode = expGenerator->astHelper->getReferenceTypeFor(
        refType->getContentType(expGenerator->astHelper), Ast::ReferenceMode::NO_DEREF
      );
    } else {
      result.astNode = type;
    }
    return true;
  } else {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidNoDerefOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
}


Bool ExpressionGenerator::_generateCastOp(
  TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("CastOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the deepest reference.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), false, false, session, derefResult
  )) return false;

  // Get the target type.
  auto targetAstType = expGenerator->astHelper->traceType(astNode->getTargetType().get());
  if (targetAstType == 0) return false;

  // Cast the value.
  Bool retVal;
  if (session->getTgContext() != 0) {
    retVal = g->generateCast(
      session, derefResult.astType, targetAstType, astNode, derefResult.targetData.get(), false, result
    );
  } else {
    retVal = expGenerator->astHelper->isExplicitlyCastableTo(
      derefResult.astType, targetAstType, session->getExecutionContext()
    );
    if (targetAstType->getInitializationMethod(
      expGenerator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
      result.astType = expGenerator->astHelper->getReferenceTypeFor(targetAstType, Ast::ReferenceMode::IMPLICIT);
    } else {
      result.astType = targetAstType;
    }
  }
  if (!retVal) {
    expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidCastNotice>(astNode->getSourceLocation()));
    return false;
  }
  return true;
}


Bool ExpressionGenerator::_generateSizeOp(
  TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Get the operand type.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  Session childSession(session, 0, 0);
  if (!expGenerator->generate(operand, g, &childSession, operandResult)) return false;
  Ast::Type *astType = operandResult.astType;
  if (astType == 0) {
    if (operandResult.astNode->isDerivedFrom<Spp::Ast::Type>()) {
      astType = static_cast<Spp::Ast::Type*>(operandResult.astNode);
    } else if (operandResult.astNode->isDerivedFrom<Spp::Ast::Template>()) {
      auto tpl = static_cast<Spp::Ast::Template*>(operandResult.astNode);
      Core::Data::Ast::List list;
      TioSharedPtr matchResult;
      if (tpl->matchInstance(&list, expGenerator->getAstHelper(), matchResult)) {
        astType = matchResult.ti_cast_get<Spp::Ast::Type>();
      }
    }
  }
  if (astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidSizeOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference if needed.
  astType = expGenerator->astHelper->tryGetDeepReferenceContentType(astType);

  // Get the allocation size.
  Word size;
  if (!g->getTypeAllocationSize(astType, session, size)) return false;

  // Generate the Word64 type needed for the result.
  auto bitCount = expGenerator->astHelper->getNeededIntSize(size);
  if (bitCount < 8) bitCount = 8;
  auto astWordType = expGenerator->astHelper->getIntType(bitCount);

  // Generate a constant with that size.
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateIntLiteral(session->getTgContext(), bitCount, false, size, result.targetData)) {
      return false;
    }
  }
  result.astType = astWordType;
  return true;
}


Bool ExpressionGenerator::_generateInitOp(
  TiObject *self, Spp::Ast::InitOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidInitOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference and get content type.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), false, false, session, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidInitOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  // Prepare parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParams(param, g, session, &paramAstNodes, &paramAstTypes, &paramTgValues)) {
    return false;
  }

  if (session->getTgContext() != 0) {
    if (!g->generateVarInitialization(
      astContentType, target.targetData.get(), astNode, &paramAstNodes, &paramAstTypes, &paramTgValues, session
    )) return false;
  }
  result = target;
  return true;
}


Bool ExpressionGenerator::_generateTerminateOp(
  TiObject *self, Spp::Ast::TerminateOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidTerminateOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference and get content type.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), false, false, session, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidTerminateOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (session->getTgContext() != 0) {
    if (!g->generateVarDestruction(astContentType, target.targetData.get(), astNode, session)) return false;
  }
  result = target;
  return true;
}


Bool ExpressionGenerator::_generateNextArgOp(
  TiObject *self, Spp::Ast::NextArgOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, session, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the deepest reference.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), false, false, session, derefResult
  )) return false;
  if (ti_cast<Ast::ReferenceType>(derefResult.astType) == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidNextArgNotice>(astNode->getSourceLocation())
    );
    return false;
  }

  // Get the target type.
  auto targetAstType = expGenerator->astHelper->traceType(astNode->getTargetType().get());
  if (targetAstType->getInitializationMethod(
    expGenerator->astHelper, session->getExecutionContext()
  ) != Ast::TypeInitMethod::NONE) {
    targetAstType = expGenerator->astHelper->getReferenceTypeFor(targetAstType, Ast::ReferenceMode::IMPLICIT);
  }
  if (targetAstType == 0) return false;
  TiObject *targetTgType;
  if (!g->getGeneratedType(targetAstType, session, targetTgType, 0)) return false;

  // Get the next arg.
  Bool retVal;
  if (session->getTgContext() != 0) {
    retVal = session->getTg()->generateNextArg(
      session->getTgContext(), targetTgType, derefResult.targetData.get(), result.targetData
    );
    if (!retVal) {
      expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidNextArgNotice>(astNode->getSourceLocation()));
      return false;
    }
  }
  result.astType = targetAstType;
  return true;
}


Bool ExpressionGenerator::_generateUseInOp(
  TiObject *self, Spp::Ast::UseInOp *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("UseInOp operand is missing."));
  }
  if (!expGenerator->generate(operand, g, session, result)) return false;
  if (result.astType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  auto refType = ti_cast<Ast::ReferenceType>(result.astType);
  if (refType == 0) {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidUseInTargetNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Generate the body.
  // Make sure the reference is not an implicit reference.
  auto contentType = refType->getContentType(expGenerator->getAstHelper());
  auto explicitRefType = expGenerator->astHelper->getReferenceTypeFor(contentType, Ast::ReferenceMode::EXPLICIT);
  g->addThisDefinition(
    astNode->getBody().get(), astNode->getOperandName().get(), explicitRefType, astNode->isSkipInjection(),
    result.targetData, session
  );
  TerminalStatement terminal = TerminalStatement::UNKNOWN;
  if (!g->generateStatementBlock(astNode->getBody().get(), session, terminal)) return false;

  return true;
}


Bool ExpressionGenerator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = &astNode->getValue().getStr();

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, session, charTgType, 0)) return false;

  auto strAstType = expGenerator->astHelper->getCharArrayType(value->getLength() + 1);
  TiObject *strTgType;
  if (!g->getGeneratedType(strAstType, session, strTgType, 0)) return false;

  auto strPtrAstType = expGenerator->astHelper->getPointerTypeFor(strAstType);
  TiObject *strPtrTgType;
  if (!g->getGeneratedType(strPtrAstType, session, strPtrTgType, 0)) return false;

  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateStringLiteral(
      session->getTgContext(), value->getBuf(), charTgType, strTgType, result.targetData
    )) return false;
  }
  result.astType =  strPtrAstType;
  return true;
}


Bool ExpressionGenerator::_generateCharLiteral(
  TiObject *self, Core::Data::Ast::CharLiteral *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = astNode->getValue().getStr()(0);

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, session, charTgType, 0)) return false;

  auto bitCount = charAstType->getBitCount(expGenerator->astHelper, session->getExecutionContext());

  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateIntLiteral(session->getTgContext(), bitCount, false, value, result.targetData)) {
      return false;
    }
  }
  result.astType = charAstType;
  return true;
}


Bool ExpressionGenerator::_generateIntegerLiteral(
  TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto src = astNode->getValue().get();

  // TODO: Consider non-English letters prefixes and postfixes in the literal.
  // TODO: Implement unsigned integers.

  // Parse the given value.
  Int base = 10;
  if (compareStr(src, S("0b"), 2) == 0 || compareStr(src, S("0B"), 2) == 0) {
    base = 2;
    src += 2;
  } else if (compareStr(src, S("0ن"), 3) == 0) {
    base = 2;
    src += 3;
  } else if (compareStr(src, S("0o"), 2) == 0 || compareStr(src, S("0O"), 2) == 0) {
    base = 8;
    src += 2;
  } else if (compareStr(src, S("0م"), 3) == 0) {
    base = 8;
    src += 3;
  } else if (
    compareStr(src, S("0h"), 2) == 0 || compareStr(src, S("0H"), 2) == 0 ||
    compareStr(src, S("0x"), 2) == 0 || compareStr(src, S("0X"), 2) == 0
  ) {
    base = 16;
    src += 2;
  }
  LongWord value = 0;
  while (
    (*src >= C('0') && *src <= C('9')) ||
    (*src >= C('a') && *src <= C('f')) ||
    (*src >= C('A') && *src <= C('F'))
  ) {
    Int digit = 0;
    if (*src >= C('0') && *src <= C('9')) digit = *src - C('0');
    else if (*src >= C('a') && *src <= C('f')) digit = *src - C('a') + 10;
    else if (*src >= C('A') && *src <= C('F')) digit = *src - C('A') + 10;
    ASSERT(digit < base);
    value *= base;
    value += digit;
    ++src;
  }

  // Is it a signed number?
  Bool signedNum = true;
  if (*src == C('u') || *src == C('U')) {
    signedNum = false;
    ++src;
  } else if (compareStr(src, S("ط"), 2) == 0) {
    signedNum = false;
    src += 2;
  }

  // Determine integer size.
  Bool typeRequested = false;
  Int size = 0;
  if (*src == C('i') || *src == C('I')) {
    typeRequested = true;
    ++src;
    if (getStrLen(src) > 0) size = std::stoi(src);
  } else if (compareStr(src, S("ص"), 2) == 0) {
    typeRequested = true;
    src += 2;
    if (getStrLen(src) > 0) size = std::stoi(src);
  }
  if (size == 0) {
    if (value == 0 || value == 1) {
      // Give special treatment to 0 and 1 and consider it unsigned.
      signedNum = false;
    }
    size = signedNum ?
      expGenerator->astHelper->getNeededIntSize((LongInt)value) :
      expGenerator->astHelper->getNeededWordSize(value);
  }

  // Get the requested type.
  Ast::Type *astType;
  if (!typeRequested && value == 0) {
    astType = expGenerator->astHelper->getNullType();
  } else if (signedNum) {
    astType = expGenerator->astHelper->getIntType(size);
  } else {
    astType = expGenerator->astHelper->getWordType(size);
  }
  auto sourceLocation = astNode->findSourceLocation().get();
  expGenerator->astHelper->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
  TiObject *intTgType;
  Bool retVal = g->getGeneratedType(astType, session, intTgType, 0);
  expGenerator->astHelper->getNoticeStore()->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateIntLiteral(session->getTgContext(), size, signedNum, value, result.targetData)) {
      return false;
    }
  }
  result.astType = astType;
  return true;
}


Bool ExpressionGenerator::_generateFloatLiteral(
  TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto src = astNode->getValue().get();

  // TODO: Consider non-English letters prefixes and postfixes in the literal.
  // TODO: Consider the different float sizes.

  // Parse the given value.
  std::size_t numSize;
  Double value = std::stof(src, &numSize);
  src += numSize;

  // Determine float size.
  Int size = 32;
  if (*src == C('f') || *src == C('F')) {
    ++src;
    if (getStrLen(src) > 0) size = std::stoi(src);
  } else if (compareStr(src, S("ع"), 2) == 0) {
    src += 2;
    if (getStrLen(src) > 0) size = std::stoi(src);
  }

  // Get the requested float type.
  auto floatAstType = expGenerator->astHelper->getFloatType(size);
  auto sourceLocation = astNode->findSourceLocation().get();
  expGenerator->astHelper->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
  TiObject *floatTgType;
  Bool retVal = g->getGeneratedType(floatAstType, session, floatTgType, 0);
  expGenerator->astHelper->getNoticeStore()->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateFloatLiteral(session->getTgContext(), size, value, result.targetData)) return false;
  }
  result.astType = floatAstType;
  return true;
}


Bool ExpressionGenerator::_generateInnerFunction(
  TiObject *self, Spp::Ast::Function *astFunction, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (!g->generateFunctionDecl(astFunction, session)) return false;
  session->getFuncDeps()->add(astFunction);

  auto tgFunction = session->getEda()->getCodeGenData<TiObject>(astFunction);
  auto astFunctionPointerType = expGenerator->astHelper->getPointerTypeFor(astFunction->getType().get());
  TiObject *tgFunctionPointerType;
  if (!g->getGeneratedType(astFunctionPointerType, session, tgFunctionPointerType, 0)) {
    throw EXCEPTION(GenericException, S("Failed to generate function pointer type."));
  }
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateFunctionPointer(
      session->getTgContext(), tgFunction, tgFunctionPointerType, result.targetData
    )) return false;
  }
  result.astType = astFunctionPointerType;
  return true;
}


//==============================================================================
// Inner Generation Functions

Bool ExpressionGenerator::_generateReferenceToNonObjectMember(
  TiObject *self, TiObject *obj, Core::Data::Node *astNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  Bool retVal = false;

  if (expGenerator->astHelper->isVariable(obj)) {
    // Make sure the var is not an object member.
    if (expGenerator->getAstHelper()->getVariableDomain(obj) == Ast::DefinitionDomain::OBJECT) {
      expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidObjectMemberAccessNotice>(
        Core::Data::Ast::findSourceLocation(astNode)
      ));
      retVal = false;
    } else {
      retVal = expGenerator->generateVarReference(astNode, obj, g, session, result);
    }
  } else if (ti_cast<Ast::ArgPack>(obj) != 0) {
    retVal = expGenerator->generateVarReference(astNode, obj, g, session, result);
  } else if (
    obj->isDerivedFrom<Core::Data::Ast::Scope>() || obj->isDerivedFrom<Ast::Type>() ||
    obj->isDerivedFrom<Ast::Template>() || obj->isDerivedFrom<Ast::Function>()
  ) {
    result.astNode = obj;
    retVal = true;
    // If the found object is a type, then let's make sure it's preprocessed.
    auto dataType = ti_cast<Spp::Ast::DataType>(result.astNode);
    if (dataType != 0) {
      auto sourceLocation = Core::Data::Ast::findSourceLocation(astNode).get();
      if (sourceLocation != 0) expGenerator->astHelper->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
      TiObject *tgType;
      auto result = g->getGeneratedType(dataType, session, tgType, 0);
      if (sourceLocation != 0) {
        expGenerator->astHelper->getNoticeStore()->popPrefixSourceLocation(
          Core::Data::getSourceLocationRecordCount(sourceLocation)
        );
      }
      if (!result) return false;
    }
  } else if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    retVal = expGenerator->generateStringLiteral(
      static_cast<Core::Data::Ast::StringLiteral*>(obj), g, session, result
    );
  } else if (obj->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    retVal = expGenerator->generateIntegerLiteral(
      static_cast<Core::Data::Ast::IntegerLiteral*>(obj), g, session, result
    );
  } else if (obj->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    retVal = expGenerator->generateFloatLiteral(
      static_cast<Core::Data::Ast::FloatLiteral*>(obj), g, session, result
    );
  } else {
    expGenerator->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
  }

  return retVal;
}


Bool ExpressionGenerator::_generateVarReference(
  TiObject *self, TiObject *refAstNode, TiObject *varAstNode, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto tgVar = session->getEda()->tryGetCodeGenData<TiObject>(varAstNode);
  if (tgVar == 0 && session->getTgContext() != 0) {
    // Generate the variable definition.
    auto varDef = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(varAstNode)->getOwner());
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Unexpected error while looking for variable definition."));
    }
    if (expGenerator->getAstHelper()->getVariableDomain(varDef) == Ast::DefinitionDomain::GLOBAL) {
      if (!g->generateVarDef(varDef, session)) return false;
      tgVar = session->getEda()->getCodeGenData<TiObject>(varAstNode);
    } else {
      expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::UninitializedVariableNotice>(
        Core::Data::Ast::findSourceLocation(refAstNode)
      ));
      return false;
    }
  }

  Ast::Type *astType = expGenerator->getAstHelper()->traceType(varAstNode);
  TiObject *tgType;
  if (!g->getGeneratedType(astType, session, tgType, 0)) return false;

  if (expGenerator->getAstHelper()->isValueOnlyVariable(varAstNode)) {
    result.targetData = getSharedPtr(tgVar);
    result.astType = astType;
  } else {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateVarReference(session->getTgContext(), tgType, tgVar, result.targetData)) return false;
    }
    result.astType = expGenerator->astHelper->getReferenceTypeFor(astType, Ast::ReferenceMode::IMPLICIT);
  }
  return true;
}


Bool ExpressionGenerator::_generateMemberVarReference(
  TiObject *self, Core::Data::Node *astNode, TiObject *tgStructValue, Ast::Type * astStructType, TiObject *astMemberVar,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  TiObject *tgStructType;
  if (!g->getGeneratedType(astStructType, session, tgStructType, 0)) return false;

  // Make sure the var is an object member.
  if (expGenerator->getAstHelper()->getVariableDomain(astMemberVar) != Ast::DefinitionDomain::OBJECT) {
    expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidGlobalDefAccessNotice>(
      Core::Data::Ast::findSourceLocation(astNode)
    ));
    return false;
  }

  // Get the member generated value and type.
  auto tgMemberVar = session->getEda()->tryGetCodeGenData<TiObject>(astMemberVar);
  if (tgMemberVar == 0) {
    // This situation will only happen if we have circular code generation.
    expGenerator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::CircularUserTypeCodeGenNotice>(
      Core::Data::Ast::findSourceLocation(astNode)
    ));
    return false;
  }

  auto astMemberType = Ast::getAstType(astMemberVar);
  TiObject *tgMemberType;
  if (!g->getGeneratedType(astMemberType, session, tgMemberType, 0)) return false;

  // Generate member access.
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateMemberVarReference(
      session->getTgContext(), tgStructType, tgMemberType, tgMemberVar, tgStructValue, result.targetData
    )) {
      return false;
    }
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astMemberType, Ast::ReferenceMode::IMPLICIT);
  return true;
}


Bool ExpressionGenerator::_generateArrayReference(
  TiObject *self, Core::Data::Node *astNode, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal,
  Ast::Type *astIndexType, Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Cast the index to int64.
  GenResult castedIndex;
  if (session->getTgContext() != 0) {
    if (!g->generateCast(
      session, astIndexType, expGenerator->astHelper->getArchIntType(), astNode, tgIndexVal, false, castedIndex
    )) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
    }
  }

  // Get the deepest reference.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(astType, tgValue, false, false, session, target)) return false;

  // Prepare the array type.
  TiObject *tgArrayType;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    if (!g->getGeneratedType(astType, session, tgArrayType, &astType)) return false;
  } else {
    astType = astRefType->getContentType(expGenerator->astHelper);
    // TargetGenerator expects a pointer type as it doesn't have the concept of references, so we need to
    // get the pointer type for the given reference type.
    auto astPtrType = expGenerator->astHelper->getPointerTypeFor(astType);
    if (!g->getGeneratedType(astPtrType, session, tgArrayType, 0)) return false;
  }
  auto astArrayType = ti_cast<Ast::ArrayType>(astType);
  if (astArrayType == 0) {
    throw EXCEPTION(GenericException, S("Unexpected type for array reference."));
  }

  // Find element type.
  auto astElementType = astArrayType->getContentType(expGenerator->astHelper);
  TiObject *tgElementType;
  if (!g->getGeneratedType(astElementType, session, tgElementType, 0)) return false;

  // Generate member access.
  if (session->getTgContext() != 0) {
    if (!session->getTg()->generateArrayElementReference(
      session->getTgContext(), tgArrayType, tgElementType, castedIndex.targetData.get(), target.targetData.get(),
      result.targetData
    )) return false;
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astElementType, Ast::ReferenceMode::IMPLICIT);
  return true;
}


Bool ExpressionGenerator::_generateFunctionCall(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
  Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee->getInlined()) {
    // TODO: Generate inlined function body.
    throw EXCEPTION(GenericException, S("Inline function generation is not implemented yet."));
  } else {
    // Build called function declaration.
    if (!g->generateFunctionDecl(callee, session)) return false;
    session->getFuncDeps()->add(callee);
    auto tgFunction = session->getEda()->getCodeGenData<TiObject>(callee);

    // Create function call.
    auto astRetType = callee->getType()->traceRetType(expGenerator->astHelper);
    if (astRetType->getInitializationMethod(
      expGenerator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
      if (session->getTgContext() != 0) {
        // Pass a reference to the return value.
        if (!g->generateTempVar(astNode, astRetType, session, false)) return false;
        TioSharedPtr tgResult;
        if (!session->getTg()->generateVarReference(
          session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(astRetType),
          session->getEda()->getCodeGenData<TiObject>(astNode), tgResult
        )) {
          return false;
        }
        PlainList<TiObject> paramTgValuesWithRet;
        paramTgValuesWithRet.add(tgResult.get());
        for (Int i = 0; i < paramTgValues->getElementCount(); ++i) {
          paramTgValuesWithRet.add(paramTgValues->getElement(i));
        }
        if (!session->getTg()->generateFunctionCall(
          session->getTgContext(), tgFunction, &paramTgValuesWithRet, result.targetData
        )) return false;
        result.targetData = tgResult;
        // Register temp var for destruction.
        g->registerDestructor(
          astNode, astRetType, session->getExecutionContext(), session->getDestructionStack()
        );
      }
      result.astType = expGenerator->astHelper->getReferenceTypeFor(astRetType, Ast::ReferenceMode::IMPLICIT);
    } else {
      if (session->getTgContext() != 0) {
        if (!session->getTg()->generateFunctionCall(
          session->getTgContext(), tgFunction, paramTgValues, result.targetData
        )) return false;
      }
      result.astType = astRetType;
    }
    return true;
  }
}


Bool ExpressionGenerator::_generateFunctionPtrCall(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::FunctionType *astFuncType,
  TiObject *tgFuncPtr, TiObject *tgFuncPtrType, Containing<TiObject> *paramTgValues,
  Generation *g, Session *session, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto astRetType = astFuncType->traceRetType(expGenerator->astHelper);
  if (astRetType->getInitializationMethod(
    expGenerator->astHelper, session->getExecutionContext()
  ) != Ast::TypeInitMethod::NONE) {
    if (session->getTgContext() != 0) {
      // Pass a reference to the return value.
      if (!g->generateTempVar(astNode, astRetType, session, false)) return false;
      TioSharedPtr tgResult;
      if (!session->getTg()->generateVarReference(
        session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(astRetType),
        session->getEda()->getCodeGenData<TiObject>(astNode), tgResult
      )) {
        return false;
      }
      PlainList<TiObject> paramTgValuesWithRet;
      paramTgValuesWithRet.add(tgResult.get());
      for (Int i = 0; i < paramTgValues->getElementCount(); ++i) {
        paramTgValuesWithRet.add(paramTgValues->getElement(i));
      }
      if (!session->getTg()->generateFunctionPtrCall(
        session->getTgContext(), tgFuncPtr, tgFuncPtrType, &paramTgValuesWithRet, result.targetData
      )) return false;
      result.targetData = tgResult;
      // Register temp var for destruction.
      g->registerDestructor(
        astNode, astRetType, session->getExecutionContext(), session->getDestructionStack()
      );
    }
    result.astType = expGenerator->astHelper->getReferenceTypeFor(astRetType, Ast::ReferenceMode::IMPLICIT);
  } else {
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateFunctionPtrCall(
        session->getTgContext(), tgFuncPtr, tgFuncPtrType, paramTgValues, result.targetData
      )) return false;
    }
    result.astType = astRetType;
  }
  return true;
}


Bool ExpressionGenerator::_prepareFunctionParams(
  TiObject *self, Spp::Ast::FunctionType *calleeType, Generation *g, Session *session,
  DynamicContaining<TiObject> *paramAstNodes, DynamicContaining<TiObject> *paramAstTypes,
  SharedList<TiObject> *paramTgVals
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  Ast::FunctionType::ArgMatchContext context;
  for (Int i = 0; i < paramTgVals->getElementCount(); ++i) {
    Ast::Type *srcType = static_cast<Ast::Type*>(paramAstTypes->getElement(i));
    auto status = calleeType->matchNextArg(srcType, context, expGenerator->astHelper, session->getExecutionContext());
    ASSERT(status != Ast::TypeMatchStatus::NONE);

    // Cast the value if needed.
    if (context.type != 0) {
      GenResult castedVal;
      if (session->getTgContext() != 0) {
        if (!g->generateCast(
          session, srcType, context.type, ti_cast<Core::Data::Node>(paramAstNodes->getElement(i)),
          paramTgVals->getElement(i), false, castedVal
        )) {
          throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
        };
      }
      paramTgVals->set(i, castedVal.targetData);
    } else {
      // For var args we need to send values, not references.
      GenResult result;
      auto contentType = expGenerator->astHelper->tryGetDeepReferenceContentType(srcType);
      if (contentType->getInitializationMethod(
        expGenerator->astHelper, session->getExecutionContext()
      ) == Ast::TypeInitMethod::NONE) {
        if (!expGenerator->dereferenceIfNeeded(srcType, paramTgVals->getElement(i), true, false, session, result)) {
          throw EXCEPTION(GenericException, S("Unexpected error."));
        }
      } else {
        if (!expGenerator->dereferenceIfNeeded(srcType, paramTgVals->getElement(i), false, false, session, result)) {
          throw EXCEPTION(GenericException, S("Unexpected error."));
        }
      }
      paramTgVals->set(i, result.targetData);
    }
  }
  return true;
}


Bool ExpressionGenerator::_prepareCallee(
  TiObject *self, Core::Data::Node *astNode, Containing<TiObject> *argTypes, Char const *op,
  Generation *g, Session *session, GenResult &calleeResult, GenResult &thisResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  GenResult prevResult;
  Ast::CalleeLookupRequest lookupRequest;
  if (!expGenerator->prepareCalleeLookupRequest(astNode, g, session, prevResult, lookupRequest)) {
    return false;
  }
  lookupRequest.astNode = astNode;
  lookupRequest.op = op;
  lookupRequest.argTypes = argTypes;
  lookupRequest.ec = session->getExecutionContext();

  // Lookup the callee.
  Ast::CalleeLookupResult lookupResult;
  expGenerator->calleeTracer->lookupCallee(lookupRequest, lookupResult);
  if (lookupResult.isFailure()) {
    expGenerator->astHelper->getNoticeStore()->add(lookupResult.notice);
    return false;
  }

  // Generate reference chain.
  return expGenerator->generateCalleeReferenceChain(
    lookupResult, astNode, prevResult, g, session, calleeResult, thisResult
  );
}


Bool ExpressionGenerator::_prepareCalleeLookupRequest(
  TiObject *self, TiObject *operand, Generation *g, Session *session,
  GenResult &prevResult, Ast::CalleeLookupRequest &calleeRequest
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (operand->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    ////
    //// A member of the current scope.
    ////
    calleeRequest.target = static_cast<Core::Data::Ast::Identifier*>(operand)->findOwner<Core::Data::Ast::Scope>();
    calleeRequest.mode = Ast::CalleeLookupMode::DIRECTLY_ACCESSIBLE;
    calleeRequest.ref = operand;
    prevResult = GenResult();
    return true;
  } else if (operand->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    ////
    //// Call a member of a specific object/scope.
    ////
    // Generate the object reference.
    auto linkOperator = static_cast<Core::Data::Ast::LinkOperator*>(operand);
    auto first = linkOperator->getFirst().get();
    if (first == 0) {
      throw EXCEPTION(GenericException, S("First AST element missing from link operator."));
    }
    if (!expGenerator->generate(first, g, session, prevResult)) return false;

    if (prevResult.astType != 0) {
      if (linkOperator->getSecond()->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        // Calling a member of an object.
        auto thisType = expGenerator->astHelper->tryGetDeepReferenceContentType(prevResult.astType);
        auto thisRefType = expGenerator->astHelper->getReferenceTypeFor(thisType, Ast::ReferenceMode::IMPLICIT);
        calleeRequest.target = thisType;
        calleeRequest.mode = Ast::CalleeLookupMode::OBJECT_MEMBER;
        calleeRequest.ref = linkOperator->getSecond().get();
        calleeRequest.thisType = thisRefType;
        return true;
      } else if (linkOperator->getSecond()->isDerivedFrom<Ast::Block>()) {
        // .{} operator
        auto refType = ti_cast<Ast::ReferenceType>(prevResult.astType);
        if (refType == 0) {
          expGenerator->astHelper->getNoticeStore()->add(
            newSrdObj<Spp::Notices::InvalidUseInTargetNotice>(linkOperator->findSourceLocation())
          );
          return false;
        }
        auto contentType = refType->getContentType(expGenerator->getAstHelper());
        auto explicitRefType = expGenerator->astHelper->getReferenceTypeFor(contentType, Ast::ReferenceMode::EXPLICIT);
        auto block = linkOperator->getSecond().s_cast_get<Ast::Block>();
        g->addThisDefinition(block, S("this"), explicitRefType, false, prevResult.targetData, session);
        TerminalStatement terminal = TerminalStatement::UNKNOWN;
        if (!g->generateStatementBlock(block, session, terminal)) return false;
        Ast::Type *prevAstType;
        Ast::Type *prevThisAstType;
        prevAstType = expGenerator->astHelper->tryGetDeepReferenceContentType(contentType);
        if (expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(prevAstType) == 0) {
          prevThisAstType = expGenerator->astHelper->getReferenceTypeFor(prevAstType, Ast::ReferenceMode::IMPLICIT);
        } else {
          prevThisAstType = 0;
        }
        calleeRequest.target = prevAstType;
        calleeRequest.thisType = prevThisAstType;
        calleeRequest.mode = Ast::CalleeLookupMode::OBJECT_MEMBER;
        return true;
      } else {
        expGenerator->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidOperationNotice>(linkOperator->findSourceLocation())
        );
        return false;
      }
    } else if (
      prevResult.astNode != 0 &&
      (prevResult.astNode->isDerivedFrom<Ast::Type>() || prevResult.astNode->isDerivedFrom<Core::Data::Ast::Scope>())
    ) {
      if (linkOperator->getSecond()->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        // Calling a global in another module or type.
        calleeRequest.target = prevResult.astNode;
        calleeRequest.mode = Ast::CalleeLookupMode::SCOPE_MEMBER;
        calleeRequest.ref = linkOperator->getSecond().get();
        return true;
      } else {
        expGenerator->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidOperationNotice>(linkOperator->findSourceLocation())
        );
        return false;
      }
    } else {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidDotOpTargetNotice>(linkOperator->findSourceLocation())
      );
      return false;
    }
  } else {
    if (
      operand->isDerivedFrom<Core::Data::Ast::ParamPass>() &&
      static_cast<Core::Data::Ast::ParamPass*>(operand)->getType() == Core::Data::Ast::BracketType::SQUARE
    ) {
      ////
      //// Call a template function.
      ////
      auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(operand);
      if (!expGenerator->prepareCalleeLookupRequest(
        paramPass->getOperand().get(), g, session, prevResult, calleeRequest
      )) return false;
      calleeRequest.templateParam = paramPass->getParam().get();
      return true;
    } else {
      ////
      //// Call the result of a previous expression.
      ////
      if (!expGenerator->generate(operand, g, session, prevResult)) return false;

      // Lookup a callee on the result.
      Ast::Type *prevAstType;
      Ast::Type *prevThisAstType;
      if (prevResult.astType) {
        prevAstType = expGenerator->astHelper->tryGetDeepReferenceContentType(prevResult.astType);
        // We don't need to set `this` if the value itself is a function pointer since in this case the intention would be
        // to actually call that function.
        if (expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(prevAstType) == 0) {
          prevThisAstType = expGenerator->astHelper->getReferenceTypeFor(prevAstType, Ast::ReferenceMode::IMPLICIT);
        } else {
          prevThisAstType = 0;
        }
        calleeRequest.mode = Ast::CalleeLookupMode::OBJECT_MEMBER;
      } else {
        prevAstType = ti_cast<Ast::Type>(prevResult.astNode);
        prevThisAstType = 0;
      }
      calleeRequest.target = prevAstType;
      calleeRequest.thisType = prevThisAstType;
      return true;
    }
  }
}


Bool ExpressionGenerator::_generateCalleeReferenceChain(
  TiObject *self, Ast::CalleeLookupResult const &calleeInfo, Core::Data::Node *astNode, GenResult const &prevResult,
  Generation *g, Session *session, GenResult &calleeResult, GenResult &thisResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (prevResult.astType != 0) {
    auto prevAstType = expGenerator->astHelper->tryGetDeepReferenceContentType(prevResult.astType);
    if (expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(prevAstType) == 0) {
      thisResult = prevResult;
    }
  }

  calleeResult = prevResult;
  for (Int i = 0; i < calleeInfo.stack.getLength(); ++i) {
    auto item = calleeInfo.stack(i);
    if (item.type == Ast::CalleeLookupResultStackEntryType::THIS_MARKER) {
      thisResult = calleeResult;
    } else if (item.type == Ast::CalleeLookupResultStackEntryType::FUNCTION_CALL) {
      PlainList<TiObject> paramAstTypes;
      SharedList<TiObject> paramTgValues;
      PlainList<TiObject> paramAstNodes;
      GenResult prevCalleeResult = calleeResult;
      if (!expGenerator->generateRoundParamPassOnCallee(
        astNode, prevCalleeResult, thisResult, &paramTgValues, &paramAstTypes, &paramAstNodes, g, session, calleeResult
      )) return false;
      calleeResult.astNode = 0;
    } else {
      if (item.obj->isDerivedFrom<Ast::Function>()) {
        // In case of member function calling we can have function items when calleeResult.astType is not null.
        calleeResult.astNode = item.obj;
        calleeResult.astType = 0;
        calleeResult.targetData.reset();
      } else if (item.obj->isDerivedFrom<Ast::Type>()) {
        // TODO: Capture the correct ~init function to avoid having to look it up again.
        if (!expGenerator->generateReferenceToNonObjectMember(item.obj, astNode, g, session, calleeResult)) {
          return false;
        }
        break;
      } else {
        if (calleeResult.astType != 0) {
          // Generate member var reference.
          GenResult structResult;
          if (!expGenerator->dereferenceIfNeeded(
            calleeResult.astType, calleeResult.targetData.get(), false, false, session, structResult
          )) return false;
          if (!expGenerator->generateMemberVarReference(
            astNode, structResult.targetData.get(), structResult.astType, item.obj, g, session, calleeResult
          )) return false;
        } else {
          // Generate non-member reference.
          if (!expGenerator->generateReferenceToNonObjectMember(item.obj, astNode, g, session, calleeResult)) {
            return false;
          }
        }
      }
    }
  }

  return true;
}


Bool ExpressionGenerator::_referencifyThisIfNeeded(
  TiObject *self, Core::Data::Node *astNode, GenResult const &thisArg,
  Generation *g, Session *session, GenResult &thisResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (thisArg.astType != 0 && ti_cast<Ast::ReferenceType>(thisArg.astType) == 0) {
    if (session->getTgContext() != 0) {
      // Member functions need a reference so we'll create a temp var.
      // This code path should not be reached with custom-initialization variables, so we don't need to
      // register the destructor here.
      if (!g->generateTempVar(astNode, thisArg.astType, session, false)) return false;
      PlainList<TiObject> thisParamAstNodes({ astNode });
      PlainList<TiObject> thisParamAstTypes({ thisArg.astType });
      SharedList<TiObject> thisParamTgValues({ thisArg.targetData });
      if (!session->getTg()->generateVarReference(
        session->getTgContext(), session->getEda()->getCodeGenData<TiObject>(thisArg.astType),
        session->getEda()->getCodeGenData<TiObject>(astNode), thisResult.targetData
      )) {
        return false;
      }
      if (!g->generateVarInitialization(
        thisArg.astType, thisResult.targetData.get(), astNode,
        &thisParamAstNodes, &thisParamAstTypes, &thisParamTgValues, session
      )) return false;
    }
    thisResult.astType = expGenerator->astHelper->getReferenceTypeFor(
      thisArg.astType, Ast::ReferenceMode::IMPLICIT
    );
  } else {
    thisResult = thisArg;
  }

  return true;
}


Bool ExpressionGenerator::_generateParams(
  TiObject *self, TiObject *astNode, Generation *g, Session *session,
  DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes,
  SharedList<TiObject> *resultValues
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (astNode == 0) return true;

  if (astNode->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto scope = static_cast<Core::Data::Ast::Scope*>(astNode);
    // Generate the sub elements.
    SharedList<TiObject> subTgValues;
    PlainList<TiObject> subAstTypes;
    PlainList<TiObject> subAstNodes;
    for (Int i = 0; i < scope->getCount(); ++i) {
      if (!expGenerator->generateParams(scope->getElement(i), g, session, &subAstNodes, &subAstTypes, &subTgValues)) {
        return false;
      }
    }
    // Generate the count
    Ast::Type *intAstType = expGenerator->astHelper->getIntType(32);
    TiObject *intTgType;
    if (!g->getGeneratedType(intAstType, session, intTgType, 0)) return false;
    TioSharedPtr tgCount;
    if (session->getTgContext() != 0) {
      if (!session->getTg()->generateIntLiteral(session->getTgContext(), 32, true, subTgValues.getCount(), tgCount)) {
        return false;
      }
    }
    // Add them all to the result.
    resultAstNodes->addElement(scope);
    resultTypes->addElement(intAstType);
    resultValues->add(tgCount);
    for (Int i = 0; i < subTgValues.getCount(); ++i) {
      resultAstNodes->addElement(subAstNodes.get(i));
      resultTypes->addElement(subAstTypes.get(i));
      resultValues->add(subTgValues.get(i));
    }
    return true;
  } else if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    auto list = static_cast<Core::Data::Ast::List*>(astNode);
    for (Int i = 0; i < list->getElementCount(); ++i) {
      if (!expGenerator->generateParams(list->getElement(i), g, session, resultAstNodes, resultTypes, resultValues)) {
        return false;
      }
    }
  } else {
    GenResult result;
    if (!expGenerator->generate(astNode, g, session, result)) return false;
    if (result.astType == 0) {
      expGenerator->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
    resultValues->add(result.targetData);
    resultTypes->addElement(result.astType);
    resultAstNodes->addElement(astNode);
  }
  return true;
}


//==============================================================================
// Helper Functions

Bool ExpressionGenerator::dereferenceIfNeeded(
  Spp::Ast::Type *astType, TiObject *tgValue, Bool valueNeeded, Bool implicitOnly, Session *session, GenResult &result
) {
  auto refType = ti_cast<Spp::Ast::ReferenceType>(astType);
  if (refType != 0 && refType->isAutoDeref() && (!implicitOnly || refType->isImplicit())) {
    Spp::Ast::Type *cntAstType = refType->getContentType(this->astHelper);
    auto cntRefAstType = ti_cast<Spp::Ast::ReferenceType>(cntAstType);
    Bool innerRefAvailable =
      cntRefAstType != 0 && cntRefAstType->isAutoDeref() && (!implicitOnly || cntRefAstType->isImplicit());
    if (innerRefAvailable || valueNeeded) {
      TioSharedPtr derefTgValue;
      if (session->getTgContext() != 0) {
        auto tgContentType = session->getEda()->getCodeGenData<TiObject>(cntAstType);
        if (!session->getTg()->generateDereference(session->getTgContext(), tgContentType, tgValue, derefTgValue)) {
          return false;
        }
      }
      return this->dereferenceIfNeeded(cntAstType, derefTgValue.get(), valueNeeded, implicitOnly, session, result);
    }
  }
  result.astType = astType;
  result.targetData = getSharedPtr(tgValue);
  return true;
}


Bool ExpressionGenerator::castLogicalOperand(
  Generation *g, Session *session, TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  auto boolType = this->astHelper->getBoolType();
  if (astType == 0) {
    this->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidLogicalOperandNotice>());
    return false;
  }
  if (session->getTgContext() != 0) {
    GenResult castResult;
    if (!g->generateCast(session, astType, boolType, ti_cast<Core::Data::Node>(astNode), tgValue, true, castResult)) {
      this->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
    result = castResult.targetData;
  } else if (!this->astHelper->isImplicitlyCastableTo(astType, boolType, session->getExecutionContext())) {
    this->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} // namespace
