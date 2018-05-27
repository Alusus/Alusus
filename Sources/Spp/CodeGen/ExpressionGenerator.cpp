/**
 * @file Spp/CodeGen/ExpressionGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::ExpressionGenerator.
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
    &this->generateSquareParamPass,
    &this->generateOperator,
    &this->generateLogicalOperator,
    &this->generateAssignment,
    &this->generatePointerOp,
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
    &this->generateFunctionCall,
    &this->generateBuiltInFunctionCall,
    &this->generateUserFunctionCall
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
  this->generateSquareParamPass = &ExpressionGenerator::_generateSquareParamPass;
  this->generateOperator = &ExpressionGenerator::_generateOperator;
  this->generateLogicalOperator = &ExpressionGenerator::_generateLogicalOperator;
  this->generateAssignment = &ExpressionGenerator::_generateAssignment;
  this->generatePointerOp = &ExpressionGenerator::_generatePointerOp;
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
  this->generateBuiltInFunctionCall = &ExpressionGenerator::_generateBuiltInFunctionCall;
  this->generateUserFunctionCall = &ExpressionGenerator::_generateUserFunctionCall;
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
  } else if (astNode->isDerivedFrom<Core::Data::Ast::AssignmentOperator>()) {
    auto assignmentOp = static_cast<Core::Data::Ast::AssignmentOperator*>(astNode);
    return expGenerator->generateAssignment(assignmentOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    return expGenerator->generateOperator(infixOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::OutfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
    return expGenerator->generateOperator(outfixOp, g, tg, tgContext, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::PointerOp>()) {
    auto pointerOp = static_cast<Spp::Ast::PointerOp*>(astNode);
    return expGenerator->generatePointerOp(pointerOp, g, tg, tgContext, result);
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
      if (expGenerator->astHelper->isVarDefinition(obj)) {
        retVal = expGenerator->generateVarReference(obj, g, tg, tgContext, result);
      } else if (
        obj->isDerivedFrom<Ast::Module>() || obj->isDerivedFrom<Ast::Type>() || obj->isDerivedFrom<Ast::Template>()
      ) {
        result.astNode = obj;
        retVal = true;
      } else {
        expGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    },
    searchOwners ? 0 : Core::Data::Seeker::Flags::SKIP_OWNERS
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

  if (astNode->getType() != STR(".")) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Generate the object reference.
  auto first = astNode->getFirst().get();
  if (first == 0) {
    throw EXCEPTION(GenericException, STR("First AST element missing from link operator."));
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
  } else if (firstResult.astNode != 0 && firstResult.astNode->isDerivedFrom<Ast::Module>()) {
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
  SharedList<TiObject, TiObject> paramTgValues;
  PlainList<TiObject, TiObject> paramAstTypes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParamList(param, g, tg, tgContext, &paramAstTypes, &paramTgValues)) return false;

  if (operand->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    // Call a function of the current context.

    // Look for a matching callee.
    TiObject *callee;
    Ast::Type *calleeType;
    if (!expGenerator->astHelper->lookupCallee(
      operand, astNode->getOwner(), true, &paramAstTypes, tg->getExecutionContext(), callee, calleeType
    )) return false;
    if (callee->isDerivedFrom<Ast::Function>()) {
      ////
      //// Call a function.
      ////
      // Prepare the arguments to send.
      expGenerator->prepareFunctionParams(
        static_cast<Ast::Function*>(callee), g, tg, tgContext, &paramAstTypes, &paramTgValues
      );
      // Generate the function call.
      return expGenerator->generateFunctionCall(
        static_cast<Ast::Function*>(callee), &paramAstTypes, &paramTgValues, g, tg, tgContext, result
      );
    } else if (calleeType != 0 && calleeType->isDerivedFrom<Ast::ArrayType>()) {
      ////
      //// Reference array element.
      ////
      // Get a reference to the array.
      GenResult arrayResult;
      if (!expGenerator->generateVarReference(callee, g, tg, tgContext, arrayResult)) return false;
      // Reference array element.
      return expGenerator->generateArrayReference(
        arrayResult.targetData.get(), arrayResult.astType,
        paramTgValues.getElement(0), static_cast<Ast::Type*>(paramAstTypes.getElement(0)),
        g, tg, tgContext, result
      );
    } else {
      throw EXCEPTION(GenericException, STR("Invalid callee."));
    }
  } else if (operand->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    ////
    //// Call a member of a specific object/scope.
    ////
    // Generate the object reference.
    auto linkOperator = static_cast<Core::Data::Ast::LinkOperator*>(operand);
    auto first = linkOperator->getFirst().get();
    if (first == 0) {
      throw EXCEPTION(GenericException, STR("First AST element missing from link operator."));
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
      if (expGenerator->astHelper->isTypeOrRefTypeOf<Ast::ArrayType>(prevResult.astType)) {
        //// Reference element of an array result of the expression.
        return expGenerator->generateArrayReference(
          prevResult.targetData.get(), prevResult.astType,
          paramTgValues.getElement(0), static_cast<Ast::Type*>(paramAstTypes.getElement(0)),
          g, tg, tgContext, result
        );
      } else {
        // TODO: Call a function pointer from a previous expression.
        throw EXCEPTION(GenericException, STR("Not implemented yet."));
      }
    } else if (firstResult.astNode != 0 && firstResult.astNode->isDerivedFrom<Ast::Module>()) {
      //// Calling a global in another module.
      // Look for a matching callee.
      TiObject *callee;
      Ast::Type *calleeType;
      if (!expGenerator->astHelper->lookupCallee(
        second, static_cast<Ast::Module*>(firstResult.astNode), false, &paramAstTypes, tg->getExecutionContext(),
        callee, calleeType
      )) return false;
      if (callee->isDerivedFrom<Ast::Function>()) {
        ////
        //// Call a function.
        ////
        // Prepare the arguments to send.
        expGenerator->prepareFunctionParams(
          static_cast<Ast::Function*>(callee), g, tg, tgContext, &paramAstTypes, &paramTgValues
        );
        // Generate the function call.
        return expGenerator->generateFunctionCall(
          static_cast<Ast::Function*>(callee), &paramAstTypes, &paramTgValues, g, tg, tgContext, result
        );
      } else if (calleeType != 0 && calleeType->isDerivedFrom<Ast::ArrayType>()) {
        ////
        //// Reference array element.
        ////
        // Get a reference to the array.
        GenResult arrayResult;
        if (!expGenerator->generateVarReference(callee, g, tg, tgContext, arrayResult)) return false;
        // Reference array element.
        return expGenerator->generateArrayReference(
          arrayResult.targetData.get(), arrayResult.astType,
          paramTgValues.getElement(0), static_cast<Ast::Type*>(paramAstTypes.getElement(0)),
          g, tg, tgContext, result
        );
      } else {
        throw EXCEPTION(GenericException, STR("Invalid callee."));
      }
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
    if (expGenerator->astHelper->isTypeOrRefTypeOf<Ast::ArrayType>(prevResult.astType)) {
      ////
      //// Reference element of an array result of the expression.
      ////
      return expGenerator->generateArrayReference(
        prevResult.targetData.get(), prevResult.astType,
        paramTgValues.getElement(0), static_cast<Ast::Type*>(paramAstTypes.getElement(0)),
        g, tg, tgContext, result
      );
    } else {
      // TODO: Call a function pointer from a previous expression.
      throw EXCEPTION(GenericException, STR("Not implemented yet."));
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

  // Determine operator function name.
  Char const *funcName;
  if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    if (infixOp->getType() == STR("+")) funcName = STR("__add");
    else if (infixOp->getType() == STR("-")) funcName = STR("__sub");
    else if (infixOp->getType() == STR("*")) funcName = STR("__mul");
    else if (infixOp->getType() == STR("/")) funcName = STR("__div");
    else if (infixOp->getType() == STR("%")) funcName = STR("__rem");
    else if (infixOp->getType() == STR(">>")) funcName = STR("__shr");
    else if (infixOp->getType() == STR("<<")) funcName = STR("__shl");
    else if (infixOp->getType() == STR("&")) funcName = STR("__and");
    else if (infixOp->getType() == STR("|")) funcName = STR("__or");
    else if (infixOp->getType() == STR("$")) funcName = STR("__xor");
    else if (infixOp->getType() == STR("==")) funcName = STR("__equal");
    else if (infixOp->getType() == STR("!=")) funcName = STR("__notEqual");
    else if (infixOp->getType() == STR(">")) funcName = STR("__greaterThan");
    else if (infixOp->getType() == STR(">=")) funcName = STR("__greaterThanOrEqual");
    else if (infixOp->getType() == STR("<")) funcName = STR("__lessThan");
    else if (infixOp->getType() == STR("<=")) funcName = STR("__lessThanOrEqual");
    else if (infixOp->getType() == STR("+=")) funcName = STR("__addAssign");
    else if (infixOp->getType() == STR("-=")) funcName = STR("__subAssign");
    else if (infixOp->getType() == STR("*=")) funcName = STR("__mulAssign");
    else if (infixOp->getType() == STR("/=")) funcName = STR("__divAssign");
    else if (infixOp->getType() == STR("%=")) funcName = STR("__remAssign");
    else if (infixOp->getType() == STR(">>=")) funcName = STR("__shrAssign");
    else if (infixOp->getType() == STR("<<=")) funcName = STR("__shlAssign");
    else if (infixOp->getType() == STR("&=")) funcName = STR("__andAssign");
    else if (infixOp->getType() == STR("|=")) funcName = STR("__orAssign");
    else if (infixOp->getType() == STR("$=")) funcName = STR("__xorAssign");
    else if (infixOp->getType() == STR("||")) {
      return expGenerator->generateLogicalOperator(infixOp, g, tg, tgContext, result);
    } else if (infixOp->getType() == STR("&&")) {
      return expGenerator->generateLogicalOperator(infixOp, g, tg, tgContext, result);
    } else {
      throw EXCEPTION(GenericException, STR("Unexpected infix operator."));
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::PrefixOperator*>(astNode);
    if (outfixOp->getType() == STR("+")) funcName = STR("__pos");
    else if (outfixOp->getType() == STR("-")) funcName = STR("__neg");
    else if (outfixOp->getType() == STR("!")) funcName = STR("__not");
    else if (outfixOp->getType() == STR("++")) funcName = STR("__earlyInc");
    else if (outfixOp->getType() == STR("--")) funcName = STR("__earlyDec");
    else {
      throw EXCEPTION(GenericException, STR("Unexpected prefix operator."));
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::PostfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::PostfixOperator*>(astNode);
    if (outfixOp->getType() == STR("++")) funcName = STR("__lateInc");
    else if (outfixOp->getType() == STR("--")) funcName = STR("__lateDec");
    else {
      throw EXCEPTION(GenericException, STR("Unexpected postfix operator."));
    }
  } else {
    throw EXCEPTION(GenericException, STR("Unexpected operator type."));
  }

  // Generate parameters list.
  SharedList<TiObject, TiObject> paramTgValues;
  PlainList<TiObject, TiObject> paramAstTypes;
  if (!expGenerator->generateParamList(
    ti_cast<Containing<TiObject>>(astNode), g, tg, tgContext, &paramAstTypes, &paramTgValues
  )) return false;

  // Look for a matching function to call.
  TiObject *callee;
  Ast::Type *calleeType;
  if (!expGenerator->astHelper->lookupCalleeByName(
    funcName, Core::Data::Ast::findSourceLocation(astNode), astNode->getOwner(), true,
    &paramAstTypes, tg->getExecutionContext(), callee, calleeType
  )) return false;
  auto function = ti_cast<Ast::Function>(callee);
  if (function == 0) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::NoCalleeMatchNotice>(
      Core::Data::Ast::findSourceLocation(astNode)
    ));
    return false;
  }

  // Prepare the arguments to send.
  expGenerator->prepareFunctionParams(function, g, tg, tgContext, &paramAstTypes, &paramTgValues);

  // Generate the functionc all.
  return expGenerator->generateFunctionCall(function, &paramAstTypes, &paramTgValues, g, tg, tgContext, result);
}


Bool ExpressionGenerator::_generateLogicalOperator(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the operator.
  TioSharedPtr secondContext;
  if (!tg->prepareLogicalOp(tgContext, secondContext)) return false;

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
  if (astNode->getType() == STR("||")) {
    if (!tg->finishLogicalOr(
      tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
    )) return false;
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == STR("&&")) {
    if (!tg->finishLogicalAnd(
      tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
    )) return false;
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else {
    throw EXCEPTION(GenericException, STR("Invalid logical operator."));
  }
}


Bool ExpressionGenerator::_generateAssignment(
  TiObject *self, Core::Data::Ast::AssignmentOperator *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (astNode->getType() != STR("=")) {
    return expGenerator->generateOperator(astNode, g, tg, tgContext, result);
  }

  // Generate assignment target.
  auto var = astNode->getFirst().get();
  if (var == 0) {
    throw EXCEPTION(GenericException, STR("Assignment target is missing."));
  }
  GenResult varResult;
  if (!expGenerator->generate(var, g, tg, tgContext, varResult)) return false;
  if (varResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(var))
    );
    return false;
  }
  auto varRefAstType = ti_cast<Ast::ReferenceType>(varResult.astType);
  if (varRefAstType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::UnsupportedOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  auto varAstType = varRefAstType->getContentType(expGenerator->astHelper);

  // Generate assignment value.
  auto val = astNode->getSecond().get();
  if (val == 0) {
    throw EXCEPTION(GenericException, STR("Assignment value is missing."));
  }
  GenResult valResult;
  if (!expGenerator->generate(val, g, tg, tgContext, valResult)) return false;
  if (valResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(var))
    );
    return false;
  }

  // Is value implicitly castable?
  if (!valResult.astType->isImplicitlyCastableTo(varAstType, expGenerator->astHelper, tg->getExecutionContext())) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::NotImplicitlyCastableNotice>(Core::Data::Ast::findSourceLocation(val))
    );
    return false;
  }

  // Cast the generated value.
  TioSharedPtr castedTgVal;
  if (!g->generateCast(tg, tgContext, valResult.astType, varAstType, valResult.targetData.get(), castedTgVal)) {
    return false;
  }

  // Create the store instruction.
  TioSharedPtr assignOp;
  if (!tg->generateAssign(
    tgContext, getCodeGenData<TiObject>(varAstType), castedTgVal.get(), varResult.targetData.get(), assignOp
  )) return false;
  result = varResult;
  return true;
}


Bool ExpressionGenerator::_generatePointerOp(
  TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, STR("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, tg, tgContext, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
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


Bool ExpressionGenerator::_generateContentOp(
  TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, STR("ContentOp operand is missing."));
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
    throw EXCEPTION(GenericException, STR("ContentOp operand is missing."));
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
  if (!g->generateCast(
    tg, tgContext, derefResult.astType, targetAstType, derefResult.targetData.get(), result.targetData
  )) return false;
  result.astType = targetAstType;
  return true;
}


Bool ExpressionGenerator::_generateSizeOp(
  TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the noop target generator.
  expGenerator->noOpTargetGenerator->setExecutionContext(getSharedPtr(tg->getExecutionContext()));

  // Generate the Int64 type needed for the result.
  auto astInt64Type = expGenerator->astHelper->getInt64Type();
  TiObject *tgInt64Type;
  if (!g->getGeneratedType(astInt64Type, tg, tgInt64Type, 0)) {
    throw EXCEPTION(GenericException, STR("Failed to generate Int64 type."));
  }

  // Get the operand type.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, STR("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(
    operand, g, ti_cast<TargetGeneration>(expGenerator->noOpTargetGenerator), 0, operandResult
  )) return false;
  Ast::Type *astType = operandResult.astType;
  if (astType == 0) astType = ti_cast<Ast::Type>(operandResult.astNode);
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

  // Generate a constant with that size.
  if (!tg->generateIntLiteral(tgContext, 64, size, result.targetData)) return false;
  result.astType = astInt64Type;
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

  auto charPtrAstType = expGenerator->astHelper->getCharPtrType();
  TiObject *charPtrTgType;
  if (!g->getGeneratedType(charPtrAstType, tg, charPtrTgType, 0)) return false;

  auto strAstType = expGenerator->astHelper->getCharArrayType(value->size() + 1);
  TiObject *strTgType;
  if (!g->getGeneratedType(strAstType, tg, strTgType, 0)) return false;

  if (!tg->generateStringLiteral(tgContext, value->c_str(), charTgType, strTgType, result.targetData)) return false;
  result.astType = charPtrAstType;
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

  if (!tg->generateIntLiteral(tgContext, bitCount, value, result.targetData)) return false;
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
  if (compareStr(src, STR("0b"), 2) == 0 || compareStr(src, STR("0B"), 2) == 0) {
    base = 2;
    src += 2;
  } else if (compareStr(src, STR("0o"), 2) == 0 || compareStr(src, STR("0O"), 2) == 0) {
    base = 8;
    src += 2;
  } else if (compareStr(src, STR("0h"), 2) == 0 || compareStr(src, STR("0H"), 2) == 0) {
    base = 16;
    src += 2;
  }
  Long value = 0;
  while (
    (*src >= CHR('0') && *src <= CHR('9')) ||
    (*src >= CHR('a') && *src <= CHR('f')) ||
    (*src >= CHR('A') && *src <= CHR('F'))
  ) {
    Int digit = 0;
    if (*src >= CHR('0') && *src <= CHR('9')) digit = *src - CHR('0');
    else if (*src >= CHR('a') && *src <= CHR('f')) digit = *src - CHR('a') + 10;
    else if (*src >= CHR('A') && *src <= CHR('F')) digit = *src - CHR('A') + 10;
    ASSERT(digit < base);
    value *= base;
    value += digit;
    ++src;
  }

  // Is it a signed number?
  // Bool signedNum = true;
  if (*src == CHR('u') || *src == CHR('U')) {
    // signedNum = false;
    ++src;
  }

  // Determine integer size.
  Int size = 32;
  if (*src == CHR('i') || *src == CHR('I')) {
    ++src;
    if (getStrLen(src) > 0) size = std::stoi(src);
  }

  // Get the requested int type.
  auto intAstType = expGenerator->astHelper->getIntType(size);
  auto sourceLocation = astNode->findSourceLocation().get();
  expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation);
  TiObject *intTgType;
  Bool retVal = g->getGeneratedType(intAstType, tg, intTgType, 0);
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (!tg->generateIntLiteral(tgContext, size, value, result.targetData)) return false;
  result.astType = intAstType;
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
  if (*src == CHR('f') || *src == CHR('F')) {
    ++src;
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
  if (!tg->generateFloatLiteral(tgContext, size, value, result.targetData)) return false;
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
      throw EXCEPTION(GenericException, STR("Unexpected error while looking for variable definition."));
    }
    if (!g->generateVarDef(varDef, tg)) return false;

    tgVar = getCodeGenData<TiObject>(astNode);
  }

  auto astType = Ast::getAstType(astNode);
  TiObject *tgType;
  if (!g->getGeneratedType(astType, tg, tgType, 0)) return false;

  if (!tg->generateVarReference(tgContext, tgType, tgVar, result.targetData)) return false;
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

  // Find the member variable.
  auto body = astStructType->getBody().get();
  if (body == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  TiObject *astMemberVar;
  if (!expGenerator->astHelper->getSeeker()->tryGet(astNode, body, astMemberVar)) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Get the member generated value and type.
  auto tgMemberVar = getCodeGenData<TiObject>(astMemberVar);
  auto astMemberType = Ast::getAstType(astMemberVar);
  TiObject *tgMemberType;
  if (!g->getGeneratedType(astMemberType, tg, tgMemberType, 0)) return false;

  // Generate member access.
  if (!tg->generateMemberVarReference(tgContext, tgStructType, tgMemberType, tgMemberVar, tgValue, result.targetData)) {
    return false;
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
  if (!g->generateCast(
    tg, tgContext, astIndexType, expGenerator->astHelper->getInt64Type(), tgIndexVal, tgCastedIndex
  )) {
    // This should not happen since non-castable calls should be filtered out earlier.
    throw EXCEPTION(GenericException, STR("Invalid cast was unexpectedly found."));
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
    throw EXCEPTION(GenericException, STR("Unexpected type for array reference."));
  }

  // Find element type.
  auto astElementType = astArrayType->getContentType(expGenerator->astHelper);
  TiObject *tgElementType;
  if (!g->getGeneratedType(astElementType, tg, tgElementType, 0)) return false;

  // Generate member access.
  if (!tg->generateArrayElementReference(
    tgContext, tgArrayType, tgElementType, tgCastedIndex.get(), tgValue, result.targetData)) {
    return false;
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

  // is function built-in?
  if (callee->getName().getStr().size() > 0 && callee->getName().getStr().at(0) == CHR('#')) {
    return expGenerator->generateBuiltInFunctionCall(callee, paramAstTypes, paramTgValues, g, tg, tgContext, result);
  } else {
    if (callee->getInlined()) {
      // TODO: Generate inlined function body.
      return false;
    } else {
      return expGenerator->generateUserFunctionCall(callee, paramAstTypes, paramTgValues, g, tg, tgContext, result);
    }
  }
}


Bool ExpressionGenerator::_generateBuiltInFunctionCall(
  TiObject *self, Spp::Ast::Function *callee,
  Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
  Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (paramAstTypes == 0 || paramAstTypes->getElementCount() == 0) {
    throw EXCEPTION(GenericException, STR("Param AST types are missing."));
  }

  auto astRetType = callee->traceRetType(expGenerator->astHelper);

  Ast::Type *astType = expGenerator->astHelper->getValueTypeFor(paramAstTypes->getElement(0));
  TiObject *tgType;
  if (!g->getGeneratedType(astType, tg, tgType, 0)) return false;

  // Binary Math Operations
  if (callee->getName() == STR("#add")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #add built-in function."));
    }
    if (!tg->generateAdd(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#sub")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #sub built-in function."));
    }
    if (!tg->generateSub(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#mul")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #mul built-in function."));
    }
    if (!tg->generateMul(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#div")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #div built-in function."));
    }
    if (!tg->generateDiv(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#rem")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #rem built-in function."));
    }
    if (!tg->generateRem(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#shr")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #shr built-in function."));
    }
    if (!tg->generateShr(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#shl")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #shl built-in function."));
    }
    if (!tg->generateShl(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#and")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #and built-in function."));
    }
    if (!tg->generateAnd(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#or")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #or built-in function."));
    }
    if (!tg->generateOr(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#xor")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #xor built-in function."));
    }
    if (!tg->generateXor(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#addAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #addAssign built-in function."));
    }
    if (!tg->generateAddAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#subAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #subAssign built-in function."));
    }
    if (!tg->generateSubAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#mulAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #mulAssign built-in function."));
    }
    if (!tg->generateMulAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#divAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #divAssign built-in function."));
    }
    if (!tg->generateDivAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#remAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #remAssign built-in function."));
    }
    if (!tg->generateRemAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#shrAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #shrAssign built-in function."));
    }
    if (!tg->generateShrAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#shlAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #shlAssign built-in function."));
    }
    if (!tg->generateShlAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#andAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #andAssign built-in function."));
    }
    if (!tg->generateAndAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#orAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #orAssign built-in function."));
    }
    if (!tg->generateOrAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#xorAssign")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #xorAssign built-in function."));
    }
    if (!tg->generateXorAssign(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;

  // Unary Operations
  } else if (callee->getName() == STR("#pos")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #pos built-in function."));
    }
    // Unary + operator is a no-op.
    result.astType = ti_cast<Ast::Type>(paramAstTypes->getElement(0));
    result.targetData = getSharedPtr(paramTgValues->getElement(0));
    return true;
  } else if (callee->getName() == STR("#neg")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #neg built-in function."));
    }
    if (!tg->generateNeg(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#not")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #not built-in function."));
    }
    if (!tg->generateNot(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#earlyInc")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #earlyInc built-in function."));
    }
    if (!tg->generateEarlyInc(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#earlyDec")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #earlyDec built-in function."));
    }
    if (!tg->generateEarlyDec(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#lateInc")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #lateInc built-in function."));
    }
    if (!tg->generateLateInc(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#lateDec")) {
    if (paramTgValues->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #lateDec built-in function."));
    }
    if (!tg->generateLateDec(tgContext, tgType, paramTgValues->getElement(0), result.targetData)) return false;
    result.astType = astRetType;
    return true;

  // Comparison Operations
  } else if (callee->getName() == STR("#equal")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #equal built-in function."));
    }
    if (!tg->generateEqual(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#notEqual")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #notEqual built-in function."));
    }
    if (!tg->generateNotEqual(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#greaterThan")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #greaterThan built-in function."));
    }
    if (!tg->generateGreaterThan(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#greaterThanOrEqual")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #greaterThanOrEqual built-in function.")
      );
    }
    if (!tg->generateGreaterThanOrEqual(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#lessThan")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #lessThan built-in function."));
    }
    if (!tg->generateLessThan(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  } else if (callee->getName() == STR("#lessThanOrEqual")) {
    if (paramTgValues->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #lessThanOrEqual built-in function.")
      );
    }
    if (!tg->generateLessThanOrEqual(
      tgContext, tgType, paramTgValues->getElement(0), paramTgValues->getElement(1), result.targetData
    )) return false;
    result.astType = astRetType;
    return true;
  }

  throw EXCEPTION(GenericException, STR("Unexpected built-in function."));
}


Bool ExpressionGenerator::_generateUserFunctionCall(
    TiObject *self, Spp::Ast::Function *callee,
    Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
    Generation *g, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Build funcion declaration.
  if (!g->generateFunctionDecl(callee, tg)) return false;
  auto tgFunction = getCodeGenData<TiObject>(callee);

  // Create function call.
  if (!tg->generateFunctionCall(tgContext, tgFunction, paramTgValues, result.targetData)) return false;
  result.astType = callee->traceRetType(expGenerator->astHelper);
  return true;
}


//==============================================================================
// Helper Functions

Bool ExpressionGenerator::generateParamList(
  TiObject *astNode, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  ListContaining<TiObject> *resultTypes, SharedList<TiObject, TiObject> *resultValues
) {
  if (astNode == 0) return true;

  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    if (!this->generateParamList(
      ti_cast<Containing<TiObject>>(astNode), g, tg, tgContext, resultTypes, resultValues
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
  }
  return true;
}


Bool ExpressionGenerator::generateParamList(
  Containing<TiObject> *astNodes, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  ListContaining<TiObject> *resultTypes, SharedList<TiObject, TiObject> *resultValues
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
  }
  return true;
}


void ExpressionGenerator::prepareFunctionParams(
  Spp::Ast::Function *callee, Generation *g, TargetGeneration *tg, TiObject *tgContext,
  ListContaining<TiObject> *paramAstTypes, SharedList<TiObject, TiObject> *paramTgVals
) {
  Ast::Function::ArgMatchContext context;
  for (Int i = 0; i < paramTgVals->getElementCount(); ++i) {
    Ast::Type *srcType = static_cast<Ast::Type*>(paramAstTypes->getElement(i));
    auto status = callee->matchNextArg(srcType, context, this->astHelper, tg->getExecutionContext());
    ASSERT(status != Ast::CallMatchStatus::NONE);

    // Cast the value if needed.
    if (context.type != 0) {
      TioSharedPtr tgCastedVal;
      if (!g->generateCast(tg, tgContext, srcType, context.type, paramTgVals->getElement(i), tgCastedVal)) {
        // This should not happen since non-castable calls should be filtered out earlier.
        throw EXCEPTION(GenericException, STR("Invalid cast was unexpectedly found."));
      }
      paramTgVals->set(i, tgCastedVal);
    } else {
      // For var args we need to send values, not references.
      GenResult result;
      if (!this->dereferenceIfNeeded(tg, tgContext, srcType, paramTgVals->getElement(i), result)) {
        throw EXCEPTION(GenericException, STR("Unexpected error."));
      }
      paramTgVals->set(i, result.targetData);
    }
  }
}


Bool ExpressionGenerator::dereferenceIfNeeded(
  TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *astType, TiObject *tgValue, GenResult &result
) {
  auto refType = ti_cast<Spp::Ast::ReferenceType>(astType);
  if (refType != 0) {
    result.astType = refType->getContentType(this->astHelper);
    auto tgContentType = getCodeGenData<TiObject>(result.astType);
    return tg->generateDereference(tgContext, tgContentType, tgValue, result.targetData);
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
  if (!g->generateCast(tg, tgContext, astType, this->astHelper->getBoolType(), tgValue, result)) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} } // namespace
