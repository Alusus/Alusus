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
  this->generateParamPass = &ExpressionGenerator::_generateParamPass;
  this->generateInfixOp = &ExpressionGenerator::_generateInfixOp;
  this->generateAssignment = &ExpressionGenerator::_generateAssignment;
  this->generateFunctionCall = &ExpressionGenerator::_generateFunctionCall;
  this->generateBuiltInFunctionCall = &ExpressionGenerator::_generateBuiltInFunctionCall;
  this->generateUserFunctionCall = &ExpressionGenerator::_generateUserFunctionCall;
}


//==============================================================================
// Code Generation Functions

Bool ExpressionGenerator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);
  auto operand = astNode->getOperand().get();
  if (!expGenerator->generator->generatePhrase(
    operand, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef)
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
    if (!expGenerator->lookupFunction(operand, astNode, &paramTypes, function)) return false;
    // Generate the functionc all.
    Bool result = expGenerator->generateFunctionCall(
      function, &paramTypes, &paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
    );
    if (result) lastProcessedRef = astNode;
    return result;
  }
}


Bool ExpressionGenerator::_generateInfixOp(
  TiObject *self, Core::Data::Ast::InfixOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(expGenerator, ExpressionGenerator);

  // Determine operator function name.
  Char const *funcName;
  if (astNode->getType() == STR("+")) funcName = STR("__add");
  else if (astNode->getType() == STR("-")) funcName = STR("__sub");
  else if (astNode->getType() == STR("*")) funcName = STR("__mul");
  else if (astNode->getType() == STR("/")) funcName = STR("__div");
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
  if (!expGenerator->lookupFunction(funcName, astNode, &paramTypes, function)) return false;

  // Generate the functionc all.
  Bool result = expGenerator->generateFunctionCall(
    function, &paramTypes, &paramCgs, llvmIrBuilder, llvmFunc, resultType, llvmResult
  );
  if (result) lastProcessedRef = astNode;
  return result;
}


Bool ExpressionGenerator::_generateAssignment(
  TiObject *self, Core::Data::Ast::AssignmentOperator *astNode, llvm::IRBuilder<> *llvmIrBuilder,
  llvm::Function *llvmFunc, Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
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
  if (!expGenerator->generator->getVariableGenerator()->generateVarReference(
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
    expGenerator->generator->getSourceLocationStack()->push_back(metadata->getSourceLocation());
    expGenerator->generator->getParserState()->addNotice(
      std::make_shared<NotImplicitlyCastableNotice>(expGenerator->generator->getSourceLocationStack())
    );
    expGenerator->generator->getSourceLocationStack()->pop_back();
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
  lastProcessedRef = astNode;
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


Bool ExpressionGenerator::lookupFunction(
  Char const *name, Core::Data::Node *astNode, Core::Basic::Container<Core::Basic::TiObject> *types,
  Ast::Function *&function
) {
  Core::Data::Ast::Identifier identifier;
  identifier.setValue(name);
  return this->lookupFunction(&identifier, astNode, types, function);
}


Bool ExpressionGenerator::lookupFunction(
  TiObject *ref, Core::Data::Node *astNode, Core::Basic::Container<Core::Basic::TiObject> *types,
  Ast::Function *&function
) {
  function = 0;
  using CallMatchStatus = Ast::Function::CallMatchStatus;
  CallMatchStatus matchStatus = CallMatchStatus::NONE;
  Int matchCount = 0;
  SharedPtr<Core::Data::Notice> notice;
  this->generator->getSeeker()->doForeach(ref, astNode->getOwner(),
    [=, &function, &matchStatus, &matchCount, &notice]
      (TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
          auto f = static_cast<Ast::Function*>(obj);
          CallMatchStatus ms;
          ms = f->matchCall(
            types, this->generator->getExecutionContext().get(), this->generator->getRootManager()
          );
          if (ms != CallMatchStatus::NONE && matchStatus == CallMatchStatus::NONE) {
            function = f;
            matchStatus = ms;
            matchCount = 1;
          } else if (ms == CallMatchStatus::CASTED && matchStatus == CallMatchStatus::CASTED) {
            matchCount++;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::CASTED) {
            function  = f;
            matchStatus = ms;
            matchCount = 1;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::EXACT) {
            matchCount++;
          }
        }
        return Core::Data::Seeker::Verb::MOVE;
      }
  );
  // Did we have a matched function to call?
  if (notice != 0 && (matchCount > 1 || function == 0)) {
    notice->prependSourceLocationStack(this->generator->getSourceLocationStack());
    this->generator->getParserState()->addNotice(notice);
  }
  if (matchCount > 1) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    this->generator->getSourceLocationStack()->push_back(metadata->getSourceLocation());
    this->generator->getParserState()->addNotice(
      std::make_shared<MultipleCalleeMatchNotice>(this->generator->getSourceLocationStack())
    );
    this->generator->getSourceLocationStack()->pop_back();
    return false;
  } else if (function == 0) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    this->generator->getSourceLocationStack()->push_back(metadata->getSourceLocation());
    this->generator->getParserState()->addNotice(
      std::make_shared<NoCalleeMatchNotice>(this->generator->getSourceLocationStack())
    );
    this->generator->getSourceLocationStack()->pop_back();
    return false;
  }

  return true;
}

} } // namespace
