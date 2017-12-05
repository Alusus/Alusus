/**
 * @file Spp/LlvmCodeGen/ExpressionGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::ExpressionGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace LlvmCodeGen
{

//==============================================================================
// Initialization Functions

void ExpressionGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateIdentifierAccess,
    &this->generateIdentifierReference,
    &this->generateParamPass,
    &this->generateInfixOp,
    &this->generateAssignment,
    &this->generateFunctionCall,
    &this->generateBuiltInFunctionCall,
    &this->generateUserFunctionCall
  });
}


void ExpressionGenerator::initBindings()
{
  this->generateIdentifierAccess = &ExpressionGenerator::_generateIdentifierAccess;
  this->generateIdentifierReference = &ExpressionGenerator::_generateIdentifierReference;
  this->generateParamPass = &ExpressionGenerator::_generateParamPass;
  this->generateInfixOp = &ExpressionGenerator::_generateInfixOp;
  this->generateAssignment = &ExpressionGenerator::_generateAssignment;
  this->generateFunctionCall = &ExpressionGenerator::_generateFunctionCall;
  this->generateBuiltInFunctionCall = &ExpressionGenerator::_generateBuiltInFunctionCall;
  this->generateUserFunctionCall = &ExpressionGenerator::_generateUserFunctionCall;
}


//==============================================================================
// Code Generation Functions

Bool ExpressionGenerator::_generateIdentifierAccess(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  resultType = 0;
  llvmResult = 0;
  lastProcessedNode = 0;
  Bool result = false;
  Bool symbolFound = false;
  expGenerator->generator->getSeeker()->doForeach(astNode, astNode->getOwner(),
    [=, &result, &lastProcessedNode, &llvmResult, &resultType, &symbolFound]
    (TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      // Check if the found obj is a variable definition.
      if (Ast::isVarDefinition(obj)) {
        result = expGenerator->generator->getVariableGenerator()->generateVarAccess(
          obj, llvmIrBuilder, llvmFunc, resultType, llvmResult
        );
        if (result) {
          lastProcessedNode = astNode;
        }
      } else if (obj->isDerivedFrom<Ast::Function>()) {
        result = true;
      } else {
        expGenerator->generator->getNoticeStore()->add(
          std::make_shared<InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    }
  );
  if (!symbolFound) {
    expGenerator->generator->getNoticeStore()->add(
      std::make_shared<UnknownSymbolNotice>(astNode->findSourceLocation())
    );
  }
  return result;
}


Bool ExpressionGenerator::_generateIdentifierReference(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  resultType = 0;
  llvmResult = 0;
  lastProcessedNode = 0;
  Bool result = false;
  Bool symbolFound = false;
  expGenerator->generator->getSeeker()->doForeach(astNode, astNode->getOwner(),
    [=, &result, &lastProcessedNode, &llvmResult, &resultType, &symbolFound]
    (TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      // Check if the found obj is a variable definition.
      if (Ast::isVarDefinition(obj)) {
        result = expGenerator->generator->getVariableGenerator()->generateVarReference(
          obj, llvmIrBuilder, llvmFunc, resultType, llvmResult
        );
        if (result) {
          lastProcessedNode = astNode;
        }
      } else {
        expGenerator->generator->getNoticeStore()->add(
          std::make_shared<InvalidReferenceNotice>(astNode->findSourceLocation())
        );
      }
      return Core::Data::Seeker::Verb::STOP;
    }
  );
  if (!symbolFound) {
    expGenerator->generator->getNoticeStore()->add(
      std::make_shared<UnknownSymbolNotice>(astNode->findSourceLocation())
    );
  }
  return result;
}


Bool ExpressionGenerator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();
  if (!expGenerator->generator->generatePhrase(
    operand, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode)
  ) {
    return false;
  }
  if (resultType != 0) {
    // TODO: Check for function pointers and arrays.
    return false;
  } else {
    // Prepare parameters list.
    using Core::Basic::TiObject;
    using Core::Basic::PlainList;
    PlainList<llvm::Value, TiObject> paramCgs;
    PlainList<TiObject, TiObject> paramTypes;
    auto param = astNode->getParam().get();
    if (param != 0) {
      if (param->isDerivedFrom<Core::Data::Ast::ExpressionList>()) {
        if (!expGenerator->generateParamList(
          ti_cast<Core::Data::Container>(param), llvmIrBuilder, llvmFunc, &paramTypes, &paramCgs
        )) return false;
      } else {
        llvm::Value *paramResultCg;
        Ast::Type *paramType;
        TiObject *paramLastProcessedRef;
        if (!expGenerator->generator->generatePhrase(
          param, llvmIrBuilder, llvmFunc, paramType, paramResultCg, paramLastProcessedRef
        )) return false;
        paramCgs.add(paramResultCg);
        paramTypes.add(paramType);
      }
    }
    // Look for a matching function to call.
    Ast::Function *function = 0;
    if (!Ast::lookupFunction(
      operand, astNode, &paramTypes, expGenerator->generator->getExecutionContext().get(),
      expGenerator->generator->getRootManager(), expGenerator->generator->getNoticeStore(), function
    )) {
      return false;
    }
    // Generate the functionc all.
    Bool result = expGenerator->generateFunctionCall(
      function, &paramTypes, &paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
    );
    if (result) lastProcessedNode = astNode;
    return result;
  }
}


Bool ExpressionGenerator::_generateInfixOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Determine operator function name.
  Char const *funcName;
  if (astNode->getType() == STR("+")) funcName = STR("__add");
  else if (astNode->getType() == STR("-")) funcName = STR("__sub");
  else if (astNode->getType() == STR("*")) funcName = STR("__mul");
  else if (astNode->getType() == STR("/")) funcName = STR("__div");
  else if (astNode->getType() == STR("==")) funcName = STR("__equal");
  else if (astNode->getType() == STR("!=")) funcName = STR("__notEqual");
  else if (astNode->getType() == STR(">")) funcName = STR("__greaterThan");
  else if (astNode->getType() == STR(">=")) funcName = STR("__greaterThanOrEqual");
  else if (astNode->getType() == STR("<")) funcName = STR("__lessThan");
  else if (astNode->getType() == STR("<=")) funcName = STR("__lessThanOrEqual");
  else {
    throw EXCEPTION(GenericException, STR("Unexpected infix operator."));
  }

  // Prepare parameters list.
  using Core::Basic::TiObject;
  using Core::Basic::PlainList;
  PlainList<llvm::Value, TiObject> paramCgs;
  PlainList<TiObject, TiObject> paramTypes;
  if (!expGenerator->generateParamList(
    ti_cast<Core::Data::Container>(astNode), llvmIrBuilder, llvmFunc, &paramTypes, &paramCgs
  )) return false;

  // Look for a matching function to call.
  Ast::Function *function = 0;
  if (!Ast::lookupFunction(
    funcName, astNode, &paramTypes, expGenerator->generator->getExecutionContext().get(),
    expGenerator->generator->getRootManager(), expGenerator->generator->getNoticeStore(), function
  )) {
    return false;
  }

  // Generate the functionc all.
  Bool result = expGenerator->generateFunctionCall(
    function, &paramTypes, &paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
  );
  if (result) lastProcessedNode = astNode;
  return result;
}


Bool ExpressionGenerator::_generateAssignment(
  TiObject *self, Core::Data::Ast::AssignmentOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder,
  llvm::Function *llvmFunc, Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Generate assignment target.
  auto var = astNode->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (var == 0) {
    throw EXCEPTION(GenericException, STR("Assignment target is missing."));
  }
  Ast::Type *varAstType;
  llvm::Value *varLlvmPointer;
  TiObject *varLastRef;
  if (!expGenerator->generateIdentifierReference(
    var, llvmIrBuilder, llvmFunc, varAstType, varLlvmPointer, varLastRef
  )) return false;

  // Generate assignment value.
  auto val = astNode->getSecond().get();
  if (val == 0) {
    throw EXCEPTION(GenericException, STR("Assignment value is missing."));
  }
  Ast::Type *valAstType;
  llvm::Value *valLlvmValue;
  TiObject *valLastRef;
  if (!expGenerator->generator->generatePhrase(val, llvmIrBuilder, llvmFunc, valAstType, valLlvmValue, valLastRef)) {
    return false;
  }

  // Is value implicitly castable?
  if (!valAstType->isImplicitlyCastableTo(
    varAstType, expGenerator->generator->getExecutionContext().get(), expGenerator->generator->getRootManager()
  )) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(val);
    ASSERT(metadata != 0);
    expGenerator->generator->getNoticeStore()->add(
      std::make_shared<NotImplicitlyCastableNotice>(metadata->findSourceLocation())
    );
    return false;
  }

  // Cast the generated value.
  llvm::Value *castedLlvmVal;
  if (!expGenerator->generator->getTypeGenerator()->generateCast(
    llvmIrBuilder, valAstType, varAstType, valLlvmValue, castedLlvmVal
  )) {
    return false;
  }

  // Create the store instruction.
  llvmIrBuilder->CreateStore(castedLlvmVal, varLlvmPointer);

  // Set return values.
  llvmResult = castedLlvmVal;
  resultType = varAstType;
  lastProcessedNode = astNode;
  return true;
}


Bool ExpressionGenerator::_generateFunctionCall(
  TiObject *self, Spp::Ast::Function *callee,
  Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
  llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Cast arguments, if needed.
  Ast::Function::ArgMatchContext context;
  for (Int i = 0; i < paramCgs->getElementCount(); ++i) {
    Ast::Type *srcType = static_cast<Ast::Type*>(paramTypes->getElement(i));
    auto status = callee->matchNextArg(
      srcType, context, expGenerator->generator->getExecutionContext().get(),
      expGenerator->generator->getRootManager()
    );
    ASSERT(status != Ast::Function::CallMatchStatus::NONE);
    if (context.type != 0) {
      llvm::Value *destValue;
      if (!expGenerator->generator->getTypeGenerator()->generateCast(
        llvmIrBuilder, srcType, context.type, paramCgs->getElement(i), destValue)
      ) {
        // This should not happen since non-castable calls should be filtered out earlier.
        throw EXCEPTION(GenericException, STR("Invalid cast was unexpectedly found."));
      }
      paramCgs->setElement(i, destValue);
    }
  }

  // is function built-in?
  if (callee->getName().getStr().size() > 0 && callee->getName().getStr().at(0) == CHR('#')) {
    return expGenerator->generateBuiltInFunctionCall(
      callee, paramTypes, paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
    );
  } else {
    if (callee->getInlined()) {
      // TODO: Generate inlined function body.
      return false;
    } else {
      return expGenerator->generateUserFunctionCall(
        callee, paramTypes, paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
      );
    }
  }
}


Bool ExpressionGenerator::_generateBuiltInFunctionCall(
  TiObject *self, Spp::Ast::Function *callee,
  Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
  llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  resultType = callee->traceRetType(expGenerator->generator->getSeeker());

  // Binary Math Operations
  if (callee->getName() == STR("#addInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #addInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateAdd(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#addFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #addFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFAdd(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#subInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #subInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateSub(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#subFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #subFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFSub(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#mulInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #mulInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateMul(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#mulFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #mulFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFMul(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#divInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #divInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateSDiv(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#divFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #divFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFDiv(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;

  // Unary Operations
  } else if (callee->getName() == STR("#negInt")) {
    if (paramCgs->getElementCount() != 1) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #negInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateNeg(paramCgs->getElement(0));
    return true;
  } else if (callee->getName() == STR("#negFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #negFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFNeg(paramCgs->getElement(0));
    return true;

  // Int Comparison Operations
  } else if (callee->getName() == STR("#equalInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #equalInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateICmpEQ(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#notEqualInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #notEqualInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateICmpNE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#greaterThanInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #greaterThanInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateICmpSGT(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#greaterThanOrEqualInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #greaterThanOrEqualInt built-in function.")
      );
    }
    llvmResult = llvmIrBuilder->CreateICmpSGE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#lessThanInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #lessThanInt built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateICmpSLT(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#lessThanOrEqualInt")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #lessThanOrEqualInt built-in function.")
      );
    }
    llvmResult = llvmIrBuilder->CreateICmpSLE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;

  // Float Comparison Operations
  } else if (callee->getName() == STR("#equalFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #equalFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFCmpOEQ(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#notEqualFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #notEqualFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFCmpONE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#greaterThanFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #greaterThanFloat built-in function.")
      );
    }
    llvmResult = llvmIrBuilder->CreateFCmpOGT(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#greaterThanOrEqualFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #greaterThanOrEqualFloat built-in function.")
      );
    }
    llvmResult = llvmIrBuilder->CreateFCmpOGE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#lessThanFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(GenericException, STR("Unexpected argument count in call to #lessThanFloat built-in function."));
    }
    llvmResult = llvmIrBuilder->CreateFCmpOLT(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  } else if (callee->getName() == STR("#lessThanOrEqualFloat")) {
    if (paramCgs->getElementCount() != 2) {
      throw EXCEPTION(
        GenericException, STR("Unexpected argument count in call to #lessThanOrEqualFloat built-in function.")
      );
    }
    llvmResult = llvmIrBuilder->CreateFCmpOLE(paramCgs->getElement(0), paramCgs->getElement(1));
    return true;
  }

  throw EXCEPTION(GenericException, STR("Unexpected built-in function."));
}


Bool ExpressionGenerator::_generateUserFunctionCall(
  TiObject *self, Spp::Ast::Function *callee,
  Core::Basic::Container<Core::Basic::TiObject> *paramTypes, Core::Basic::Container<llvm::Value> *paramCgs,
  llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc, Spp::Ast::Type *&resultType, llvm::Value *&llvmResult
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Build funcion declaration.
  if (!expGenerator->generator->generateFunctionDecl(callee)) return false;
  auto llvmFuncBox = callee->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Function*>>();
  // Create function call.
  std::vector<llvm::Value*> args;
  for (Int i = 0; i < paramCgs->getElementCount(); ++i) {
    args.push_back(paramCgs->getElement(i));
  }
  auto llvmCall = llvmIrBuilder->CreateCall(llvmFuncBox->get(), args);
  // Assign results.
  llvmResult = llvmCall;
  resultType = callee->traceRetType(expGenerator->generator->getSeeker());
  return true;
}


//==============================================================================
// Helper Functions

Bool ExpressionGenerator::generateParamList(
  Core::Data::Container *astNodes, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Core::Basic::ListContainer<Core::Basic::TiObject> *resultTypes, Core::Basic::ListContainer<llvm::Value> *llvmResults
) {
  for (Int i = 0; i < astNodes->getCount(); ++i) {
    llvm::Value *paramResultCg;
    Ast::Type *paramType;
    TiObject *paramLastProcessedRef;
    if (!this->generator->generatePhrase(
      astNodes->get(i), llvmIrBuilder, llvmFunc, paramType, paramResultCg, paramLastProcessedRef
    )) return false;
    llvmResults->addElement(paramResultCg);
    resultTypes->addElement(paramType);
  }
  return true;
}

} } // namespace
