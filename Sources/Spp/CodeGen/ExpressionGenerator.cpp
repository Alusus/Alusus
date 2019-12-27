/**
 * @file Spp/CodeGen/ExpressionGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
    &this->generateInitOp,
    &this->generateTerminateOp,
    &this->generateStringLiteral,
    &this->generateCharLiteral,
    &this->generateIntegerLiteral,
    &this->generateFloatLiteral,
    &this->generateVarReference,
    &this->generateMemberReference,
    &this->generateMemberVarReference,
    &this->generateArrayReference,
    &this->generateFunctionCall,
    &this->prepareFunctionParams,
    &this->generateCalleeReferenceChain
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
  this->generateInitOp = &ExpressionGenerator::_generateInitOp;
  this->generateTerminateOp = &ExpressionGenerator::_generateTerminateOp;
  this->generateStringLiteral = &ExpressionGenerator::_generateStringLiteral;
  this->generateCharLiteral = &ExpressionGenerator::_generateCharLiteral;
  this->generateIntegerLiteral = &ExpressionGenerator::_generateIntegerLiteral;
  this->generateFloatLiteral = &ExpressionGenerator::_generateFloatLiteral;
  this->generateVarReference = &ExpressionGenerator::_generateVarReference;
  this->generateMemberReference = &ExpressionGenerator::_generateMemberReference;
  this->generateMemberVarReference = &ExpressionGenerator::_generateMemberVarReference;
  this->generateArrayReference = &ExpressionGenerator::_generateArrayReference;
  this->generateFunctionCall = &ExpressionGenerator::_generateFunctionCall;
  this->prepareFunctionParams = &ExpressionGenerator::_prepareFunctionParams;
  this->generateCalleeReferenceChain = &ExpressionGenerator::_generateCalleeReferenceChain;
}


//==============================================================================
// Top Level Generation Functions

Bool ExpressionGenerator::_generate(
  TiObject *self, TiObject *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    auto expList = static_cast<Core::Data::Ast::List*>(astNode);
    return expGenerator->generateList(expList, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    return expGenerator->generateIdentifier(identifier, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
    auto linkOperator = static_cast<Core::Data::Ast::LinkOperator*>(astNode);
    return expGenerator->generateLinkOperator(linkOperator, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    return expGenerator->generateParamPass(paramPass, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    return expGenerator->generateOperator(infixOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::OutfixOperator>()) {
    auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
    return expGenerator->generateOperator(outfixOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::PointerOp>()) {
    auto pointerOp = static_cast<Spp::Ast::PointerOp*>(astNode);
    return expGenerator->generatePointerOp(pointerOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::AstRefOp>()) {
    auto astRefOp = static_cast<Spp::Ast::AstRefOp*>(astNode);
    return expGenerator->generateAstRefOp(astRefOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContentOp>()) {
    auto contentOp = static_cast<Spp::Ast::ContentOp*>(astNode);
    return expGenerator->generateContentOp(contentOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::CastOp>()) {
    auto castOp = static_cast<Spp::Ast::CastOp*>(astNode);
    return expGenerator->generateCastOp(castOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::SizeOp>()) {
    auto sizeOp = static_cast<Spp::Ast::SizeOp*>(astNode);
    return expGenerator->generateSizeOp(sizeOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::InitOp>()) {
    auto initOp = static_cast<Spp::Ast::InitOp*>(astNode);
    return expGenerator->generateInitOp(initOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Spp::Ast::TerminateOp>()) {
    auto terminateOp = static_cast<Spp::Ast::TerminateOp*>(astNode);
    return expGenerator->generateTerminateOp(terminateOp, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    return expGenerator->generateStringLiteral(stringLiteral, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::CharLiteral>()) {
    auto charLiteral = static_cast<Core::Data::Ast::CharLiteral*>(astNode);
    return expGenerator->generateCharLiteral(charLiteral, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    auto integerLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(astNode);
    return expGenerator->generateIntegerLiteral(integerLiteral, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    auto floatLiteral = static_cast<Core::Data::Ast::FloatLiteral*>(astNode);
    return expGenerator->generateFloatLiteral(floatLiteral, g, deps, result);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Data::Ast::Bracket*>(astNode);
    auto operand = bracket->getOperand().get();
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND && operand != 0) {
      return expGenerator->generate(operand, g, deps, result);
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
  TiObject *self, Core::Data::Ast::List *astNode, Generation *g, GenDeps const &deps, GenResult &result
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
    if (!expGenerator->generate(innerNode, g, deps, innerResult)) return false;
  }
  result = innerResult;
  return true;
}


Bool ExpressionGenerator::_generateIdentifier(
  TiObject *self, Core::Data::Ast::Identifier *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  return expGenerator->generateScopeMemberReference(astNode->getOwner(), astNode, true, g, deps, result);
}


Bool ExpressionGenerator::_generateScopeMemberReference(
  TiObject *self, TiObject *scope, Core::Data::Ast::Identifier *astNode, Bool searchOwners,
  Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  Bool retVal = false;
  Bool symbolFound = false;
  expGenerator->astHelper->getSeeker()->foreach(astNode, scope,
    [=, &retVal, &symbolFound, &result]
    (TiObject *obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;

      // Unbox if we have a box.
      auto box = ti_cast<TioWeakBox>(obj);
      if (box != 0) obj = box->get().lock().get();

      // Check if the found obj is a variable definition.
      if (expGenerator->astHelper->isAstReference(obj)) {
        // Make sure the var is not an object member.
        if (expGenerator->getAstHelper()->getDefinitionDomain(obj) == Ast::DefinitionDomain::OBJECT) {
          auto ownerType = static_cast<Core::Data::Node*>(obj)->findOwner<Spp::Ast::UserType>();
          if (ownerType != 0 && ownerType == deps.astSelfType) {
            // Generate a reference to a member variable.
            auto astSelfPtrType = expGenerator->astHelper->getReferenceTypeFor(deps.astSelfType);
            retVal = expGenerator->generateMemberReference(deps.tgSelf, astSelfPtrType, astNode, g, deps, result);
          } else {
            expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidObjectMemberAccessNotice>(
              Core::Data::Ast::findSourceLocation(astNode)
            ));
            retVal = false;
          }
        } else {
          retVal = expGenerator->generateVarReference(astNode, obj, g, deps, result);
        }
      } else if (
        obj->isDerivedFrom<Core::Data::Ast::Scope>() || obj->isDerivedFrom<Ast::Type>() ||
        obj->isDerivedFrom<Ast::Template>() || obj->isDerivedFrom<Ast::Function>()
      ) {
        result.astNode = obj;
        retVal = true;
      } else if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        retVal = expGenerator->generateStringLiteral(
          static_cast<Core::Data::Ast::StringLiteral*>(obj), g, deps, result
        );
      } else if (obj->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
        retVal = expGenerator->generateIntegerLiteral(
          static_cast<Core::Data::Ast::IntegerLiteral*>(obj), g, deps, result
        );
      } else if (obj->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
        retVal = expGenerator->generateFloatLiteral(
          static_cast<Core::Data::Ast::FloatLiteral*>(obj), g, deps, result
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
  TiObject *self, Core::Data::Ast::LinkOperator *astNode, Generation *g, GenDeps const &deps, GenResult &result
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
  if (!expGenerator->generate(first, g, deps, firstResult)) return false;

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
      firstResult.targetData.get(), firstResult.astType, second, g, deps, result
    );
  } else if (firstResult.astNode != 0) {
    // Generate a reference to a global in another module.
    return expGenerator->generateScopeMemberReference(firstResult.astNode, second, false, g, deps, result);
  } else {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }
}


Bool ExpressionGenerator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  if (astNode->getType() == Core::Data::Ast::BracketType::ROUND) {
    return expGenerator->generateRoundParamPass(astNode, g, deps, result);
  } else {
    return expGenerator->generateSquareParamPass(astNode, g, deps, result);
  }
}


Bool ExpressionGenerator::_generateRoundParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();

  // Prepare parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParamList(param, g, deps, &paramAstNodes, &paramAstTypes, &paramTgValues)) {
    return false;
  }

  if (operand->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    ////
    //// Call a member of the current scope.
    ////
    Ast::CalleeLookupResult calleeResult;
    if (!expGenerator->astHelper->lookupCalleeInScope(
      operand, astNode->getOwner(), true, 0, &paramAstTypes, deps.tg->getExecutionContext(), calleeResult
    )) {
      expGenerator->noticeStore->add(calleeResult.notice);
      return false;
    }
    GenResult prevResult;
    GenResult callee;
    GenResult thisArg;
    if (!expGenerator->generateCalleeReferenceChain(calleeResult, astNode, prevResult, g, deps, callee, thisArg)) {
      return false;
    }
    return expGenerator->generateRoundParamPassOnCallee(
      astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, deps, result
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
    if (!expGenerator->generate(first, g, deps, firstResult)) return false;
    // Generate the member identifier.
    auto second = linkOperator->getSecond().ti_cast_get<Core::Data::Ast::Identifier>();
    if (second == 0) {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidOperationNotice>(linkOperator->findSourceLocation())
      );
      return false;
    }

    if (firstResult.astType != 0) {
      auto thisType = expGenerator->astHelper->tryGetDeepReferenceContentType(firstResult.astType);
      auto thisRefType = expGenerator->astHelper->getReferenceTypeFor(thisType);
      // Lookup the callee.
      Ast::CalleeLookupResult calleeResult;
      if (!expGenerator->astHelper->lookupCalleeInScope(
        second, thisType, true, thisRefType, &paramAstTypes, deps.tg->getExecutionContext(),
        calleeResult
      )) {
        expGenerator->noticeStore->add(calleeResult.notice);
        return false;
      }
      if (deps.tgContext != 0) {
        if (firstResult.astType->isEqual(thisType, expGenerator->astHelper, deps.tg->getExecutionContext())) {
          // The previous expression returned a value rather than a ref, and member functions need a reference,
          // so we'll create a temp var.
          // This code path should not be reached with custom-initialization variables, so we don't need to
          // register the destructor here.
          if (!g->generateTempVar(astNode, thisType, deps, false)) return false;
          PlainList<TiObject> thisParamAstNodes({ astNode });
          PlainList<TiObject> thisParamAstTypes({ firstResult.astType });
          SharedList<TiObject> thisParamTgValues({ firstResult.targetData });
          if (!deps.tg->generateVarReference(
            deps.tgContext, getCodeGenData<TiObject>(thisType), getCodeGenData<TiObject>(astNode),
            firstResult.targetData
          )) {
            return false;
          }
          firstResult.astType = thisRefType;
          if (!g->generateVarInitialization(
            thisType, firstResult.targetData.get(), astNode,
            &thisParamAstNodes, &thisParamAstTypes, &thisParamTgValues, deps
          )) return false;
        }
      }
      // Trace through the chain.
      GenResult callee;
      GenResult thisArg;
      if (!expGenerator->generateCalleeReferenceChain(
        calleeResult, astNode, firstResult, g, deps, callee, thisArg
      )) return false;
      // Make the call.
      return expGenerator->generateRoundParamPassOnCallee(
        astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, deps, result
      );
    } else if (firstResult.astNode != 0) {
      //// Calling a global in another module.
      // Look for a matching callee.
      Ast::CalleeLookupResult calleeResult;
      if (!expGenerator->astHelper->lookupCalleeInScope(
        second, static_cast<Ast::Module*>(firstResult.astNode), false, 0, &paramAstTypes,
        deps.tg->getExecutionContext(), calleeResult
      )) {
        expGenerator->noticeStore->add(calleeResult.notice);
        return false;
      }
      // Prepare the callee and make the call.
      GenResult prevResult;
      GenResult callee;
      GenResult thisArg;
      if (!expGenerator->generateCalleeReferenceChain(calleeResult, astNode, prevResult, g, deps, callee, thisArg)) {
        return false;
      }
      return expGenerator->generateRoundParamPassOnCallee(
        astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, deps, result
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
    if (!expGenerator->generate(operand, g, deps, prevResult)) return false;

    // Lookup a callee on the result.
    auto prevAstType = expGenerator->astHelper->tryGetDeepReferenceContentType(prevResult.astType);
    Ast::CalleeLookupResult calleeResult;
    if (!expGenerator->astHelper->lookupCalleeOnObject(
      prevAstType, 0, &paramAstTypes, deps.tg->getExecutionContext(), 0, calleeResult
    )) {
      calleeResult.notice->setSourceLocation(astNode->findSourceLocation());
      expGenerator->noticeStore->add(calleeResult.notice);
      return false;
    }

    // Make the call.
    GenResult callee;
    GenResult thisArg;
    if (!expGenerator->generateCalleeReferenceChain(calleeResult, astNode, prevResult, g, deps, callee, thisArg)) {
      return false;
    }
    return expGenerator->generateRoundParamPassOnCallee(
      astNode, callee, thisArg, &paramTgValues, &paramAstTypes, &paramAstNodes, g, deps, result
    );
  }
}


Bool ExpressionGenerator::_generateRoundParamPassOnCallee(
  TiObject *self, Core::Data::Node *astNode, GenResult const &callee, GenResult const &thisArg,
  SharedList<TiObject> *paramTgValues, PlainList<TiObject> *paramAstTypes, PlainList<TiObject> *paramAstNodes,
  Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee.astNode != 0 && callee.astNode->isDerivedFrom<Ast::Function>()) {
    //// Call a function.
    ////
    // Prepare the arguments to send.
    if (thisArg.targetData != 0) {
      paramAstNodes->insert(0, astNode);
      paramAstTypes->insert(0, thisArg.astType);
      paramTgValues->insert(0, thisArg.targetData);
    }
    if (!expGenerator->prepareFunctionParams(
      static_cast<Ast::Function*>(callee.astNode)->getType().get(), g, deps, paramAstNodes, paramAstTypes, paramTgValues
    )) return false;
    // Generate the function call.
    return expGenerator->generateFunctionCall(
      astNode, static_cast<Ast::Function*>(callee.astNode), paramAstTypes, paramTgValues, g, deps, result
    );
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
      if (thisArg.targetData != 0) {
        paramAstNodes->insert(0, astNode);
        paramAstTypes->insert(0, thisArg.astType);
        paramTgValues->insert(0, thisArg.targetData);
      }
      // Make sure the args match.
      if (astFuncType->matchCall(
        paramAstTypes, expGenerator->astHelper, deps.tg->getExecutionContext()
      ) == Ast::TypeMatchStatus::NONE) {
        expGenerator->noticeStore->add(
          std::make_shared<Spp::Notices::ArgsMismatchNotice>(Core::Data::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      // Cast funciton args if needed.
      if (!expGenerator->prepareFunctionParams(
        astFuncType, g, deps, paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      // Get the target data of the pointer and its type.
      GenResult derefCallee;
      if (!expGenerator->dereferenceIfNeeded(
        callee.astType, callee.targetData.get(), true, deps, derefCallee
      )) return false;
      TiObject *tgFuncPtrType = getCodeGenData<TiObject>(derefCallee.astType);
      // Generate the call.
      if (deps.tgContext != 0) {
        if (!deps.tg->generateFunctionPtrCall(
          deps.tgContext, derefCallee.targetData.get(), tgFuncPtrType, paramTgValues, result.targetData
        )) return false;
      }
      result.astType = astFuncType->traceRetType(expGenerator->astHelper);
      return true;
    } else if (contentType != 0 && contentType->isDerivedFrom<Ast::ArrayType>()) {
      //// Reference array element.
      ////
      // Get a reference to the array.
      GenResult derefCallee;
      if (!expGenerator->dereferenceIfNeeded(
        callee.astType, callee.targetData.get(), false, deps, derefCallee
      )) return false;
      // Reference array element.
      return expGenerator->generateArrayReference(
        astNode, derefCallee.targetData.get(), derefCallee.astType,
        paramTgValues->getElement(0), static_cast<Ast::Type*>(paramAstTypes->getElement(0)), g, deps, result
      );
    } else {
      expGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  }
}


Bool ExpressionGenerator::_generateSquareParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();

  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
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
  TiObject *self, Core::Data::Node *astNode, Generation *g, GenDeps const &deps, GenResult &result
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
      return expGenerator->generateLogicalOp(infixOp, g, deps, result);
    } else if (infixOp->getType() == S("&&") || infixOp->getType() == S("and")) {
      return expGenerator->generateLogicalOp(infixOp, g, deps, result);
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
    ti_cast<Containing<TiObject>>(astNode), g, deps, &paramAstNodes, &paramAstTypes, &paramTgValues
  )) return false;

  // Look for a matching function to call.
  auto param0AstContentType = expGenerator->astHelper->tryGetDeepReferenceContentType(
    ti_cast<Ast::Type>(paramAstTypes.getElement(0))
  );
  if (param0AstContentType == 0) {
    throw EXCEPTION(GenericException, S("Unexpected AST type for first operator parameter."));
  }
  Ast::CalleeLookupResult calleeResult;
  Ast::Function *function = 0;
  if (expGenerator->astHelper->lookupCalleeInScopeByName(
    funcName, Core::Data::Ast::findSourceLocation(astNode), param0AstContentType,
    true, 0, &paramAstTypes, deps.tg->getExecutionContext(), calleeResult
  )) function = ti_cast<Ast::Function>(calleeResult.stack.get(calleeResult.stack.getCount() - 1));

  if (function != 0) {
    // Prepare the arguments to send.
    if (!expGenerator->prepareFunctionParams(
      function->getType().get(), g, deps, &paramAstNodes, &paramAstTypes, &paramTgValues
    )) return false;

    // Generate the function call.
    return expGenerator->generateFunctionCall(
      astNode, function, &paramAstTypes, &paramTgValues, g, deps, result
    );
  } else {
    // We have no function for this op, so fall back to default implementation.
    if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::ARITHMETIC) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::BINARY) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::COMPARISON) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateComparisonOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::ARITHMETIC_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateArithmeticAssignOp(
        infixOp, &paramTgValues, &paramAstTypes, g, deps, result
      );
    } else if (opType == OpType::BINARY_ASSIGN) {
      auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
      return expGenerator->generateBinaryAssignOp(infixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::INT_UNARY_VAL) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateIntUnaryValOp(outfixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else if (opType == OpType::UNARY_VAR) {
      auto outfixOp = static_cast<Core::Data::Ast::OutfixOperator*>(astNode);
      return expGenerator->generateUnaryVarOp(outfixOp, &paramTgValues, &paramAstTypes, g, deps, result);
    } else {
      if (calleeResult.notice != 0) {
        expGenerator->noticeStore->add(calleeResult.notice);
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
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Prepare the operator.
  TioSharedPtr secondContext;
  if (deps.tgContext != 0) {
    if (!deps.tg->prepareLogicalOp(deps.tgContext, secondContext)) return false;
  }

  // Generate 1st operand.
  GenResult firstResult;
  if (!expGenerator->generate(astNode->getFirst().get(), g, deps, firstResult)) return false;

  // Cast 1st operand to boolean.
  TioSharedPtr firstCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, deps, astNode->getFirst().get(), firstResult.astType, firstResult.targetData.get(), firstCastedResult
  )) return false;

  // Generate 2nd operand.
  GenResult secondResult;
  if (!expGenerator->generate(astNode->getSecond().get(), g, GenDeps(deps, secondContext.get()), secondResult)) {
    return false;
  }

  // Cast 2nd operand to boolean.
  TioSharedPtr secondCastedResult;
  if (!expGenerator->castLogicalOperand(
    g, GenDeps(deps, secondContext.get()), astNode->getSecond().get(), secondResult.astType,
    secondResult.targetData.get(), secondCastedResult
  )) return false;

  // Finalize the operator.
  if (astNode->getType() == S("||") || astNode->getType() == S("or")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->finishLogicalOr(
        deps.tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("&&") || astNode->getType() == S("and")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->finishLogicalAnd(
        deps.tgContext, secondContext.get(), firstCastedResult.get(), secondCastedResult.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, deps, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, deps, param2
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      deps, param2.astType, astTargetType, astNode, param2.targetData.get(), false, param2.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, deps.tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("+")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateAdd(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("-")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateSub(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("*")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateMul(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("/")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateDiv(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("%")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateRem(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, deps, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, deps, param2
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      deps, param2.astType, astTargetType, astNode, param2.targetData.get(), false, param2.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, deps.tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateShr(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("<<")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateShl(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("&")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateAnd(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("|")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateOr(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astTargetType;
    return true;
  } else if (astNode->getType() == S("$")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateXor(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, deps, param1
  )) return false;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, deps, param2
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param1.astType, astTargetType, astNode, param1.targetData.get(), false, param1.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
    if (!g->generateCast(
      deps, param2.astType, astTargetType, astNode, param2.targetData.get(), false, param2.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, deps.tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("==")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateEqual(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("!=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateNotEqual(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateGreaterThan(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S(">=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateGreaterThanOrEqual(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateLessThan(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = expGenerator->astHelper->getBoolType();
    return true;
  } else if (astNode->getType() == S("<=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateLessThanOrEqual(
        deps.tgContext, tgTargetType, param1.targetData.get(), param2.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), false, deps, param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  // Cast the value to the destination type.
  Bool retVal;
  if (deps.tgContext != 0) {
    retVal = g->generateCast(
      deps, param.astType, astContentType, astNode, param.targetData.get(), true, param.targetData
    );
  } else {
    retVal = expGenerator->astHelper->isImplicitlyCastableTo(
      param.astType, astContentType, deps.tg->getExecutionContext()
    );
  }
  if (!retVal) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, deps.tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateAssign(
        deps.tgContext, tgContentType, param.targetData.get(), target.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, deps, param
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param.astType, astContentType, astNode, param.targetData.get(), false, param.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, deps.tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("+=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateAddAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("-=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateSubAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("*=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateMulAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("/=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateDivAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("%=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateRemAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }
  GenResult param;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(paramAstTypes->get(1)), paramTgValues->getElement(1), true, deps, param
  )) return false;
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (!astContentType->isDerivedFrom<Ast::IntegerType>() || !param.astType->isDerivedFrom<Ast::IntegerType>()) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::IncompatibleOperatorTypesNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param.astType, astContentType, astNode, param.targetData.get(), false, param.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgContentType;
  if (!g->getGeneratedType(astContentType, deps.tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S(">>=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateShrAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("<<=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateShlAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("&=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateAndAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("|=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateOrAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
      )) return false;
    }
    result.astType = astRefType;
    return true;
  } else if (astNode->getType() == S("$=")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateXorAssign(
        deps.tgContext, tgContentType, target.targetData.get(), param.targetData.get(), result.targetData
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, deps, param
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param.astType, astTargetType, astNode, param.targetData.get(), false, param.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, deps.tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("-")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateNeg(deps.tgContext, tgTargetType, param.targetData.get(), result.targetData)) return false;
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), true, deps, param
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

  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, param.astType, astTargetType, astNode, param.targetData.get(), false, param.targetData
    )) {
      throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
    }
  }

  TiObject *tgTargetType;
  if (!g->getGeneratedType(astTargetType, deps.tg, tgTargetType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->getType() == S("!") || astNode->getType() == S("!!") || astNode->getType() == S("not")) {
    if (deps.tgContext != 0) {
      if (!deps.tg->generateNot(deps.tgContext, tgTargetType, param.targetData.get(), result.targetData)) return false;
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
  Generation *g, GenDeps const &deps, GenResult &result
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
    static_cast<Ast::Type*>(paramAstTypes->get(0)), paramTgValues->getElement(0), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
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
  if (!g->getGeneratedType(astContentType, deps.tg, tgContentType, 0)) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating arithmetic op result target type."));
  }

  if (astNode->isDerivedFrom<Core::Data::Ast::PrefixOperator>()) {
    if (astNode->getType() == S("--")) {
      if (deps.tgContext != 0) {
        if (!deps.tg->generateEarlyDec(
          deps.tgContext, tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (deps.tgContext != 0) {
        if (!deps.tg->generateEarlyInc(
          deps.tgContext, tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else {
      throw EXCEPTION(InvalidArgumentException, S("astNode"), S("Does not represent an arithmetic operator."));
    }
  } else {
    if (astNode->getType() == S("--")) {
      if (deps.tgContext != 0) {
        if (!deps.tg->generateLateDec(
          deps.tgContext, tgContentType, target.targetData.get(), result.targetData
        )) return false;
      }
      result.astType = astContentType;
      return true;
    } else if (astNode->getType() == S("++")) {
      if (deps.tgContext != 0) {
        if (!deps.tg->generateLateInc(
          deps.tgContext, tgContentType, target.targetData.get(), result.targetData
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
  TiObject *self, Spp::Ast::PointerOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
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
    if (!g->generateFunction(astFunction, deps)) return false;
    auto tgFunction = getCodeGenData<TiObject>(astFunction);
    auto astFunctionPointerType = expGenerator->astHelper->getPointerTypeFor(astFunction->getType().get());
    TiObject *tgFunctionPointerType;
    if (!g->getGeneratedType(astFunctionPointerType, deps.tg, tgFunctionPointerType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate function pointer type."));
    }
    if (deps.tgContext != 0) {
      if (!deps.tg->generateFunctionPointer(deps.tgContext, tgFunction, tgFunctionPointerType, result.targetData)) return false;
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
    result.astType = expGenerator->astHelper->swichInnerReferenceTypeWithPointerType(operandRefAstType);
    TiObject *tgType;
    if (!g->getGeneratedType(result.astType, deps.tg, tgType, 0)) return false;
    if (result.astType == 0) return false;
    result.targetData = operandResult.targetData;
    return true;
  }
}


Bool ExpressionGenerator::_generateAstRefOp(
  TiObject *self, Spp::Ast::AstRefOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("AstRefOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
  if (operandResult.astNode != 0) {
    // We have an AST node.
    // Generate pointer to void.
    auto voidType = expGenerator->astHelper->getVoidType();
    auto voidPtrType = expGenerator->astHelper->getPointerTypeFor(voidType);
    TiObject *tgVoidPtrType;
    if (!g->getGeneratedType(voidPtrType, deps.tg, tgVoidPtrType, 0)) {
      return false;
    }
    // Generate a pointer literal.
    if (!deps.tg->generatePointerLiteral(deps.tgContext, tgVoidPtrType, operandResult.astNode, result.targetData)) {
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
  TiObject *self, Spp::Ast::ContentOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the pointer itself, not a reference to a pointer.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), true, deps, derefResult
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
  TiObject *self, Spp::Ast::CastOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidReferenceNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Get the deepest reference.
  GenResult derefResult;
  if (!expGenerator->dereferenceIfNeeded(
    operandResult.astType, operandResult.targetData.get(), false, deps, derefResult
  )) return false;

  // Get the target type.
  auto targetAstType = expGenerator->astHelper->traceType(astNode->getTargetType().get());
  if (targetAstType == 0) return false;

  // Cast the value.
  Bool retVal;
  if (deps.tgContext != 0) {
    retVal = g->generateCast(
      deps, derefResult.astType, targetAstType, astNode, derefResult.targetData.get(), false, result.targetData
    );
  } else {
    retVal = expGenerator->astHelper->isExplicitlyCastableTo(
      derefResult.astType, targetAstType, deps.tg->getExecutionContext()
    );
  }
  if (!retVal) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidCastNotice>(astNode->getSourceLocation()));
    return false;
  }
  result.astType = targetAstType;
  return true;
}


Bool ExpressionGenerator::_generateSizeOp(
  TiObject *self, Spp::Ast::SizeOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Get the operand type.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("PointerOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, GenDeps(deps, 0), operandResult)) return false;
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
  astType = expGenerator->astHelper->tryGetDeepReferenceContentType(astType);

  // Get the allocation size.
  auto sourceLocation = Core::Data::Ast::findSourceLocation(operand).get();
  if (sourceLocation != 0) expGenerator->noticeStore->pushPrefixSourceLocation(sourceLocation);
  Word size;
  auto retVal = g->getTypeAllocationSize(astType, deps.tg, size);
  if (sourceLocation != 0) {
    expGenerator->noticeStore->popPrefixSourceLocation(Core::Data::getSourceLocationRecordCount(sourceLocation));
  }
  if (!retVal) return false;

  // Generate the Word64 type needed for the result.
  auto bitCount = expGenerator->astHelper->getNeededWordSize(size);
  if (bitCount < 8) bitCount = 8;
  auto astWordType = expGenerator->astHelper->getWordType(bitCount);

  // Generate a constant with that size.
  if (deps.tgContext != 0) {
    if (!deps.tg->generateIntLiteral(deps.tgContext, bitCount, false, size, result.targetData)) return false;
  }
  result.astType = astWordType;
  return true;
}


Bool ExpressionGenerator::_generateInitOp(
  TiObject *self, Spp::Ast::InitOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidInitOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference and get content type.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(operandResult.astType), operandResult.targetData.get(), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidInitOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  // Prepare parameters list.
  SharedList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  PlainList<TiObject> paramAstNodes;
  auto param = astNode->getParam().get();
  if (!expGenerator->generateParamList(param, g, deps, &paramAstNodes, &paramAstTypes, &paramTgValues)) {
    return false;
  }

  if (deps.tgContext != 0) {
    if (!g->generateVarInitialization(
      astContentType, target.targetData.get(), astNode, &paramAstNodes, &paramAstTypes, &paramTgValues, deps
    )) return false;
  }
  result = target;
  return true;
}


Bool ExpressionGenerator::_generateTerminateOp(
  TiObject *self, Spp::Ast::TerminateOp *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate the operand.
  auto operand = astNode->getOperand().get();
  if (operand == 0) {
    throw EXCEPTION(GenericException, S("ContentOp operand is missing."));
  }
  GenResult operandResult;
  if (!expGenerator->generate(operand, g, deps, operandResult)) return false;
  if (operandResult.astType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTerminateOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }

  // Dereference and get content type.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(
    static_cast<Ast::Type*>(operandResult.astType), operandResult.targetData.get(), false, deps, target
  )) return false;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTerminateOperandNotice>(Core::Data::Ast::findSourceLocation(operand))
    );
    return false;
  }
  Ast::Type *astContentType = astRefType->getContentType(expGenerator->astHelper);

  if (deps.tgContext != 0) {
    if (!g->generateVarDestruction(astContentType, target.targetData.get(), astNode, deps)) return false;
  }
  result = target;
  return true;
}


Bool ExpressionGenerator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = &astNode->getValue().getStr();

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, deps.tg, charTgType, 0)) return false;

  auto strAstType = expGenerator->astHelper->getCharArrayType(value->size() + 1);
  TiObject *strTgType;
  if (!g->getGeneratedType(strAstType, deps.tg, strTgType, 0)) return false;

  auto strPtrAstType = expGenerator->astHelper->getPointerTypeFor(strAstType);
  TiObject *strPtrTgType;
  if (!g->getGeneratedType(strPtrAstType, deps.tg, strPtrTgType, 0)) return false;

  if (deps.tgContext != 0) {
    if (expGenerator->offlineExecution) {
      if (!deps.tg->generateStringLiteral(deps.tgContext, value->c_str(), charTgType, strTgType, result.targetData)) return false;
    } else {
      if (!deps.tg->generatePointerLiteral(deps.tgContext, strPtrTgType, (void*)value->c_str(), result.targetData)) return false;
    }
  }
  result.astType =  strPtrAstType;
  return true;
}


Bool ExpressionGenerator::_generateCharLiteral(
  TiObject *self, Core::Data::Ast::CharLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto value = astNode->getValue().getStr()[0];

  auto charAstType = expGenerator->astHelper->getCharType();
  TiObject *charTgType;
  if (!g->getGeneratedType(charAstType, deps.tg, charTgType, 0)) return false;

  auto bitCount = charAstType->getBitCount(expGenerator->astHelper);

  if (deps.tgContext != 0) {
    if (!deps.tg->generateIntLiteral(deps.tgContext, bitCount, false, value, result.targetData)) return false;
  }
  result.astType = charAstType;
  return true;
}


Bool ExpressionGenerator::_generateIntegerLiteral(
  TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
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
  Bool retVal = g->getGeneratedType(astType, deps.tg, intTgType, 0);
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (deps.tgContext != 0) {
    if (!deps.tg->generateIntLiteral(deps.tgContext, size, signedNum, value, result.targetData)) return false;
  }
  result.astType = astType;
  return true;
}


Bool ExpressionGenerator::_generateFloatLiteral(
  TiObject *self, Core::Data::Ast::FloatLiteral *astNode, Generation *g, GenDeps const &deps, GenResult &result
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
  Bool retVal = g->getGeneratedType(floatAstType, deps.tg, floatTgType, 0);
  expGenerator->noticeStore->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!retVal) return false;

  // Generate the literal.
  if (deps.tgContext != 0) {
    if (!deps.tg->generateFloatLiteral(deps.tgContext, size, value, result.targetData)) return false;
  }
  result.astType = floatAstType;
  return true;
}


//==============================================================================
// Inner Generation Functions

Bool ExpressionGenerator::_generateVarReference(
  TiObject *self, TiObject *refAstNode, TiObject *varAstNode, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  auto tgVar = tryGetCodeGenData<TiObject>(varAstNode);
  if (tgVar == 0) {
    // Generate the variable definition.
    auto varDef = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(varAstNode)->getOwner());
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Unexpected error while looking for variable definition."));
    }
    if (expGenerator->getAstHelper()->getDefinitionDomain(varDef) == Ast::DefinitionDomain::GLOBAL) {
      if (!g->generateVarDef(varDef, deps)) return false;
      tgVar = getCodeGenData<TiObject>(varAstNode);
    } else {
      expGenerator->noticeStore->add(std::make_shared<Spp::Notices::UninitializedLocalVariableNotice>(
        Core::Data::Ast::findSourceLocation(refAstNode)
      ));
      return false;
    }
  }

  auto astType = Ast::getAstType(varAstNode);
  TiObject *tgType;
  if (!g->getGeneratedType(astType, deps.tg, tgType, 0)) return false;

  if (deps.tgContext != 0) {
    if (!deps.tg->generateVarReference(deps.tgContext, tgType, tgVar, result.targetData)) return false;
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astType);
  return true;
}


Bool ExpressionGenerator::_generateMemberReference(
  TiObject *self, TiObject *tgValue, Ast::Type * astType, Core::Data::Ast::Identifier *astNode,
  Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Get the deepest reference.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(astType, tgValue, false, deps, target)) return false;

  // Prepare the struct type.
  Ast::Type *astStructType;
  Ast::Type *astStructValueType;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    astStructValueType = target.astType;
    astStructType = astStructValueType;
  } else {
    astStructValueType = astRefType->getContentType(expGenerator->astHelper);
    // TargetGenerator expects a pointer type as it doesn't have the concept of references, so we need to
    // get the pointer type for the given reference type.
    astStructType = expGenerator->astHelper->getPointerTypeFor(astStructValueType);
  }
  if (!astStructValueType->isDerivedFrom<Ast::DataType>()) {
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidOperationNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  // Find the member variable.
  auto body = static_cast<Ast::DataType*>(astStructValueType)->getBody().get();
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
  // Make sure the member is not an alias to a non-member.
  auto astMemberVarNode = ti_cast<Core::Data::Node>(astMemberVar);
  if (astMemberVarNode == 0 || astMemberVarNode->getOwner() == 0 || astMemberVarNode->getOwner()->getOwner() != body) {
    // The found member was probably an alias to a non member.
    expGenerator->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeMemberNotice>(astNode->findSourceLocation())
    );
    return false;
  }

  return expGenerator->generateMemberVarReference(
    astNode, target.targetData.get(), astStructType, astMemberVar, g, deps, result
  );
}


Bool ExpressionGenerator::_generateMemberVarReference(
  TiObject *self, Core::Data::Node *astNode, TiObject *tgStructValue, Ast::Type * astStructType, TiObject *astMemberVar,
  Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  TiObject *tgStructType;
  if (!g->getGeneratedType(astStructType, deps.tg, tgStructType, 0)) return false;

  // Make sure the var is an object member.
  if (expGenerator->getAstHelper()->getDefinitionDomain(astMemberVar) != Ast::DefinitionDomain::OBJECT) {
    expGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidGlobalDefAccessNotice>(
      Core::Data::Ast::findSourceLocation(astNode)
    ));
    return false;
  }

  // Get the member generated value and type.
  auto tgMemberVar = getCodeGenData<TiObject>(astMemberVar);
  auto astMemberType = Ast::getAstType(astMemberVar);
  TiObject *tgMemberType;
  if (!g->getGeneratedType(astMemberType, deps.tg, tgMemberType, 0)) return false;

  // Generate member access.
  if (deps.tgContext != 0) {
    if (!deps.tg->generateMemberVarReference(
      deps.tgContext, tgStructType, tgMemberType, tgMemberVar, tgStructValue, result.targetData
    )) {
      return false;
    }
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astMemberType);
  return true;
}


Bool ExpressionGenerator::_generateArrayReference(
  TiObject *self, Core::Data::Node *astNode, TiObject *tgValue, Ast::Type *astType, TiObject *tgIndexVal,
  Ast::Type *astIndexType, Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Cast the index to int64.
  TioSharedPtr tgCastedIndex;
  if (deps.tgContext != 0) {
    if (!g->generateCast(
      deps, astIndexType, expGenerator->astHelper->getInt64Type(), astNode, tgIndexVal, false, tgCastedIndex
    )) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
    }
  }

  // Get the deepest reference.
  GenResult target;
  if (!expGenerator->dereferenceIfNeeded(astType, tgValue, false, deps, target)) return false;

  // Prepare the array type.
  TiObject *tgArrayType;
  auto astRefType = ti_cast<Ast::ReferenceType>(target.astType);
  if (astRefType == 0) {
    if (!g->getGeneratedType(astType, deps.tg, tgArrayType, &astType)) return false;
  } else {
    astType = astRefType->getContentType(expGenerator->astHelper);
    // TargetGenerator expects a pointer type as it doesn't have the concept of references, so we need to
    // get the pointer type for the given reference type.
    auto astPtrType = expGenerator->astHelper->getPointerTypeFor(astType);
    if (!g->getGeneratedType(astPtrType, deps.tg, tgArrayType, 0)) return false;
  }
  auto astArrayType = ti_cast<Ast::ArrayType>(astType);
  if (astArrayType == 0) {
    throw EXCEPTION(GenericException, S("Unexpected type for array reference."));
  }

  // Find element type.
  auto astElementType = astArrayType->getContentType(expGenerator->astHelper);
  TiObject *tgElementType;
  if (!g->getGeneratedType(astElementType, deps.tg, tgElementType, 0)) return false;

  // Generate member access.
  if (deps.tgContext != 0) {
    if (!deps.tg->generateArrayElementReference(
      deps.tgContext, tgArrayType, tgElementType, tgCastedIndex.get(), target.targetData.get(), result.targetData)) {
      return false;
    }
  }
  result.astType = expGenerator->astHelper->getReferenceTypeFor(astElementType);
  return true;
}


Bool ExpressionGenerator::_generateFunctionCall(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
  Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
  Generation *g, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (callee->getInlined()) {
    // TODO: Generate inlined function body.
    throw EXCEPTION(GenericException, S("Inline function generation is not implemented yet."));
  } else {
    // Build called funcion.
    if (!g->generateFunction(callee, deps)) return false;
    auto tgFunction = getCodeGenData<TiObject>(callee);

    // Create function call.
    auto astRetType = callee->getType()->traceRetType(expGenerator->astHelper);
    if (astRetType->hasCustomInitialization(expGenerator->astHelper, deps.tg->getExecutionContext())) {
      if (deps.tgContext != 0) {
        // Pass a reference to the return value.
        if (!g->generateTempVar(astNode, astRetType, deps, false)) return false;
        TioSharedPtr tgResult;
        if (!deps.tg->generateVarReference(
          deps.tgContext, getCodeGenData<TiObject>(astRetType), getCodeGenData<TiObject>(astNode), tgResult
        )) {
          return false;
        }
        PlainList<TiObject> paramTgValuesWithRet;
        paramTgValuesWithRet.add(tgResult.get());
        for (Int i = 0; i < paramTgValues->getElementCount(); ++i) {
          paramTgValuesWithRet.add(paramTgValues->getElement(i));
        }
        if (!deps.tg->generateFunctionCall(deps.tgContext, tgFunction, &paramTgValuesWithRet, result.targetData)) return false;
        result.targetData = tgResult;
        // Register temp var for destruction.
        g->registerDestructor(astNode, astRetType, deps.tg->getExecutionContext(), deps.destructionStack);
      }
      result.astType = expGenerator->astHelper->getReferenceTypeFor(astRetType);
    } else {
      if (deps.tgContext != 0) {
        if (!deps.tg->generateFunctionCall(deps.tgContext, tgFunction, paramTgValues, result.targetData)) return false;
      }
      result.astType = astRetType;
    }
    return true;
  }
}


Bool ExpressionGenerator::_prepareFunctionParams(
  TiObject *self, Spp::Ast::FunctionType *calleeType, Generation *g, GenDeps const &deps,
  DynamicContaining<TiObject> *paramAstNodes, DynamicContaining<TiObject> *paramAstTypes,
  SharedList<TiObject> *paramTgVals
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  Ast::FunctionType::ArgMatchContext context;
  for (Int i = 0; i < paramTgVals->getElementCount(); ++i) {
    Ast::Type *srcType = static_cast<Ast::Type*>(paramAstTypes->getElement(i));
    auto status = calleeType->matchNextArg(srcType, context, expGenerator->astHelper, deps.tg->getExecutionContext());
    ASSERT(status != Ast::TypeMatchStatus::NONE);

    // Cast the value if needed.
    if (context.type != 0) {
      Ast::Type *neededAstType;
      if (context.type->hasCustomInitialization(expGenerator->astHelper, deps.tg->getExecutionContext())) {
        neededAstType = expGenerator->astHelper->getReferenceTypeFor(context.type);
      } else {
        neededAstType = context.type;
      }
      TioSharedPtr tgCastedVal;
      if (deps.tgContext != 0) {
        if (!g->generateCast(
          deps, srcType, neededAstType, ti_cast<Core::Data::Node>(paramAstNodes->getElement(i)),
          paramTgVals->getElement(i), false, tgCastedVal
        )) {
          throw EXCEPTION(GenericException, S("Casting unexpectedly failed."));
        };
      }
      paramTgVals->set(i, tgCastedVal);
    } else {
      // For var args we need to send values, not references.
      GenResult result;
      if (!expGenerator->dereferenceIfNeeded(srcType, paramTgVals->getElement(i), true, deps, result)) {
        throw EXCEPTION(GenericException, S("Unexpected error."));
      }
      paramTgVals->set(i, result.targetData);
    }
  }
  return true;
}


Bool ExpressionGenerator::_generateCalleeReferenceChain(
  TiObject *self, Ast::CalleeLookupResult const &calleeInfo, Core::Data::Node *astNode, GenResult const &prevResult,
  Generation *g, GenDeps const &deps, GenResult &calleeResult, GenResult &thisResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  if (calleeInfo.thisIndex == -1) {
    thisResult = prevResult;
  } else {
    thisResult.astNode = 0;
    thisResult.astType = 0;
    thisResult.targetData.reset();
  }
  calleeResult = prevResult;
  for (Int i = 0; i < calleeInfo.stack.getCount(); ++i) {
    auto item = calleeInfo.stack.get(i);
    if (item->isDerivedFrom<Ast::Function>()) {
      calleeResult.astNode = item;
      calleeResult.astType = 0;
      calleeResult.targetData.reset();
      break;
    } else {
      if (calleeResult.astType != 0) {
        // Generate member var reference.
        GenResult structResult;
        if (!expGenerator->dereferenceIfNeeded(
          calleeResult.astType, calleeResult.targetData.get(), false, deps, structResult
        )) return false;
        if (!expGenerator->generateMemberVarReference(
          astNode, structResult.targetData.get(), structResult.astType, item, g, deps, calleeResult
        )) return false;
      } else {
        // Generate global var reference.
        if (!expGenerator->generateVarReference(astNode, item, g, deps, calleeResult)) return false;
      }
      if (calleeInfo.thisIndex == i) {
        thisResult = calleeResult;
      }
    }
  }
  return true;
}


//==============================================================================
// Helper Functions

Bool ExpressionGenerator::generateParamList(
  TiObject *astNode, Generation *g, GenDeps const &deps,
  DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes,
  SharedList<TiObject> *resultValues
) {
  if (astNode == 0) return true;

  if (astNode->isDerivedFrom<Core::Data::Ast::List>()) {
    if (!this->generateParamList(
      ti_cast<Containing<TiObject>>(astNode), g, deps, resultAstNodes, resultTypes, resultValues
    )) return false;
  } else {
    GenResult result;
    if (!this->generate(astNode, g, deps, result)) return false;
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
  Containing<TiObject> *astNodes, Generation *g, GenDeps const &deps,
  DynamicContaining<TiObject> *resultAstNodes, DynamicContaining<TiObject> *resultTypes,
  SharedList<TiObject> *resultValues
) {
  for (Int i = 0; i < astNodes->getElementCount(); ++i) {
    GenResult result;
    if (!this->generate(astNodes->getElement(i), g, deps, result)) return false;
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


Bool ExpressionGenerator::dereferenceIfNeeded(
  Spp::Ast::Type *astType, TiObject *tgValue, Bool valueNeeded, GenDeps const &deps, GenResult &result
) {
  auto refType = ti_cast<Spp::Ast::ReferenceType>(astType);
  if (refType != 0) {
    Spp::Ast::Type *derefAstType = refType->getContentType(this->astHelper);
    if (ti_cast<Spp::Ast::ReferenceType>(derefAstType) != 0 || valueNeeded) {
      TioSharedPtr derefTgValue;
      if (deps.tgContext != 0) {
        auto tgContentType = getCodeGenData<TiObject>(derefAstType);
        if (!deps.tg->generateDereference(deps.tgContext, tgContentType, tgValue, derefTgValue)) return false;
      }
      return this->dereferenceIfNeeded(derefAstType, derefTgValue.get(), valueNeeded, deps, result);
    }
  }
  result.astType = astType;
  result.targetData = getSharedPtr(tgValue);
  return true;
}


Bool ExpressionGenerator::castLogicalOperand(
  Generation *g, GenDeps const &deps, TiObject *astNode, Spp::Ast::Type *astType,
  TiObject *tgValue, TioSharedPtr &result
) {
  auto boolType = this->astHelper->getBoolType();
  if (astType == 0) {
    this->noticeStore->add(std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>());
    return false;
  }
  if (deps.tgContext != 0) {
    if (!g->generateCast(deps, astType, boolType, ti_cast<Core::Data::Node>(astNode), tgValue, true, result)) {
      this->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
  } else if (!this->astHelper->isImplicitlyCastableTo(astType, boolType, deps.tg->getExecutionContext())) {
    this->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidLogicalOperandNotice>(Core::Data::Ast::findSourceLocation(astNode))
    );
    return false;
  }

  return true;
}

} } // namespace
