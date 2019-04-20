/**
 * @file Spp/CodeGen/ExpressionGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::ExpressionGenerator.
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

void ExpressionGenerator::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->generate,
    &this->generateList,
    &this->generateIdentifier,
    &this->generateScopeMemberReference,
    &this->generateLinkOperator,
    &this->generateParamPass,
    &this->generateRoundParamPass,
    &this->generateRoundParamPassOnCallee,
    &this->generateRoundParamPassOnResult,
    &this->generateRoundParamPassOnMember,
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
    &this->generateContentOp,
    &this->generateCastOp,
    &this->generateSizeOp,
    &this->generateStringLiteral,
    &this->generateCharLiteral,
    &this->generateIntegerLiteral,
    &this->generateFloatLiteral,
    &this->generateVarReference,
    &this->generateMemberReference,
    &this->generateArrayReference,
    &this->generateFunctionCall
  });
}


void ExpressionGenerator::initBindings()
{
  this->generate = &ExpressionGenerator::_generate;
  this->generateList = &ExpressionGenerator::_generateList;
  this->generateIdentifier = &ExpressionGenerator::_generateIdentifier;
  this->generateScopeMemberReference = &ExpressionGenerator::_generateScopeMemberReference;
  this->generateLinkOperator = &ExpressionGenerator::_generateLinkOperator;
  this->generateParamPass = &ExpressionGenerator::_generateParamPass;
  this->generateRoundParamPass = &ExpressionGenerator::_generateRoundParamPass;
  this->generateRoundParamPassOnCallee = &ExpressionGenerator::_generateRoundParamPassOnCallee;
  this->generateRoundParamPassOnResult = &ExpressionGenerator::_generateRoundParamPassOnResult;
  this->generateRoundParamPassOnMember = &ExpressionGenerator::_generateRoundParamPassOnMember;
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
  this->generateContentOp = &ExpressionGenerator::_generateContentOp;
  this->generateCastOp = &ExpressionGenerator::_generateCastOp;
  this->generateSizeOp = &ExpressionGenerator::_generateSizeOp;
  this->generateStringLiteral = &ExpressionGenerator::_generateStringLiteral;
  this->generateCharLiteral = &ExpressionGenerator::_generateCharLiteral;
  this->generateIntegerLiteral = &ExpressionGenerator::_generateIntegerLiteral;
  this->generateFloatLiteral = &ExpressionGenerator::_generateFloatLiteral;
  this->generateVarReference = &ExpressionGenerator::_generateVarReference;
  this->generateMemberReference = &ExpressionGenerator::_generateMemberReference;
  this->generateArrayReference = &ExpressionGenerator::_generateArrayReference;
  this->generateFunctionCall = &ExpressionGenerator::_generateFunctionCall;
}


//==============================================================================
// Top Level Generation Functions

Bool ExpressionGenerator::_generate(
  TiObject *self, TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    auto expList = static_cast<Core::Data::Ast::List*>(astNode);
    return expGenerator->generateList(expList, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    return expGenerator->generateIdentifier(identifier, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOperator = static_cast<Core::Data::Ast::LinkOperator*>(astNode);
    return expGenerator->generateLinkOperator(linkOperator, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    return expGenerator->generateParamPass(paramPass, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    return expGenerator->generateOperator(infixOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::OutfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
    return expGenerator->generateOperator(outfixOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::PointerOp>()) {
    auto pointerOp = static_cast<Spp::Ast::PointerOp*>(astNode);
    return expGenerator->generatePointerOp(pointerOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::AstRefOp>()) {
    auto astRefOp = static_cast<Spp::Ast::AstRefOp*>(astNode);
    return expGenerator->generateAstRefOp(astRefOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContentOp>()) {
    auto contentOp = static_cast<Spp::Ast::ContentOp*>(astNode);
    return expGenerator->generateContentOp(contentOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::CastOp>()) {
    auto castOp = static_cast<Spp::Ast::CastOp*>(astNode);
    return expGenerator->generateCastOp(castOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::SizeOp>()) {
    auto sizeOp = static_cast<Spp::Ast::SizeOp*>(astNode);
    return expGenerator->generateSizeOp(sizeOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    return expGenerator->generateStringLiteral(stringLiteral, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::CharLiteral>()) {
    auto charLiteral = static_cast<Core::Data::Ast::CharLiteral*>(astNode);
    return expGenerator->generateCharLiteral(charLiteral, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    auto integerLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(astNode);
    return expGenerator->generateIntegerLiteral(integerLiteral, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    auto floatLiteral = static_cast<Core::Data::Ast::FloatLiteral*>(astNode);
    return expGenerator->generateFloatLiteral(floatLiteral, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Data::Ast::Bracket*>(astNode);
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
      return expGenerator->generate(bracket->getOperand().get(), g, tg, tgContext, result);
    } else {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidOperationNotice>(bracket->findSourceLocation())
      );
      return false;
    }
  }
  expGenerator->noticeStore->add(
    std::make_shared<Spp::Notices::UnsupportedOperationNotice>(Core::Data::Ast::findSourceLocation(astNode))
  );
  return false;
}


Bool ExpressionGenerator::_generateList(
  TiObject *self, Core::Data::Ast::List *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  GenResult innerResult;
  for (Int i = 0; i < astNode->getCount(); ++i) {
    auto innerNode = astNode->getElement(i);
    if (innerNode == 0) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidExpressionListNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    if (!expGenerator->generate(innerNode, g, tg, tgContext, innerResult)) return false;
  }
  result = innerResult;
  return true;
}


Bool ExpressionGenerator::_generateIdentifier(
  TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  return expGenerator->generateScopeMemberReference(astNode->getOwner(), astNode, true, g, tg, tgContext, result);
}


Bool ExpressionGenerator::_generateScopeMemberReference(
  TiObject *self, TiObject *scope, Core::Data::Ast::Identifier *astNode, Bool searchOwners,
  Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  Bool retVal = false;
  Bool symbolFound = false;
  expGenerator->astHelper->getSeeker()->foreach(astNode, scope,
    [=, &retVal, &symbolFound, &result]
    (TiObject *obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      // Check if the found obj is a variable definition.
      if (expGenerator->astHelper->isAstReference(obj)) {
        // Make sure the var is not an object member.
        if (expGenerator->getAstHelper()->getDefinitionDomain(obj) == Ast::DefinitionDomain::OBJECT) {
          expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidObjectMemberAccessNotice>(
            Core::Data::Ast::findSourceLocation(astNode)
          ));
          retVal = false;
        } else {
          retVal = expGenerator->generateVarReference(obj, g, tg, tgContext, result);
        }
      } else if (
        obj->isDerivedFrom<Core::Data::Ast::Scope>() || obj->isDerivedFrom<Ast::Type>() ||
        obj->isDerivedFrom<Ast::Template>() || obj->isDerivedFrom<Ast::Function>()
      ) {
        result.astNode = obj;
        retVal = true;
      } else if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        retVal = expGenerator->generateStringLiteral(
          static_cast<Core::Data::Ast::StringLiteral*>(obj), g, tg, tgContext, result
        );
      } else if (obj->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
        retVal = expGenerator->generateIntegerLiteral(
          static_cast<Core::Data::Ast::IntegerLiteral*>(obj), g, tg, tgContext, result
        );
      } else if (obj->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
        retVal = expGenerator->generateFloatLiteral(
          static_cast<Core::Data::Ast::FloatLiteral*>(obj), g, tg, tgContext, result
        );
      } else {
        expGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    },
    searchOwners ? 0 : SeekerExtension::Flags::SKIP_OWNERS_AND_USES
  );

  if (!symbolFound) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::UnknownSymbolNotice>(astNode->findSourceLocation()));
  }
  return retVal;
}


Bool ExpressionGenerator::_generateLinkOperator(
  TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (astNode->getType() != S(".")) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Generate the object reference.
  auto first = astNode->getFirst().get();
  if (first == 0) {
    throw EXCEPTION(GenericException, S("First AST element missing from link operator."));
  }
  GenResult firstResult;
  if (!expGenerator->generate(first, g, tg, tgContext, firstResult)) return false;

  // Get member identifier.
  auto second = astNode->getSecond().ti_cast_get<Core::Data::Ast::Identifier>();
  if (second == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (firstResult.astType != 0) {
    // Generate the member reference.
    return expGenerator->generateMemberReference(
      firstResult.targetData.get(), firstResult.astType, second, g, tg, tgContext, result
    );
  } else if (firstResult.astNode != 0) {
    // Generate a reference to a global in another module.
    return expGenerator->generateScopeMemberReference(firstResult.astNode, second, false, g, tg, tgContext, result);
  } else {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
}


Bool ExpressionGenerator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->getType() == Core::Data::Ast::BracketType::ROUND) {
    return expGenerator->generateRoundParamPass(astNode, g, tg, tgContext, result);
  } else {
    return expGenerator->generateSquareParamPass(astNode, g, tg, tgContext, result);
  }
}


Bool ExpressionGenerator::_generateRoundParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();

  // Prepare parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParamList(param, g, tg, tgContext, &paramAstNodes, &paramAstTypes, &paramTgValues)) {
    return false;
  }

  if (operand->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    ////
    //// Call a member of the current scope.
    ////
    TiObject *callee;
    Ast::Type *calleeType;
    SharedPtr<Core::Notices::Notice> notice;
    if (!expGenerator->astHelper->lookupCallee(
      operand, astNode->getOwner(), true, &paramAstTypes, tg->getExecutionContext(), callee, calleeType, notice
    )) {
      expGenerator->noticeStore->add(notice);
      return false;
    }
    // Make sure the var is not an object member.
    if (expGenerator->getAstHelper()->getDefinitionDomain(callee) == Ast::DefinitionDomain::OBJECT) {
      expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidObjectMemberAccessNotice>(
        Core::Data::Ast::findSourceLocation(astNode)
      ));
      return false;
    }
    return expGenerator->generateRoundParamPassOnCallee(
      callee, calleeType, &paramTgValues, &paramAstTypes, &paramAstNodes, g, tg, tgContext, result
    );
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
    GenResult firstResult;
    if (!expGenerator->generate(first, g, tg, tgContext, firstResult)) return false;
    // Generate the member identifier.
    auto second = linkOperator->getSecond().ti_cast_get<Core::Data::Ast::Identifier>();
    if (second == 0) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidOperationNotice>(linkOperator->findSourceLocation())
      );
      return false;
    }

    if (firstResult.astType != 0) {
      //// Calling a member of another object instance.
      GenResult prevResult;
      if (!expGenerator->generateMemberReference(
        firstResult.targetData.get(), firstResult.astType, second, g, tg, tgContext, prevResult
      )) return false;
      return expGenerator->generateRoundParamPassOnMember(
        linkOperator, &prevResult, &paramTgValues, &paramAstTypes, &paramAstNodes, g, tg, tgContext, result
      );
    } else if (firstResult.astNode != 0) {
      //// Calling a global in another module.
      // Look for a matching callee.
      TiObject *callee;
      Ast::Type *calleeType;
      SharedPtr<Core::Notices::Notice> notice;
      if (!expGenerator->astHelper->lookupCallee(
        second, static_cast<Ast::Module*>(firstResult.astNode), false, &paramAstTypes, tg->getExecutionContext(),
        callee, calleeType, notice
      )) {
        expGenerator->noticeStore->add(notice);
        return false;
      }
      // Make sure the var is not an object member.
      if (expGenerator->getAstHelper()->getDefinitionDomain(callee) == Ast::DefinitionDomain::OBJECT) {
        expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidObjectMemberAccessNotice>(
          Core::Data::Ast::findSourceLocation(second)
        ));
        return false;
      }
      return expGenerator->generateRoundParamPassOnCallee(
        callee, calleeType, &paramTgValues, &paramAstTypes, &paramAstNodes, g, tg, tgContext, result
      );
    } else {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidOperationNotice>(linkOperator->findSourceLocation())
      );
      return false;
    }
  } else {
    ////
    //// Param pass to the result of a previous expression.
    ////
    GenResult prevResult;
    if (!expGenerator->generate(operand, g, tg, tgContext, prevResult)) return false;
    return expGenerator->generateRoundParamPassOnResult(
      astNode, &prevResult, &paramTgValues, &paramAstTypes, &paramAstNodes, g, tg, tgContext, result
    );
  }
}


Bool ExpressionGenerator::_generateRoundParamPassOnCallee(
  TiObject *self, TiObject *callee, Ast::Type *calleeType,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee->isDerivedFrom<Ast::Function>()) {
    //// Call a function.
    ////
    // Prepare the arguments to send.
    if (!expGenerator->prepareFunctionParams(
      static_cast<Ast::Function*>(callee)->getType().get(), g, tg, tgContext,
      paramAstNodes, paramAstTypes, paramTgValues
    )) return false;
    // Generate the function call.
    return expGenerator->generateFunctionCall(
      static_cast<Ast::Function*>(callee), paramAstTypes, paramTgValues, g, tg, tgContext, result
    );
  } else if (calleeType != 0 && calleeType->isDerivedFrom<Ast::PointerType>()) {
    //// Call a function pointer.
    ////
    // Get function type.
    auto astFuncType = expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(calleeType);
    if (astFuncType == 0) {
      throw EXCEPTION(GenericException, S("Invalid callee type."));
    }
    // Prepare the arguments to send.
    if (!expGenerator->prepareFunctionParams(
      astFuncType, g, tg, tgContext, paramAstNodes, paramAstTypes, paramTgValues
    )) return false;
    // Get the target data of the pointer and its type.
    GenResult funcPtrResult;
    if (!expGenerator->generateVarReference(callee, g, tg, tgContext, funcPtrResult)) return false;
    GenResult derefResult;
    if (!expGenerator->dereferenceIfNeeded(
      tg, tgContext, funcPtrResult.astType, funcPtrResult.targetData.get(), derefResult
    )) return false;
    TiObject *tgFuncPtrType = getCodeGenData<TiObject>(derefResult.astType);
    // Generate the call.
    if (tgContext != 0) {
      if (!tg->generateFunctionPtrCall(
        tgContext, derefResult.targetData.get(), tgFuncPtrType, paramTgValues, result.targetData
      )) return false;
    }
    result.astType = astFuncType->traceRetType(expGenerator->astHelper);
    return true;
  } else if (calleeType != 0 && calleeType->isDerivedFrom<Ast::ArrayType>()) {
    //// Reference array element.
    ////
    // Get a reference to the array.
    GenResult arrayResult;
    if (!expGenerator->generateVarReference(callee, g, tg, tgContext, arrayResult)) return false;
    // Reference array element.
    return expGenerator->generateArrayReference(
      arrayResult.targetData.get(), arrayResult.astType,
      paramTgValues->getElement(0), static_cast<Ast::Type*>(paramAstTypes->getElement(0)),
      g, tg, tgContext, result
    );
  } else {
    throw EXCEPTION(GenericException, S("Invalid callee."));
  }
}


Bool ExpressionGenerator::_generateRoundParamPassOnResult(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, GenResult *prevResult,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (expGenerator->astHelper->isTypeOrRefTypeOf<Ast::ArrayType>(prevResult->astType)) {
    ////
    //// Reference element of an array result of the expression.
    ////
    return expGenerator->generateArrayReference(
      prevResult->targetData.get(), prevResult->astType,
      paramTgValues->getElement(0), static_cast<Ast::Type*>(paramAstTypes->getElement(0)),
      g, tg, tgContext, result
    );
  } else {
    auto astFuncType = expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(prevResult->astType);
    if (astFuncType != 0) {
      // Validate parameters.
      if (astFuncType->matchCall(
        paramAstTypes, expGenerator->astHelper, tg->getExecutionContext()
      ) == Ast::TypeMatchStatus::NONE) {
        expGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::ArgsMismatchNotice>(astNode->findSourceLocation())
        );
        return false;
      }
      // Call a function pointer from a previous expression.
      if (!expGenerator->prepareFunctionParams(
        astFuncType, g, tg, tgContext, paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      TiObject *tgFuncPtrType;
      if (!g->getGeneratedType(prevResult->astType, tg, tgFuncPtrType, 0)) return false;
      if (!tg->generateFunctionPtrCall(
        tgContext, prevResult->targetData.get(), tgFuncPtrType, paramTgValues, result.targetData
      )) return false;
      result.astType = astFuncType->traceRetType(expGenerator->astHelper);
      return true;
    } else {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::NoCalleeMatchNotice>(astNode->findSourceLocation())
      );
      return false;
    }
  }
}


Bool ExpressionGenerator::_generateRoundParamPassOnMember(
  TiObject *self, Core::Data::Ast::LinkOperator *linkOperator, GenResult *prevResult,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (expGenerator->astHelper->isTypeOrRefTypeOf<Ast::ArrayType>(prevResult->astType)) {
    //// Reference element of an array result of the expression.
    return expGenerator->generateArrayReference(
      prevResult->targetData.get(), prevResult->astType,
      paramTgValues->getElement(0), static_cast<Ast::Type*>(paramAstTypes->getElement(0)),
      g, tg, tgContext, result
    );
  } else {
    // Dereference the member variable.
    GenResult derefResult;
    if (!expGenerator->dereferenceIfNeeded(
      tg, tgContext, prevResult->astType, prevResult->targetData.get(), derefResult
    )) return false;
    auto astFuncType = expGenerator->astHelper->tryGetPointerContentType<Ast::FunctionType>(derefResult.astType);
    if (astFuncType != 0) {
      // Validate and prepare parameters.
      if (astFuncType->matchCall(
        paramAstTypes, expGenerator->astHelper, tg->getExecutionContext()
      ) == Ast::TypeMatchStatus::NONE) {
        expGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::ArgsMismatchNotice>(linkOperator->findSourceLocation())
        );
        return false;
      }
      if (!expGenerator->prepareFunctionParams(
        astFuncType, g, tg, tgContext, paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      // Generate the call.
      TiObject *tgFuncPtrType = getCodeGenData<TiObject>(derefResult.astType);
      if (tgContext != 0) {
        if (!tg->generateFunctionPtrCall(
          tgContext, derefResult.targetData.get(), tgFuncPtrType, paramTgValues, result.targetData
        )) return false;
      }
      result.astType = astFuncType->traceRetType(expGenerator->astHelper);
      return true;
    } else {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::NoCalleeMatchNotice>(linkOperator->findSourceLocation())
      );
      return false;
    }
  }
}


Bool ExpressionGenerator::_generateSquareParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();

  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astNode != 0 && operandResult.astNode->isDerivedFrom<Spp::Ast::Template>()) {
    auto tpl = static_cast<Spp::Ast::Template*>(operandResult.astNode);
    TioSharedPtr tplInstance;
    if (!tpl->matchInstance(astNode->getParam().get(), expGenerator->astHelper, tplInstance)) return false;
    result.astNode = tplInstance.get();
    return true;
  } else {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
}


Bool ExpressionGenerator::_generateOperator(
  TiObject *self, Core::Data::Node *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
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
    if (infixOp->getType() == S("+")) { funcName = S("__add"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("-")) { funcName = S("__sub"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("*")) { funcName = S("__mul"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("/")) { funcName = S("__div"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S("%")) { funcName = S("__rem"); opType = OpType::ARITHMETIC; }
    else if (infixOp->getType() == S(">>")) { funcName = S("__shr"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("<<")) { funcName = S("__shl"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("&")) { funcName = S("__and"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("|")) { funcName = S("__or"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("$")) { funcName = S("__xor"); opType = OpType::BINARY; }
    else if (infixOp->getType() == S("==")) { funcName = S("__equal"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("!=")) { funcName = S("__notEqual"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S(">")) { funcName = S("__greaterThan"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S(">=")) { funcName = S("__greaterThanOrEqual"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("<")) { funcName = S("__lessThan"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("<=")) { funcName = S("__lessThanOrEqual"); opType = OpType::COMPARISON; }
    else if (infixOp->getType() == S("=")) { funcName = S("__assign"); opType = OpType::ASSIGN; }
    else if (infixOp->getType() == S("+=")) { funcName = S("__addAssign"); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("-=")) { funcName = S("__subAssign"); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("*=")) { funcName = S("__mulAssign"); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("/=")) { funcName = S("__divAssign"); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S("%=")) { funcName = S("__remAssign"); opType = OpType::ARITHMETIC_ASSIGN; }
    else if (infixOp->getType() == S(">>=")) { funcName = S("__shrAssign"); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("<<=")) { funcName = S("__shlAssign"); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("&=")) { funcName = S("__andAssign"); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("|=")) { funcName = S("__orAssign"); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("$=")) { funcName = S("__xorAssign"); opType = OpType::BINARY_ASSIGN; }
    else if (infixOp->getType() == S("||") || infixOp->getType() == S("or")) {
      return expGenerator->generateLogicalOp(infixOp, g, tg, tgContext, result);
    } else if (infixOp->getType() == S("&&") || infixOp->getType() == S("and")) {
      return expGenerator->generateLogicalOp(infixOp, g, tg, tgContext, result);
    } else {
      throw EXCEPTION(GenericException, S("Unexpected infix operator."));
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::PrefixOperator*>(astNode);
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
  if (!expGenerator->generateParamList(
    ti_cast<Containing<TiObject>>(astNode), g, tg, tgContext, &paramAstNodes, &paramAstTypes, &paramTgValues
  )) return false;

  // Look for a matching function to call.
  TiObject *callee;
  Ast::Type *calleeType;
  Ast::Function *function = 0;
  SharedPtr<Core::Notices::Notice> notice;
  if (expGenerator->astHelper->lookupCalleeByName(
    funcName, Core::Data::Ast::findSourceLocation(astNode), astNode->getOwner(), true,
    &paramAstTypes, tg->getExecutionContext(), callee, calleeType, notice
  )) function = ti_cast<Ast::Function>(callee);

  if (function != 0) {
    // Prepare the arguments to send.
    if (!expGenerator->prepareFunctionParams(
      function->getType().get(), g, tg, tgContext, &paramAstNodes, &paramAstTypes, &paramTgValues
    )) return false;

    // Generate the functionc all.
    return expGenerator->generateFunctionCall(function, &paramAstTypes, &paramTgValues, g, tg, tgContext, result);
  } else {
    // We have no function for this op, so fall back to default implementation.
    if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::ARITHMETIC) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::BINARY) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::COMPARISON) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateComparisonOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::ARITHMETIC_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticAssignOp(
        infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result
      );
    } else if (opType == OpType::BINARY_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::INT_UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateIntUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else if (opType == OpType::UNARY_VAR) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryVarOp(outfixOp, &paramTgValues, &paramAstTypes, g, tg, tgContext, result);
    } else {
      if (notice != 0) {
        expGenerator->noticeStore->add(notice);
      } else {
        expGenerator->noticeStore->add(std::make_shared<Spp::Notices::NoCalleeMatchNotice>(
          Core::Data::Ast::findSourceLocation(astNode)
        ));
      }
      return false;
    }
  }
}


Bool ExpressionGenerator::_generateLogicalOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the operator.
  TioSharedPtr secondContext;
  if (tgContext != 0) {
    if (!tg->prepareLogicalOp(tgContext, secondContext)) return false;
  }

  // Generate 1st operand.
  GenResult firstResult;
  if (!expGenerator->generate(astNode->getFirst().get(), g, tg, tgContext, firstResult)) return false;

  // Cast 1st operand to boolean.
  TioSharedPtr firstCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, tg, tgContext, astNode->getFirst().get(), firstResult.astType, firstResult.targetData.get(), firstCastedResult
  )) return false;

  // Generate 2nd operand.
  GenResult secondResult;
  if (!expGenerator->generate(astNode->getSecond().get(), g, tg, secondContext.get(), secondResult)) return false;

  // Cast 2nd operand to boolean.
  TioSharedPtr secondCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, tg, secondContext.get(), astNode->getSecond().get(), secondResult.astType, secondResult.targetData.get(),
    secondCastedResult
  )) return false;

  // Finalize the operator.
  if (astNode->getType() == S("||") || astNode->getType() == S("or")) {
    if (tgContext != 0) {
      if (!tg->finishLogicalOr(
        tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("&&") || astNode->getType() == S("and")) {
    if (tgContext != 0) {
      if (!tg->finishLogicalAnd(
        tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
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
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param2
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
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper);
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper);
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (targetBitCount < 32) targetBitCount = 32;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    }
  } else {
    // Error.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param1.astType, astTargetType, param1.targetData.get(), param1.targetData)) {
      return false;
    }
    if (!g->generateCast(tg, tgContext, param2.astType, astTargetType, param2.targetData.get(), param2.targetData)) {
      return false;
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("+")) {
    if (tgContext != 0) {
      if (!tg->generateAdd(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("-")) {
    if (tgContext != 0) {
      if (!tg->generateSub(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("*")) {
    if (tgContext != 0) {
      if (!tg->generateMul(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("/")) {
    if (tgContext != 0) {
      if (!tg->generateDiv(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("%")) {
    if (tgContext != 0) {
      if (!tg->generateRem(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
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
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param2
  )) return false;
  Ast::Type *astTargetType = 0;

  if (param1.astType->isDerivedFrom<Ast::IntegerType>() && param2.astType->isDerivedFrom<Ast::IntegerType>()) {
    // Two integers.
    auto integerType1 = static_cast<Ast::IntegerType*>(param1.astType);
    auto integerType2 = static_cast<Ast::IntegerType*>(param2.astType);
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper);
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper);
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    }
  } else {
    // Error.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param1.astType, astTargetType, param1.targetData.get(), param1.targetData)) {
      return false;
    }
    if (!g->generateCast(tg, tgContext, param2.astType, astTargetType, param2.targetData.get(), param2.targetData)) {
      return false;
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>")) {
    if (tgContext != 0) {
      if (!tg->generateShr(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("<<")) {
    if (tgContext != 0) {
      if (!tg->generateShl(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("&")) {
    if (tgContext != 0) {
      if (!tg->generateAnd(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("|")) {
    if (tgContext != 0) {
      if (!tg->generateOr(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("$")) {
    if (tgContext != 0) {
      if (!tg->generateXor(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
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
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param2
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
    auto bitCount1 = integerType1->getBitCount(expGenerator->astHelper);
    auto bitCount2 = integerType2->getBitCount(expGenerator->astHelper);
    auto targetBitCount = bitCount1 >= bitCount2 ? bitCount1 : bitCount2;
    if (!integerType1->isSigned() && !integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getWordType(targetBitCount);
    } else if (integerType1->isSigned() && integerType2->isSigned()) {
      astTargetType = expGenerator->astHelper->getIntType(targetBitCount);
    } else if (bitCount2 == 1) {
      astTargetType = integerType1;
    } else if (bitCount1 == 1) {
      astTargetType = integerType2;
    } else {
      // error
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
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
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param1.astType, astTargetType, param1.targetData.get(), param1.targetData)) {
      return false;
    }
    if (!g->generateCast(tg, tgContext, param2.astType, astTargetType, param2.targetData.get(), param2.targetData)) {
      return false;
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("==")) {
    if (tgContext != 0) {
      if (!tg->generateEqual(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("!=")) {
    if (tgContext != 0) {
      if (!tg->generateNotEqual(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">")) {
    if (tgContext != 0) {
      if (!tg->generateGreaterThan(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">=")) {
    if (tgContext != 0) {
      if (!tg->generateGreaterThanOrEqual(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<")) {
    if (tgContext != 0) {
      if (!tg->generateLessThan(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<=")) {
    if (tgContext != 0) {
      if (!tg->generateLessThanOrEqual(
        tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  auto astRefType = ti_cast<Ast::ReferenceType>(paramAstTypes->get(0));
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (
    (!astContentType->isDerivedFrom<Ast::FloatType>() && !astContentType->isDerivedFrom<Ast::IntegerType>()) ||
    (!param.astType->isDerivedFrom<Ast::FloatType>() && !param.astType->isDerivedFrom<Ast::IntegerType>())
  ) {
    if (!param.astType->isImplicitlyCastableTo(astContentType, expGenerator->astHelper, tg->getExecutionContext())) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
      );
      return false;
    }
  }

  // Cast the value to the destination type.
  auto sourceLocation = Core::Data::Ast::findSourceLocation(astNode);
  expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation.get());
  Bool retVal;
  if (tgContext != 0) {
    retVal = g->generateCast(tg, tgContext, param.astType, astContentType, param.targetData.get(), param.targetData);
  } else {
    retVal = true;
  }
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation.get())
  );
  if (!retVal) return false;

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("=")) {
    if (tgContext != 0) {
      if (!tg->generateAssign(
        tgContext, tgContentType, param.targetData.get(), paramTgValues->getElement(0), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  auto astRefType = ti_cast<Ast::ReferenceType>(paramAstTypes->get(0));
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (
    (!astContentType->isDerivedFrom<Ast::FloatType>() && !astContentType->isDerivedFrom<Ast::IntegerType>()) ||
    (!param.astType->isDerivedFrom<Ast::FloatType>() && !param.astType->isDerivedFrom<Ast::IntegerType>())
  ) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param.astType, astContentType, param.targetData.get(), param.targetData)) {
      return false;
    }
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("+=")) {
    if (tgContext != 0) {
      if (!tg->generateAddAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("-=")) {
    if (tgContext != 0) {
      if (!tg->generateSubAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("*=")) {
    if (tgContext != 0) {
      if (!tg->generateMulAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("/=")) {
    if (tgContext != 0) {
      if (!tg->generateDivAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("%=")) {
    if (tgContext != 0) {
      if (!tg->generateRemAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateBinaryAssignOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 2) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly two elements."), paramAstTypes->getCount()
    );
  }

  auto astRefType = ti_cast<Ast::ReferenceType>(paramAstTypes->get(0));
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (!astContentType->isDerivedFrom<Ast::IntegerType>() || !param.astType->isDerivedFrom<Ast::IntegerType>()) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param.astType, astContentType, param.targetData.get(), param.targetData)) {
      return false;
    }
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>=")) {
    if (tgContext != 0) {
      if (!tg->generateShrAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("<<=")) {
    if (tgContext != 0) {
      if (!tg->generateShlAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("&=")) {
    if (tgContext != 0) {
      if (!tg->generateAndAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("|=")) {
    if (tgContext != 0) {
      if (!tg->generateOrAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("$=")) {
    if (tgContext != 0) {
      if (!tg->generateXorAssign(
        tgContext, tgContentType, paramTgValues->getElement(0), param.targetData.get(), result.targetData
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
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
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
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), param
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
    auto bitCount = integerType->getBitCount(expGenerator->astHelper);
    astTargetType = expGenerator->astHelper->getIntType(bitCount);
  } else {
    // Error.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param.astType, astTargetType, param.targetData.get(), param.targetData)) {
      return false;
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("-")) {
    if (tgContext != 0) {
      if (!tg->generateNeg(tgContext, tgTargetType, param.targetData.get(), result.targetData)) return false;
    }
    result.astType = astTargetType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateIntUnaryValOp(
  TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
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
    tg, tgContext, static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), param
  )) return false;

  Ast::IntegerType *astTargetType = 0;
  if (param.astType->isDerivedFrom<Ast::IntegerType>()) {
    astTargetType = static_cast<Ast::IntegerType*>(param.astType);
  } else {
    // Error.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Limit logical not to boolean types.
  if (
    (astNode->getType() == S("!!") || astNode->getType() == S("not")) &&
    astTargetType->getBitCount(expGenerator->astHelper) != 1
  ) {
    // Error.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, param.astType, astTargetType, param.targetData.get(), param.targetData)) {
      return false;
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("!") || astNode->getType() == S("!!") || astNode->getType() == S("not")) {
    if (tgContext != 0) {
      if (!tg->generateNot(tgContext, tgTargetType, param.targetData.get(), result.targetData)) return false;
    }
    result.astType = astTargetType;
    return true;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
  }
}


Bool ExpressionGenerator::_generateUnaryVarOp(
  TiObject *self, Core::Data::Ast::OutfixOperator *astNode,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  VALIDATE_NOT_NULL(astNode, paramTgValues, paramAstTypes);
  if (paramAstTypes->getCount() != 1) {
    throw EXCEPTION(InvalidArgumentException,
      S("paramAstTypes"), S("Must contain exactly one element."), paramAstTypes->getCount()
    );
  }

  auto astRefType = ti_cast<Ast::ReferenceType>(paramAstTypes->get(0));
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (!astContentType->isDerivedFrom<Ast::IntegerType>() && !astContentType->isDerivedFrom<Ast::FloatType>()) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    if (astNode->getType() == S("--")) {
      if (tgContext != 0) {
        if (!tg->generateEarlyDec(
          tgContext, tgContentType, paramTgValues->getElement(0), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (tgContext != 0) {
        if (!tg->generateEarlyInc(
          tgContext, tgContentType, paramTgValues->getElement(0), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else {
      throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
    }
  } else {
    if (astNode->getType() == S("--")) {
      if (tgContext != 0) {
        if (!tg->generateLateDec(
          tgContext, tgContentType, paramTgValues->getElement(0), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (tgContext != 0) {
        if (!tg->generateLateInc(
          tgContext, tgContentType, paramTgValues->getElement(0), result.targetData
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
  TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astType == 0) {
    // We have an AST node.
    // Check if the node is a function.
    auto astFunction = ti_cast<Ast::Function>(operandResult.astNode);
    if (astFunction == 0) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
      );
      return false;
    }
    // Generate a function pointer.
    if (!g->generateFunction(astFunction, tg)) return false;
    auto tgFunction = getCodeGenData<TiObject>(astFunction);
    auto astFunctionPointerType = expGenerator->astHelper->getPointerTypeFor(astFunction->getType().get());
    TiObject *tgFunctionPointerType;
    if (!g->getGeneratedType(astFunctionPointerType, tg, tgFunctionPointerType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate function pointer type."));
    }
    if (tgContext != 0) {
      if (!tg->generateFunctionPointer(tgContext, tgFunction, tgFunctionPointerType, result.targetData)) return false;
    }
    result.astType = astFunctionPointerType;
    return true;
  } else {
    // We have an expression.
    auto operandRefAstType = ti_cast<Ast::ReferenceType>(operandResult.astType);
    if (operandRefAstType == 0) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
      );
      return false;
    }

    // Get the pointer type.
    result.astType = expGenerator->astHelper->getPointerTypeForReferenceType(operandRefAstType);
    if (result.astType == 0) return false;
    result.targetData = operandResult.targetData;
    return true;
  }
}


Bool ExpressionGenerator::_generateAstRefOp(
  TiObject *self, Spp::Ast::AstRefOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("AstRefOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astNode != 0) {
    // We have an AST node.
    // Generate pointer to void.
    auto voidType = expGenerator->astHelper->getVoidType();
    auto voidPtrType = expGenerator->astHelper->getPointerTypeFor(voidType);
    TiObject *tgVoidPtrType;
    if (!g->getGeneratedType(voidPtrType, tg, tgVoidPtrType, 0)) {
      return false;
    }
    // Generate a pointer literal.
    if (!tg->generatePointerLiteral(tgContext, tgVoidPtrType, operandResult.astNode, result.targetData)) {
      return false;
    }
    result.astType = voidPtrType;
    return true;
  } else {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
}


Bool ExpressionGenerator::_generateContentOp(
  TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the pointer itself, not a reference to a pointer.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, operandResult.astType, operandResult.targetData.get(), derefResult
  )) return false;

  // Did we end up with a pointer type?
  auto operandPtrAstType = ti_cast<Ast::PointerType>(derefResult.astType);
  if (operandPtrAstType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Get the reference type.
  result.astType = expGenerator->astHelper->getReferenceTypeForPointerType(operandPtrAstType);
  if (result.astType == 0) return false;
  result.targetData = derefResult.targetData;
  return true;
}


Bool ExpressionGenerator::_generateCastOp(
  TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the value itself, not a reference to it.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    tg, tgContext, operandResult.astType, operandResult.targetData.get(), derefResult
  )) return false;

  // Get the target type.
  auto targetAstType = expGenerator->astHelper->traceType(astNode->getTargetType().get());
  if (targetAstType == 0) return false;
  if (!derefResult.astType->isExplicitlyCastableTo(targetAstType, expGenerator->astHelper, tg->getExecutionContext())) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidCastNotice>(astNode->getSourceLocation()));
    return false;
  }

  // Cast the value.
  if (tgContext != 0) {
    if (!g->generateCast(
      tg, tgContext, derefResult.astType, targetAstType, derefResult.targetData.get(), result.targetData
    )) return false;
  }
  result.astType = targetAstType;
  return true;
}


Bool ExpressionGenerator::_generateSizeOp(
  TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Get the operand type.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, 0, operandResult)) return false;
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
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidSizeOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference if needed.
  if (astType->isDerivedFrom<Ast::ReferenceType>()) {
    astType = static_cast<Ast::ReferenceType*>(astType)->getContentType(expGenerator->astHelper);
  }

  // Get the allocation size.
  auto sourceLocation = Core::Data::Ast::findSourceLocation(operand).get();
  if (sourceLocation != 0) expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation);
  Word size;
  auto retVal = g->getTypeAllocationSize(astType, tg, size);
  if (sourceLocation != 0) {
    expGenerator->noticeStore->popPrefixSourceLocation(Core::Data::getSourceLocationRecordCount(sourceLocation));
  }
  if (!retVal) return false;

  // Generate the Word64 type needed for the result.
  auto bitCount = expGenerator->astHelper->getNeededWordSize(size);
  if (bitCount < 8) bitCount = 8;
  auto astWordType = expGenerator->astHelper->getWordType(bitCount);

  // Generate a constant with that size.
  if (tgContext != 0) {
    if (!tg->generateIntLiteral(tgContext, bitCount, false, size, result.targetData)) return false;
  }
  result.astType = astWordType;
  return true;
}


Bool ExpressionGenerator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = &astNode->getValue().getStr();

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, tg, charTgType, 0)) return false;

  auto strAstType = expGenerator->astHelper->getCharArrayType(value->size() + 1);
  TiObject *strTgType;
  if (!g->getGeneratedType(strAstType, tg, strTgType, 0)) return false;

  auto strPtrAstType = expGenerator->astHelper->getPointerTypeFor(strAstType);
  TiObject *strPtrTgType;
  if (!g->getGeneratedType(strPtrAstType, tg, strPtrTgType, 0)) return false;

  if (tgContext != 0) {
    if (!tg->generateStringLiteral(tgContext, value->c_str(), charTgType, strTgType, result.targetData)) return false;
  }
  result.astType =  strPtrAstType;
  return true;
}


Bool ExpressionGenerator::_generateCharLiteral(
  TiObject *self, Core::Data::Ast::CharLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = astNode->getValue().getStr()[0];

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, tg, charTgType, 0)) return false;

  auto bitCount = charAstType->getBitCount(expGenerator->astHelper);

  if (tgContext != 0) {
    if (!tg->generateIntLiteral(tgContext, bitCount, false, value, result.targetData)) return false;
  }
  result.astType = charAstType;
  return true;
}


Bool ExpressionGenerator::_generateIntegerLiteral(
  TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
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
  // Give special treatment to 0 and 1 and consider it unsigned.
  Bool signedNum = value == 0 || value == 1 ? false : true;
  if (*src == C('u') || *src == C('U')) {
    signedNum = false;
    ++src;
  } else if (compareStr(src, S("ط"), 2) == 0) {
    signedNum = false;
    src += 2;
  }

  // Determine integer size.
  Bool typeRequested = false;
  Int size = signedNum ?
    expGenerator->astHelper->getNeededIntSize((LongInt)value) :
    expGenerator->astHelper->getNeededWordSize(value);
  if (*src == C('i') || *src == C('I')) {
    typeRequested = true;
    ++src;
    if (getStrLen(src) > 0) size = std::stoi(src);
  } else if (compareStr(src, S("ص"), 2) == 0) {
    typeRequested = true;
    src += 2;
    if (getStrLen(src) > 0) size = std::stoi(src);
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
  expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation);
  TiObject *intTgType;
  Bool retVal = g->getGeneratedType(astType, tg, intTgType, 0);
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (tgContext != 0) {
    if (!tg->generateIntLiteral(tgContext, size, signedNum, value, result.targetData)) return false;
  }
  result.astType = astType;
  return true;
}


Bool ExpressionGenerator::_generateFloatLiteral(
  TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
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
  expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation);
  TiObject *floatTgType;
  Bool retVal = g->getGeneratedType(floatAstType, tg, floatTgType, 0);
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (tgContext != 0) {
    if (!tg->generateFloatLiteral(tgContext, size, value, result.targetData)) return false;
  }
  result.astType = floatAstType;
  return true;
}


//==============================================================================
// Inner Generation Functions

Bool ExpressionGenerator::_generateVarReference(
  TiObject *self, TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto tgVar = tryGetCodeGenData<TiObject>(astNode);
  if (tgVar == 0) {
    // Generate the variable definition.
    auto varDef = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(astNode)->getOwner());
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Unexpected error while looking for variable definition."));
    }
    if (!g->generateVarDef(varDef, tg)) return false;

    tgVar = getCodeGenData<TiObject>(astNode);
  }

  auto astType = Ast::getAstType(astNode);
  TiObject *tgType;
  if (!g->getGeneratedType(astType, tg, tgType, 0)) return false;

  if (tgContext != 0) {
    if (!tg->generateVarReference(tgContext, tgType, tgVar, result.targetData)) return false;
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astType);
  return true;
}


Bool ExpressionGenerator::_generateMemberReference(
  TiObject *self, TiObject *tgValue, Ast::Type * astType, Core::Data::Ast::Identifier *astNode,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the struct type.
  Ast::Type *astStructType;
  TiObject *tgStructType;
  auto astRefType = ti_cast<Ast::ReferenceType>(astType);
  if (astRefType == 0) {
    if (!g->getGeneratedType(astType, tg, tgStructType, &astStructType)) return false;
  } else {
    astStructType = astRefType->getContentType(expGenerator->astHelper);
    // TargetGenerator expects a pointer type as it doesn't have the concept of references, so we need to
    // get the pointer type for the given reference type.
    auto astPtrType = expGenerator->astHelper->getPointerTypeFor(astStructType);
    if (!g->getGeneratedType(astPtrType, tg, tgStructType, 0)) return false;
  }
  if (!astStructType->isDerivedFrom<Ast::DataType>()) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Find the member variable.
  auto body = static_cast<Ast::DataType*>(astStructType)->getBody().get();
  if (body == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  TiObject *astMemberVar;
  if (!expGenerator->astHelper->getSeeker()->tryGet(
    astNode, body, astMemberVar, SeekerExtension::Flags::SKIP_OWNERS_AND_USES
  )) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  auto astMemberVarNode = ti_cast<Core::Data::Node>(astMemberVar);
  if (astMemberVarNode == 0 || astMemberVarNode->getOwner() == 0 || astMemberVarNode->getOwner()->getOwner() != body) {
    // The found member was probably an alias to a non member.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  // Make sure the var is an object member.
  if (expGenerator->getAstHelper()->getDefinitionDomain(astMemberVarNode) != Ast::DefinitionDomain::OBJECT) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidGlobalDefAccessNotice>(
      Core::Data::Ast::findSourceLocation(astNode)
    ));
    return false;
  }

  // Get the member generated value and type.
  auto tgMemberVar = getCodeGenData<TiObject>(astMemberVar);
  auto astMemberType = Ast::getAstType(astMemberVar);
  TiObject *tgMemberType;
  if (!g->getGeneratedType(astMemberType, tg, tgMemberType, 0)) return false;

  // Generate member access.
  if (tgContext != 0) {
    if (!tg->generateMemberVarReference(
      tgContext, tgStructType, tgMemberType, tgMemberVar, tgValue, result.targetData
    )) {
      return false;
    }
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astMemberType);
  return true;
}


Bool ExpressionGenerator::_generateArrayReference(
  TiObject *self, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal, Ast::Type *astIndexType,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Cast the index to int64.
  TioSharedPtr tgCastedIndex;
  if (tgContext != 0) {
    if (!g->generateCast(
      tg, tgContext, astIndexType, expGenerator->astHelper->getInt64Type(), tgIndexVal, tgCastedIndex
    )) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
    }
  }

  // Prepare the array type.
  TiObject *tgArrayType;
  auto astRefType = ti_cast<Ast::ReferenceType>(astType);
  if (astRefType == 0) {
    if (!g->getGeneratedType(astType, tg, tgArrayType, &astType)) return false;
  } else {
    astType = astRefType->getContentType(expGenerator->astHelper);
    // TargetGenerator expects a pointer type as it doesn't have the concept of references, so we need to
    // get the pointer type for the given reference type.
    auto astPtrType = expGenerator->astHelper->getPointerTypeFor(astType);
    if (!g->getGeneratedType(astPtrType, tg, tgArrayType, 0)) return false;
  }
  auto astArrayType = ti_cast<Ast::ArrayType>(astType);
  if (astArrayType == 0) {
    throw EXCEPTION(GenericException, S("Unexpected type for array reference."));
  }

  // Find element type.
  auto astElementType = astArrayType->getContentType(expGenerator->astHelper);
  TiObject *tgElementType;
  if (!g->getGeneratedType(astElementType, tg, tgElementType, 0)) return false;

  // Generate member access.
  if (tgContext != 0) {
    if (!tg->generateArrayElementReference(
      tgContext, tgArrayType, tgElementType, tgCastedIndex.get(), tgValue, result.targetData)) {
      return false;
    }
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astElementType);
  return true;
}


Bool ExpressionGenerator::_generateFunctionCall(
  TiObject *self, Spp::Ast::Function *callee,
  Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee->getInlined()) {
    // TODO: Generate inlined function body.
    throw EXCEPTION(GenericException, S("Inline function generation is not implemented yet."));
  } else {
    // Build called funcion.
    if (!g->generateFunction(callee, tg)) return false;
    auto tgFunction = getCodeGenData<TiObject>(callee);

    // Create function call.
    if (tgContext != 0) {
      if (!tg->generateFunctionCall(tgContext, tgFunction, paramTgValues, result.targetData)) return false;
    }
    result.astType = callee->getType()->traceRetType(expGenerator->astHelper);
    return true;
  }
}


//==============================================================================
// Helper Functions

Bool ExpressionGenerator::generateParamList(
  TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes,
  SharedList<TiObject> *resultValues
) {
  if (astNode == 0) return true;

  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    if (!this->generateParamList(
      ti_cast<Containing<TiObject>>(astNode), g, tg, tgContext, resultAstNodes, resultTypes, resultValues
    )) return false;
  } else {
    GenResult result;
    if (!this->generate(astNode, g, tg, tgContext, result)) return false;
    if (result.astType == 0) {
      this->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
    resultValues->add(result.targetData);
    resultTypes->addElement(result.astType);
    resultAstNodes->addElement(astNode);
  }
  return true;
}


Bool ExpressionGenerator::generateParamList(
  Containing<TiObject> *astNodes, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes, SharedList<TiObject> *resultValues
) {
  for (Int i = 0; i < astNodes->getElementCount(); ++i) {
    GenResult result;
    if (!this->generate(astNodes->getElement(i), g, tg, tgContext, result)) return false;
    if (result.astType == 0) {
      this->noticeStore->add(std::make_shared<Spp::Notices::InvalidReferenceNotice>(
        Core::Data::Ast::findSourceLocation(astNodes->getElement(i))
      ));
      return false;
    }
    resultValues->add(result.targetData);
    resultTypes->addElement(result.astType);
    resultAstNodes->addElement(astNodes->getElement(i));
  }
  return true;
}


Bool ExpressionGenerator::prepareFunctionParams(
  Spp::Ast::FunctionType *calleeType, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  DynamicContaining<TiObject> *paramAstNodes, DynamicContaining<TiObject> *paramAstTypes, SharedList<TiObject> *paramTgVals
) {
  Ast::FunctionType::ArgMatchContext context;
  for (Int i = 0; i < paramTgVals->getElementCount(); ++i) {
    Ast::Type *srcType = static_cast<Ast::Type*>(paramAstTypes->getElement(i));
    auto status = calleeType->matchNextArg(srcType, context, this->astHelper, tg->getExecutionContext());
    ASSERT(status != Ast::TypeMatchStatus::NONE);

    // Cast the value if needed.
    if (context.type != 0) {
      auto sourceLocation = Core::Data::Ast::findSourceLocation(paramAstNodes->getElement(i));
      this->noticeStore->pushPrefixSourceLocation(sourceLocation.get());
      TioSharedPtr tgCastedVal;
      Bool castRes;
      if (tgContext != 0) {
        castRes = g->generateCast(tg, tgContext, srcType, context.type, paramTgVals->getElement(i), tgCastedVal);
      } else {
        castRes = true;
      }
      this->noticeStore->popPrefixSourceLocation(
        Core::Data::getSourceLocationRecordCount(sourceLocation.get())
      );
      if (!castRes) return false;
      paramTgVals->set(i, tgCastedVal);
    } else {
      // For var args we need to send values, not references.
      GenResult result;
      if (!this->dereferenceIfNeeded(tg, tgContext, srcType, paramTgVals->getElement(i), result)) {
        throw EXCEPTION(GenericException, S("Unexpected error."));
      }
      paramTgVals->set(i, result.targetData);
    }
  }
  return true;
}


Bool ExpressionGenerator::dereferenceIfNeeded(
  TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *astType, TiObject *tgValue, GenResult &result
) {
  auto refType = ti_cast<Spp::Ast::ReferenceType>(astType);
  if (refType != 0) {
    result.astType = refType->getContentType(this->astHelper);
    auto tgContentType = getCodeGenData<TiObject>(result.astType);
    if (tgContext != 0) {
      return tg->generateDereference(tgContext, tgContentType, tgValue, result.targetData);
    } else {
      return true;
    }
  } else {
    result.astType = astType;
    result.targetData = getSharedPtr(tgValue);
    return true;
  }
}


Bool ExpressionGenerator::castLogicalOperand(
  Generation *g, TargetGeneration *tg, TiObject *tgContext, TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  auto boolType = this->astHelper->getBoolType();
  if (astType == 0 || !astType->isImplicitlyCastableTo(boolType, this->astHelper, tg->getExecutionContext())) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }
  if (tgContext != 0) {
    if (!g->generateCast(tg, tgContext, astType, this->astHelper->getBoolType(), tgValue, result)) {
      this->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  }

  return true;
}

} } // namespace
