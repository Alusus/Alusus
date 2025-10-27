/**
 * @file Spp/LlvmCodeGen/TargetGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::TargetGenerator.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

//==============================================================================
// Initialization Functions

void TargetGenerator::initBindings()
{
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(this);

  // Type Generation Functions
  targetGeneration->generateVoidType = &TargetGenerator::generateVoidType;
  targetGeneration->generateIntType = &TargetGenerator::generateIntType;
  targetGeneration->generateFloatType = &TargetGenerator::generateFloatType;
  targetGeneration->generatePointerType = &TargetGenerator::generatePointerType;
  targetGeneration->generateArrayType = &TargetGenerator::generateArrayType;
  targetGeneration->generateStructTypeDecl = &TargetGenerator::generateStructTypeDecl;
  targetGeneration->generateStructTypeBody = &TargetGenerator::generateStructTypeBody;
  targetGeneration->getTypeAllocationSize = &TargetGenerator::getTypeAllocationSize;
  targetGeneration->getNullaryProcedureType = &TargetGenerator::getNullaryProcedureType;

  // Function Generation Functions
  targetGeneration->generateFunctionType = &TargetGenerator::generateFunctionType;
  targetGeneration->generateFunctionDecl = &TargetGenerator::generateFunctionDecl;
  targetGeneration->prepareFunctionBody = &TargetGenerator::prepareFunctionBody;
  targetGeneration->finishFunctionBody = &TargetGenerator::finishFunctionBody;
  targetGeneration->deleteFunction = &TargetGenerator::deleteFunction;

  // Variable Definition Generation Functions
  targetGeneration->generateGlobalVariable = &TargetGenerator::generateGlobalVariable;
  targetGeneration->generateLocalVariable = &TargetGenerator::generateLocalVariable;

  // Statements Generation Functions
  targetGeneration->prepareIfStatement = &TargetGenerator::prepareIfStatement;
  targetGeneration->finishIfStatement = &TargetGenerator::finishIfStatement;
  targetGeneration->prepareWhileStatement = &TargetGenerator::prepareWhileStatement;
  targetGeneration->finishWhileStatement = &TargetGenerator::finishWhileStatement;
  targetGeneration->prepareForStatement = &TargetGenerator::prepareForStatement;
  targetGeneration->finishForStatement = &TargetGenerator::finishForStatement;
  targetGeneration->generateContinue = &TargetGenerator::generateContinue;
  targetGeneration->generateBreak = &TargetGenerator::generateBreak;

  // Casting Generation Functions
  targetGeneration->generateCastIntToInt = &TargetGenerator::generateCastIntToInt;
  targetGeneration->generateCastIntToFloat = &TargetGenerator::generateCastIntToFloat;
  targetGeneration->generateCastFloatToInt = &TargetGenerator::generateCastFloatToInt;
  targetGeneration->generateCastFloatToFloat = &TargetGenerator::generateCastFloatToFloat;
  targetGeneration->generateCastIntToPointer = &TargetGenerator::generateCastIntToPointer;
  targetGeneration->generateCastPointerToInt = &TargetGenerator::generateCastPointerToInt;
  targetGeneration->generateCastPointerToPointer = &TargetGenerator::generateCastPointerToPointer;

  // Operation Generation Functions
  targetGeneration->generateVarReference = &TargetGenerator::generateVarReference;
  targetGeneration->generateMemberVarReference = &TargetGenerator::generateMemberVarReference;
  targetGeneration->generateArrayElementReference = &TargetGenerator::generateArrayElementReference;
  targetGeneration->generateDereference = &TargetGenerator::generateDereference;
  targetGeneration->generateAssign = &TargetGenerator::generateAssign;
  targetGeneration->generateFunctionPointer = &TargetGenerator::generateFunctionPointer;
  targetGeneration->generateFunctionCall = &TargetGenerator::generateFunctionCall;
  targetGeneration->generateFunctionPtrCall = &TargetGenerator::generateFunctionPtrCall;
  targetGeneration->generateReturn = &TargetGenerator::generateReturn;

  // Logical Ops Generation Functions
  targetGeneration->prepareLogicalOp = &TargetGenerator::prepareLogicalOp;
  targetGeneration->finishLogicalOr = &TargetGenerator::finishLogicalOr;
  targetGeneration->finishLogicalAnd = &TargetGenerator::finishLogicalAnd;

  // Math Ops Generation Functions
  targetGeneration->generateAdd = &TargetGenerator::generateAdd;
  targetGeneration->generateSub = &TargetGenerator::generateSub;
  targetGeneration->generateMul = &TargetGenerator::generateMul;
  targetGeneration->generateDiv = &TargetGenerator::generateDiv;
  targetGeneration->generateRem = &TargetGenerator::generateRem;
  targetGeneration->generateShr = &TargetGenerator::generateShr;
  targetGeneration->generateShl = &TargetGenerator::generateShl;
  targetGeneration->generateAnd = &TargetGenerator::generateAnd;
  targetGeneration->generateOr = &TargetGenerator::generateOr;
  targetGeneration->generateXor = &TargetGenerator::generateXor;
  targetGeneration->generateNot = &TargetGenerator::generateNot;
  targetGeneration->generateNeg = &TargetGenerator::generateNeg;
  targetGeneration->generateEarlyInc = &TargetGenerator::generateEarlyInc;
  targetGeneration->generateEarlyDec = &TargetGenerator::generateEarlyDec;
  targetGeneration->generateLateInc = &TargetGenerator::generateLateInc;
  targetGeneration->generateLateDec = &TargetGenerator::generateLateDec;
  targetGeneration->generateShrAssign = &TargetGenerator::generateShrAssign;
  targetGeneration->generateShlAssign = &TargetGenerator::generateShlAssign;
  targetGeneration->generateAndAssign = &TargetGenerator::generateAndAssign;
  targetGeneration->generateOrAssign = &TargetGenerator::generateOrAssign;
  targetGeneration->generateXorAssign = &TargetGenerator::generateXorAssign;
  targetGeneration->generateNextArg = &TargetGenerator::generateNextArg;

  // Comparison Ops Generation Functions
  targetGeneration->generateEqual = &TargetGenerator::generateEqual;
  targetGeneration->generateNotEqual = &TargetGenerator::generateNotEqual;
  targetGeneration->generateGreaterThan = &TargetGenerator::generateGreaterThan;
  targetGeneration->generateGreaterThanOrEqual = &TargetGenerator::generateGreaterThanOrEqual;
  targetGeneration->generateLessThan = &TargetGenerator::generateLessThan;
  targetGeneration->generateLessThanOrEqual = &TargetGenerator::generateLessThanOrEqual;

  // Literal Generation Functions
  targetGeneration->generateIntLiteral = &TargetGenerator::generateIntLiteral;
  targetGeneration->generateFloatLiteral = &TargetGenerator::generateFloatLiteral;
  targetGeneration->generateStringLiteral = &TargetGenerator::generateStringLiteral;
  targetGeneration->generateNullPtrLiteral = &TargetGenerator::generateNullPtrLiteral;
  targetGeneration->generateStructLiteral = &TargetGenerator::generateStructLiteral;
  targetGeneration->generateArrayLiteral = &TargetGenerator::generateArrayLiteral;
  targetGeneration->generatePointerLiteral = &TargetGenerator::generatePointerLiteral;
}


//==============================================================================
// Main Operation Functions

void TargetGenerator::setupBuild()
{
  this->blockIndex = 0;
  this->anonymousVarIndex = 0;
  this->nullaryProcedureType.reset();
  this->vaStartEndFnType = 0;

  this->buildTarget->setupBuild();
}


//==============================================================================
// Type Generation Functions

Bool TargetGenerator::generateVoidType(TioSharedPtr &type)
{
  auto llvmType = llvm::Type::getVoidTy(*this->buildTarget->getLlvmContext());
  type = newSrdObj<VoidType>(llvmType);
  return true;
}


Bool TargetGenerator::generateIntType(Word bitCount, Bool withSign, TioSharedPtr &type)
{
  if (bitCount == 0) bitCount = this->buildTarget->getPointerBitCount();
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    this->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidIntegerBitCountNotice>());
    return false;
  }
  auto llvmType = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), bitCount);
  type = newSrdObj<IntegerType>(llvmType, bitCount, withSign);
  return true;
}


Bool TargetGenerator::generateFloatType(Word bitCount, TioSharedPtr &type)
{
  llvm::Type *llvmType;
  switch (bitCount) {
    case 32:
      llvmType = llvm::Type::getFloatTy(*this->buildTarget->getLlvmContext());
      break;
    case 64:
      llvmType = llvm::Type::getDoubleTy(*this->buildTarget->getLlvmContext());
      break;
    // TODO: Support 128 bits?
    // case 128:
    //   llvmType = llvm::Type::getFP128Ty(*this->buildTarget->getLlvmContext());
    //   break;
    default:
      this->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidFloatBitCountNotice>());
      return false;
  }
  type = newSrdObj<FloatType>(llvmType, bitCount);
  return true;
}


Bool TargetGenerator::generatePointerType(TiObject *contentType, TioSharedPtr &type)
{
  PREPARE_ARG(contentType, contentTypeWrapper, Type);
  auto llvmType = contentTypeWrapper->getLlvmType()->getPointerTo();
  type = newSrdObj<PointerType>(llvmType, getSharedPtr(contentTypeWrapper));
  return true;
}


Bool TargetGenerator::generateArrayType(TiObject *contentType, Word size, TioSharedPtr &type)
{
  PREPARE_ARG(contentType, contentTypeWrapper, Type);
  auto llvmType = llvm::ArrayType::get(contentTypeWrapper->getLlvmType(), size);
  type = newSrdObj<ArrayType>(llvmType, getSharedPtr(contentTypeWrapper), size);
  return true;
}


Bool TargetGenerator::generateStructTypeDecl(
  Char const *name, TioSharedPtr &type
) {
  auto llvmType = llvm::StructType::create(*this->buildTarget->getLlvmContext(), name);
  type = newSrdObj<StructType>(llvmType, name);
  return true;
}


Bool TargetGenerator::generateStructTypeBody(
  TiObject *type, MapContaining<TiObject> *membersTypes,
  SharedList<TiObject> *members
) {
  VALIDATE_NOT_NULL(membersTypes, members);
  PREPARE_ARG(type, tgType, StructType);

  std::vector<llvm::Type*> structMembers;
  structMembers.reserve(membersTypes->getElementCount());
  for (Int i = 0; i < membersTypes->getElementCount(); ++i) {
    auto contentTypeWrapper = ti_cast<Type>(membersTypes->getElement(i));
    if (contentTypeWrapper == 0) {
      throw EXCEPTION(
        InvalidArgumentException, S("membersTypes"), S("Not all elements are instances of LlvmCodeGen::Type")
      );
    }
    structMembers.push_back(contentTypeWrapper->getLlvmType());

    SharedPtr<Variable> var = newSrdObj<Variable>();
    var->setLlvmStructIndex(i);
    members->add(var);
  }
  static_cast<llvm::StructType*>(tgType->getLlvmType())->setBody(structMembers);
  return true;
}


Word TargetGenerator::getTypeAllocationSize(TiObject *type)
{
  PREPARE_ARG(type, tgType, Type);
  return this->buildTarget->getLlvmDataLayout()->getTypeAllocSize(tgType->getLlvmType());
}


TiObject* TargetGenerator::getNullaryProcedureType()
{
  if (this->nullaryProcedureType == 0) {
    TioSharedPtr tgVoidType;
    if (!this->generateVoidType(tgVoidType)) {
      throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
    }
    SharedMap<TiObject> argTypes;
    if (!this->generateFunctionType(&argTypes, tgVoidType.get(), false, this->nullaryProcedureType)) {
      throw EXCEPTION(GenericException, S("Failed to generate function type for root scope execution."));
    }
  }
  return this->nullaryProcedureType.get();
}


llvm::FunctionType* TargetGenerator::getVaStartEndFnType()
{
  if (this->vaStartEndFnType == 0) {
    auto int8Type = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), 8);
    auto int8PtrType = int8Type->getPointerTo();

    // Prepare args and ret type.
    std::vector<llvm::Type*> argTypes;
    argTypes.push_back(int8PtrType);
    auto retType = llvm::Type::getVoidTy(*this->buildTarget->getLlvmContext());

    // Create the function.
    this->vaStartEndFnType = llvm::FunctionType::get(retType, argTypes, false);
  }
  return this->vaStartEndFnType;
}


//==============================================================================
// Function Generation Functions

Bool TargetGenerator::generateFunctionType(
  MapContaining<TiObject>* argTypes, TiObject *retType, Bool variadic, TioSharedPtr &functionType
) {
  VALIDATE_NOT_NULL(argTypes, retType);

  // Prepare args.
  auto args = SharedMap<Type>::create({});
  std::vector<llvm::Type*> llvmArgTypes;
  llvmArgTypes.reserve(argTypes->getElementCount());
  for (Int i = 0; i < argTypes->getElementCount(); ++i) {
    auto contentTypeWrapper = ti_cast<Type>(argTypes->getElement(i));
    if (contentTypeWrapper == 0) {
      throw EXCEPTION(
        InvalidArgumentException, S("argTypes"), S("Not all elements are instances of LlvmCodeGen::Type")
      );
    }
    args->add(argTypes->getElementKey(i), getSharedPtr(contentTypeWrapper));
    llvmArgTypes.push_back(contentTypeWrapper->getLlvmType());
  }

  // Prepare ret type.
  auto retTypeWrapper = ti_cast<Type>(retType);
  if (retTypeWrapper == 0) {
    throw EXCEPTION(
      InvalidArgumentException, S("retType"), S("Not an instance of LlvmCodeGen::Type")
    );
  }

  // Create the function.
  auto llvmFuncType = llvm::FunctionType::get(retTypeWrapper->getLlvmType(), llvmArgTypes, variadic);
  functionType = newSrdObj<FunctionType>(llvmFuncType, args, getSharedPtr(retTypeWrapper), variadic);
  return true;
}


Bool TargetGenerator::generateFunctionDecl(Char const *name, TiObject *functionType, TioSharedPtr &function)
{
  VALIDATE_NOT_NULL(name, functionType);

  PREPARE_ARG(functionType, funcTypeWrapper, FunctionType);
  auto llvmFuncType = funcTypeWrapper->getLlvmFunctionType();

  // Create the function.
  llvm::Function *llvmFunc = 0;
  if (!this->perFunctionModules) {
    llvmFunc = llvm::Function::Create(
      llvmFuncType, llvm::Function::ExternalLinkage, name, this->buildTarget->getGlobalLlvmModule()
    );
  }
  function = newSrdObj<Function>(name, funcTypeWrapper, llvmFunc);
  return true;
}


Bool TargetGenerator::prepareFunctionBody(
  TiObject *function, TiObject *functionType, SharedList<TiObject> *args, TioSharedPtr &context
) {
  VALIDATE_NOT_NULL(function, args);
  PREPARE_ARG(function, funcWrapper, Function);

  llvm::Module *llvmModule;
  llvm::Function *llvmFunc;
  if (this->perFunctionModules) {
    funcWrapper->llvmModule = std::make_unique<llvm::Module>("function_module", *this->buildTarget->getLlvmContext());
    funcWrapper->llvmModule->setDataLayout(*this->buildTarget->getLlvmDataLayout());

    llvmFunc = llvm::Function::Create(
      funcWrapper->getFunctionType()->getLlvmFunctionType(), llvm::Function::ExternalLinkage,
      funcWrapper->getName().getBuf(), funcWrapper->llvmModule.get()
    );
    funcWrapper->setLlvmFunction(llvmFunc);
    llvmModule = funcWrapper->llvmModule.get();
  } else {
    llvmFunc = funcWrapper->getLlvmFunction();
    llvmModule = this->buildTarget->getGlobalLlvmModule();
  }

  // Create the block
  auto block = newSrdObj<Block>();
  block->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), llvmFunc
  ));
  block->setIrBuilder(new llvm::IRBuilder<>(block->getLlvmBlock()));
  block->setFunction(funcWrapper);

  // Prepare the function arguments.
  PREPARE_ARG(functionType, funcTypeWrapper, FunctionType);
  auto argTypes = funcTypeWrapper->getArgs();
  auto i = 0;
  for (auto iter = llvmFunc->arg_begin(); i != argTypes->getElementCount(); ++iter, ++i) {
    iter->setName(argTypes->getElementKey(i).getBuf());
    args->add(newSrdObj<Value>(iter, false));
  }

  // Is this a variadic funciton?
  if (funcWrapper->getFunctionType()->isVariadic()) {
    // Declare va_list var.
    funcWrapper->llvmVaList = block->getIrBuilder()->CreateAlloca(this->buildTarget->getVaListType(), 0, "__vaList");
    // Add declaration for llvm.va_start function.
    llvm::Function *llvmVaStartFunc = llvmModule->getFunction("llvm.va_start");
    if (llvmVaStartFunc == 0) {
      // This function is in a different module, so we'll have to define it.
      llvmVaStartFunc = llvm::Function::Create(
        this->getVaStartEndFnType(), llvm::Function::ExternalLinkage, "llvm.va_start", llvmModule
      );
    }
    // Call llvm.va_start.
    auto int8Type = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), 8);
    auto int8PtrType = int8Type->getPointerTo();
    std::vector<llvm::Value*> vaArgs;
    vaArgs.push_back(block->getIrBuilder()->CreateBitCast(funcWrapper->llvmVaList, int8PtrType));
    block->getIrBuilder()->CreateCall(llvmVaStartFunc, vaArgs);
    // Attach the variable to the var list.
    SharedPtr<Variable> arg = newSrdObj<Variable>();
    arg->setLlvmAllocaInst(funcWrapper->llvmVaList);
    args->add(arg);
  }

  context = block;
  return true;
}


Bool TargetGenerator::finishFunctionBody(
  TiObject *function, TiObject *functionType, DynamicContaining<TiObject> *args, TiObject *context
) {
  PREPARE_ARG(functionType, functionTypeWrapper, FunctionType);
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(function, funcWrapper, Function);

  auto voidRetType = functionTypeWrapper->getRetType().ti_cast_get<VoidType>();
  if (!block->isTerminated() && voidRetType != 0) {
    if (!this->generateReturn(context, 0, 0)) return false;
  }

  if (this->perFunctionModules) {
    LOG(
      Spp::LogLevel::LLVMCODEGEN_IR, S("Adding function module to build target: ") << funcWrapper->getName()
    );
    this->buildTarget->addLlvmModule(std::move(funcWrapper->llvmModule));
  }

  return true;
}


Bool TargetGenerator::deleteFunction(TiObject *function)
{
  PREPARE_ARG(function, funcWrapper, Function);
  funcWrapper->getLlvmFunction()->eraseFromParent();
  return true;
}


//==============================================================================
// Variable Definition Generation Functions

Bool TargetGenerator::generateGlobalVariable(
  TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
) {
  PREPARE_ARG(type, typeWrapper, Type);
  auto valWrapper = ti_cast<Value>(defaultValue);

  SharedPtr<Variable> var = newSrdObj<Variable>();
  var->setName(name);
  var->setType(typeWrapper);
  var->setLlvmGlobalVariable(new llvm::GlobalVariable(
    *(this->buildTarget->getGlobalLlvmModule()), typeWrapper->getLlvmType(), false,
    llvm::GlobalVariable::ExternalLinkage, valWrapper != 0 ? valWrapper->getLlvmConstant() : 0, name
  ));
  #if __APPLE__
    // TODO: This alignment should depend on build target rather than current OS.
    var->getLlvmGlobalVariable()->setAlignment(llvm::MaybeAlign(16));
  #endif

  result = var;
  return true;
}


Bool TargetGenerator::generateLocalVariable(
  TiObject *context, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(type, typeWrapper, Type);
  auto valWrapper = ti_cast<Value>(defaultValue);

  SharedPtr<Variable> var = newSrdObj<Variable>();
  var->setLlvmAllocaInst(block->getIrBuilder()->CreateAlloca(typeWrapper->getLlvmType(), 0, name));
  #if __APPLE__
    // TODO: This alignment should depend on build target rather than current OS.
    var->getLlvmAllocaInst()->setAlignment(llvm::MaybeAlign(16));
  #endif
  if (valWrapper != 0) {
    block->getIrBuilder()->CreateStore(valWrapper->getLlvmValue(), var->getLlvmAllocaInst());
  }

  result = var;
  return true;
}


//==============================================================================
// Statements Generation Functions

Bool TargetGenerator::prepareIfStatement(TiObject *context, Bool withElse, SharedPtr<CodeGen::IfTgContext> &ifTgContext)
{
  PREPARE_ARG(context, block, Block);

  SharedPtr<IfContext> ifContext = newSrdObj<IfContext>();

  // Prepare condition context.
  ifContext->setConditionBlock(getSharedPtr(block));

  // Prepare body context.
  auto bodyBlock = newSrdObj<Block>();
  bodyBlock->setLlvmBlock(
    llvm::BasicBlock::Create(
      *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
    )
  );
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setFunction(block->getFunction());
  ifContext->setBodyBlock(bodyBlock);

  // Prepare else context.
  if (withElse) {
    auto elseBlock = newSrdObj<Block>();
    elseBlock->setLlvmBlock(llvm::BasicBlock::Create(
      *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
    ));
    elseBlock->setIrBuilder(new llvm::IRBuilder<>(elseBlock->getLlvmBlock()));
    elseBlock->setFunction(block->getFunction());
    ifContext->setElseBlock(elseBlock);
  }

  ifTgContext = ifContext;
  return true;
}


Bool TargetGenerator::finishIfStatement(TiObject *context, CodeGen::IfTgContext *ifTgContext, TiObject *conditionVal)
 {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(ifTgContext, ifContext, IfContext);
  PREPARE_ARG(conditionVal, valWrapper, Value);

  // Create a merge block and jump to it from if and else bodies.
  llvm::BasicBlock *mergeLlvmBlock = 0;
  if (
    !ifContext->getBodyBlock()->isTerminated() ||
    ifContext->getElseBlock() == 0 ||
    !ifContext->getElseBlock()->isTerminated()
  ) {
    mergeLlvmBlock = llvm::BasicBlock::Create(
      *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
    );
    if (!ifContext->getBodyBlock()->isTerminated()) {
      ifContext->getBodyBlock()->getIrBuilder()->CreateBr(mergeLlvmBlock);
    }
    if (ifContext->getElseBlock() != 0 && !ifContext->getElseBlock()->isTerminated()) {
      ifContext->getElseBlock()->getIrBuilder()->CreateBr(mergeLlvmBlock);
    }
  }

  // Create the if statement.
  block->getIrBuilder()->CreateCondBr(
    valWrapper->getLlvmValue(),
    ifContext->getBodyBlock()->getLlvmEntryBlock(),
    ifContext->getElseBlock() != 0 ? ifContext->getElseBlock()->getLlvmEntryBlock() : mergeLlvmBlock
  );

  // Set insert point to the merge body.
  if (mergeLlvmBlock != 0) {
    block->getIrBuilder()->SetInsertPoint(mergeLlvmBlock);
    block->setLlvmBlock(mergeLlvmBlock);
  } else {
    block->setTerminated(true);
  }

  return true;
}


Bool TargetGenerator::prepareWhileStatement(TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext)
{
  PREPARE_ARG(context, block, Block);

  SharedPtr<LoopContext> loopContext = newSrdObj<LoopContext>();

  // Prepare condition context.
  auto condBlock = newSrdObj<Block>();
  condBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  condBlock->setIrBuilder(new llvm::IRBuilder<>(condBlock->getLlvmBlock()));
  condBlock->setFunction(block->getFunction());
  loopContext->setConditionBlock(condBlock);

  // Prepare body context.
  auto bodyBlock = newSrdObj<Block>();
  bodyBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setFunction(block->getFunction());
  loopContext->setBodyBlock(bodyBlock);

  // Prepare exit block.
  auto exitBlock = newSrdObj<Block>();
  exitBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  exitBlock->setFunction(block->getFunction());
  loopContext->setExitBlock(exitBlock);

  loopTgContext = loopContext;
  return true;
}


Bool TargetGenerator::finishWhileStatement(
  TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(loopTgContext, loopContext, LoopContext);
  PREPARE_ARG(conditionVal, valWrapper, Value);

  // Jump to condition block.
  block->getIrBuilder()->CreateBr(loopContext->getConditionBlock()->getLlvmEntryBlock());

  // Jump from body to condition block.
  if (!loopContext->getBodyBlock()->isTerminated()) {
    loopContext->getBodyBlock()->getIrBuilder()->CreateBr(loopContext->getConditionBlock()->getLlvmEntryBlock());
  }

  // Create condition branch.
  loopContext->getConditionBlock()->getIrBuilder()->CreateCondBr(
    valWrapper->getLlvmValue(), loopContext->getBodyBlock()->getLlvmEntryBlock(),
    loopContext->getExitBlock()->getLlvmEntryBlock()
  );

  // Set insert point.
  block->getIrBuilder()->SetInsertPoint(loopContext->getExitBlock()->getLlvmBlock());
  block->setLlvmBlock(loopContext->getExitBlock()->getLlvmBlock());

  return true;
}


Bool TargetGenerator::prepareForStatement(TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext)
{
  PREPARE_ARG(context, block, Block);

  SharedPtr<LoopContext> loopContext = newSrdObj<LoopContext>();

  // Prepare condition context.
  auto condBlock = newSrdObj<Block>();
  condBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  condBlock->setIrBuilder(new llvm::IRBuilder<>(condBlock->getLlvmBlock()));
  condBlock->setFunction(block->getFunction());
  loopContext->setConditionBlock(condBlock);

  // Prepare increment context.
  auto updaterBlock = newSrdObj<Block>();
  updaterBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  updaterBlock->setIrBuilder(new llvm::IRBuilder<>(updaterBlock->getLlvmBlock()));
  updaterBlock->setFunction(block->getFunction());
  loopContext->setUpdaterBlock(updaterBlock);

  // Prepare body context.
  auto bodyBlock = newSrdObj<Block>();
  bodyBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setFunction(block->getFunction());
  loopContext->setBodyBlock(bodyBlock);

  // Prepare exit block.
  auto exitBlock = newSrdObj<Block>();
  exitBlock->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  exitBlock->setFunction(block->getFunction());
  loopContext->setExitBlock(exitBlock);

  loopTgContext = loopContext;
  return true;
}


Bool TargetGenerator::finishForStatement(
  TiObject *context, CodeGen::LoopTgContext *loopTgContext, TiObject *conditionVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(loopTgContext, loopContext, LoopContext);
  PREPARE_ARG(conditionVal, valWrapper, Value);

  // Jump to condition block.
  block->getIrBuilder()->CreateBr(loopContext->getConditionBlock()->getLlvmEntryBlock());

  // Jump from body to update block.
  if (!loopContext->getBodyBlock()->isTerminated()) {
    loopContext->getBodyBlock()->getIrBuilder()->CreateBr(loopContext->getUpdaterBlock()->getLlvmEntryBlock());
  }

  // Jump from update to condition block.
  loopContext->getUpdaterBlock()->getIrBuilder()->CreateBr(loopContext->getConditionBlock()->getLlvmEntryBlock());

  // Create condition branch.
  loopContext->getConditionBlock()->getIrBuilder()->CreateCondBr(
    valWrapper->getLlvmValue(), loopContext->getBodyBlock()->getLlvmEntryBlock(),
    loopContext->getExitBlock()->getLlvmEntryBlock()
  );

  // Set insert point.
  block->getIrBuilder()->SetInsertPoint(loopContext->getExitBlock()->getLlvmBlock());
  block->setLlvmBlock(loopContext->getExitBlock()->getLlvmBlock());

  return true;
}


Bool TargetGenerator::generateContinue(TiObject *context, CodeGen::LoopTgContext *loopTgContext)
{
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(loopTgContext, loopContext, LoopContext);

  if (loopContext->getUpdaterBlock() != 0) {
    block->getIrBuilder()->CreateBr(loopContext->getUpdaterBlock()->getLlvmEntryBlock());
  } else {
    block->getIrBuilder()->CreateBr(loopContext->getConditionBlock()->getLlvmEntryBlock());
  }
  block->setTerminated(true);
  return true;
}


Bool TargetGenerator::generateBreak(TiObject *context, CodeGen::LoopTgContext *loopTgContext)
{
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(loopTgContext, loopContext, LoopContext);

  block->getIrBuilder()->CreateBr(loopContext->getExitBlock()->getLlvmEntryBlock());
  block->setTerminated(true);
  return true;
}


//==============================================================================
// Casting Generation Functions

Bool TargetGenerator::generateCastIntToInt(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(srcType, srcTypeWrapper, IntegerType);
  PREPARE_ARG(destType, destTypeWrapper, IntegerType);
  auto llvmCastedValue = block->getIrBuilder()->CreateIntCast(
    cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType(), srcTypeWrapper->isSigned()
  );
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastIntToFloat(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(srcType, srcTypeWrapper, IntegerType);
  PREPARE_ARG(destType, destTypeWrapper, Type);
  llvm::Value *llvmCastedValue;
  if (srcTypeWrapper->isSigned()) {
    llvmCastedValue = block->getIrBuilder()->CreateSIToFP(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  } else {
    llvmCastedValue = block->getIrBuilder()->CreateUIToFP(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  }
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastFloatToInt(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, destTypeWrapper, IntegerType);
  llvm::Value *llvmCastedValue;
  if (destTypeWrapper->isSigned()) {
    llvmCastedValue = block->getIrBuilder()->CreateFPToSI(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  } else {
    llvmCastedValue = block->getIrBuilder()->CreateFPToUI(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  }
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastFloatToFloat(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(srcType, srcTypeWrapper, FloatType);
  PREPARE_ARG(destType, destTypeWrapper, FloatType);

  llvm::Value *llvmCastedValue;
  if (srcTypeWrapper->getSize() > destTypeWrapper->getSize()) {
    llvmCastedValue = block->getIrBuilder()->CreateFPTrunc(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  } else {
    llvmCastedValue = block->getIrBuilder()->CreateFPExt(cgSrcVal->getLlvmValue(), destTypeWrapper->getLlvmType());
  }
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastIntToPointer(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreateIntToPtr(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastPointerToInt(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreatePtrToInt(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastPointerToPointer(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreateBitCast(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = newSrdObj<Value>(llvmCastedValue, false);
  return true;
}


//==============================================================================
// Operation Generation Functions

Bool TargetGenerator::generateVarReference(
  TiObject *context, TiObject *varType, TiObject *varDefinition, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  auto var = ti_cast<Variable>(varDefinition);
  auto value = ti_cast<Value>(varDefinition);
  if (var == 0 && value == 0) {
    throw EXCEPTION(InvalidArgumentException, S("varDefinition"), S("Argument is null or of invalid type"));
  }

  if (var != 0) {
    if (var->getLlvmAllocaInst() != 0) {
      result = newSrdObj<Value>(var->getLlvmAllocaInst(), false);
    } else if (var->getLlvmGlobalVariable() != 0) {
      llvm::Module *llvmMod = this->perFunctionModules ?
        block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
      // Make sure the target var is declared in the current module.
      llvm::GlobalVariable *llvmVar = llvmMod->getGlobalVariable(var->getName().getBuf());
      if (llvmVar == 0) {
        // This global var is in a different module, so we'll have to define it again.
        llvmVar = new llvm::GlobalVariable(
          *llvmMod, var->getType()->getLlvmType(), false, llvm::GlobalVariable::ExternalLinkage,
          0, var->getName().getBuf()
        );
      }
      // Prepare the reference.
      result = newSrdObj<Value>(llvmVar, false);
    } else {
      throw EXCEPTION(GenericException, S("Variable definition was not generated correctly."));
    }
  } else {
    result = getSharedPtr(value);
  }
  return true;
}


Bool TargetGenerator::generateMemberVarReference(
  TiObject *context, TiObject *structType, TiObject *memberType,
  TiObject *memberVarDef, TiObject *structRef, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(structRef, tgStructRef, Value);
  PREPARE_ARG(memberVarDef, tgMemberVarDef, Variable);
  PREPARE_ARG(structType, tgStructType, Type);

  llvm::Value *llvmPtr;
  if (tgStructType->isDerivedFrom<PointerType>()) {
    llvmPtr = tgStructRef->getLlvmValue();
  } else {
    llvmPtr = block->getIrBuilder()->CreateAlloca(tgStructType->getLlvmType(), 0, "");
    block->getIrBuilder()->CreateStore(tgStructRef->getLlvmValue(), llvmPtr);
  }

  auto zero = llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(32, 0, true));
  auto index = llvm::ConstantInt::get(
    *this->buildTarget->getLlvmContext(), llvm::APInt(32, tgMemberVarDef->getLlvmStructIndex(), true)
  );
  auto llvmResult = block->getIrBuilder()->CreateGEP(
    llvmPtr, llvm::makeArrayRef(std::vector<llvm::Value*>({ zero, index })), ""
  );
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateArrayElementReference(
  TiObject *context, TiObject *arrayType, TiObject *elementType, TiObject *index, TiObject *arrayRef,
  TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(arrayRef, tgArrayRef, Value);
  PREPARE_ARG(arrayType, tgArrayType, Type);
  PREPARE_ARG(index, tgIndex, Value);

  llvm::Value *llvmPtr;
  if (tgArrayType->isDerivedFrom<PointerType>()) {
    llvmPtr = tgArrayRef->getLlvmValue();
  } else {
    llvmPtr = block->getIrBuilder()->CreateAlloca(tgArrayType->getLlvmType(), 0, "");
    block->getIrBuilder()->CreateStore(tgArrayRef->getLlvmValue(), llvmPtr);
  }

  auto zero = llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(32, 0, true));
  auto llvmResult = block->getIrBuilder()->CreateGEP(
    llvmPtr, llvm::makeArrayRef(std::vector<llvm::Value*>({ zero, tgIndex->getLlvmValue() })), ""
  );
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateDereference(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  auto llvmResult = block->getIrBuilder()->CreateLoad(cgSrcVal->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateAssign(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destRef, cgDestRef, Value);
  block->getIrBuilder()->CreateStore(cgSrcVal->getLlvmValue(), cgDestRef->getLlvmValue());
  result = getSharedPtr(destRef);
  return true;
}


Bool TargetGenerator::generateFunctionPointer(
  TiObject *context, TiObject *function, TiObject *functionPtrType, TioSharedPtr &result
) {
  PREPARE_ARG(functionPtrType, functionPtrTypeWrapper, PointerType);
  PREPARE_ARG(function, funcWrapper, Function);
  PREPARE_ARG(context, block, Block);

  // // Make sure the target function is declared in the current module.
  llvm::Module *llvmMod = this->perFunctionModules ?
    block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
  llvm::Function *llvmFunc = llvmMod->getFunction(funcWrapper->getName().getBuf());
  if (llvmFunc == 0) {
    // This function is in a different module, so we'll have to define it.
    llvmFunc = llvm::Function::Create(
      funcWrapper->getFunctionType()->getLlvmFunctionType(), llvm::Function::ExternalLinkage,
      funcWrapper->getName().getBuf(), llvmMod
    );
  }

  // Generate the func pointer.
  auto llvmResult = llvm::ConstantExpr::getBitCast(
    llvmFunc, functionPtrTypeWrapper->getLlvmType()
  );
  result = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateFunctionCall(
  TiObject *context, TiObject *function,
  Containing<TiObject>* arguments, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(function, funcWrapper, Function);

  Bool variadic = funcWrapper->getFunctionType()->isVariadic();
  auto argDefCount = funcWrapper->getFunctionType()->getArgs() != 0 ?
    funcWrapper->getFunctionType()->getArgs()->getCount() : 0;

  // Prepare function args.
  std::vector<llvm::Value*> args;
  for (Int i = 0; i < arguments->getElementCount(); ++i) {
    auto llvmValBox = ti_cast<Value>(arguments->getElement(i));
    if (llvmValBox == 0) {
      throw EXCEPTION(InvalidArgumentException, S("arguments"), S("Some elements are null or of invalid type."));
    }

    auto llvmValue = llvmValBox->getLlvmValue();

    if (variadic && i >= argDefCount && llvmValue->getType()->isStructTy()) {
      // Convert struct types to pointers.
      llvmValue = block->getIrBuilder()->CreateAlloca(llvmValue->getType(), 0, "");
      block->getIrBuilder()->CreateStore(llvmValBox->getLlvmValue(), llvmValue);
    }

    args.push_back(llvmValue);
  }
  // Make sure a declaration of this function exists in the current module.
  llvm::Module *llvmMod = this->perFunctionModules ?
    block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
  llvm::Function *llvmFunc = llvmMod->getFunction(funcWrapper->getName().getBuf());
  if (llvmFunc == 0) {
    // This function is in a different module, so we'll have to define it.
    llvmFunc = llvm::Function::Create(
      funcWrapper->getFunctionType()->getLlvmFunctionType(), llvm::Function::ExternalLinkage,
      funcWrapper->getName().getBuf(), llvmMod
    );
  }
  // Create the call.
  auto llvmCall = block->getIrBuilder()->CreateCall(llvmFunc, args);
  result = newSrdObj<Value>(llvmCall, false);
  return true;
}


Bool TargetGenerator::generateFunctionPtrCall(
  TiObject *context, TiObject *functionPtr, TiObject *functionPtrType,
  Containing<TiObject>* arguments, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(functionPtr, llvmFuncPtrBox, Value);
  PREPARE_ARG(functionPtrType, llvmFuncPtrTypeBox, PointerType);

  // TODO: Validate provided args against functionptrType.

  auto llvmFuncTypeBox = llvmFuncPtrTypeBox->getContentType().ti_cast_get<FunctionType>();
  if (llvmFuncTypeBox == 0) {
    throw EXCEPTION(InvalidArgumentException, S("functionPtrType"), S("Argument is not a function pointer type."));
  }
  Bool variadic = llvmFuncTypeBox->isVariadic();
  auto argDefCount = llvmFuncTypeBox->getArgs() != 0 ? llvmFuncTypeBox->getArgs()->getCount() : 0;

  // Create function call.
  std::vector<llvm::Value*> args;
  for (Int i = 0; i < arguments->getElementCount(); ++i) {
    auto llvmValBox = ti_cast<Value>(arguments->getElement(i));
    if (llvmValBox == 0) {
      throw EXCEPTION(InvalidArgumentException, S("arguments"), S("Some elements are null or of invalid type."));
    }

    auto llvmValue = llvmValBox->getLlvmValue();

    if (variadic && i >= argDefCount && llvmValue->getType()->isStructTy()) {
      // Convert struct types to pointers.
      llvmValue = block->getIrBuilder()->CreateAlloca(llvmValue->getType(), 0, "");
      block->getIrBuilder()->CreateStore(llvmValBox->getLlvmValue(), llvmValue);
    }

    args.push_back(llvmValue);
  }
  auto llvmCall = block->getIrBuilder()->CreateCall(llvmFuncPtrBox->getLlvmValue(), args);
  result = newSrdObj<Value>(llvmCall, false);
  return true;
}


Bool TargetGenerator::generateReturn(
  TiObject *context, TiObject *retType, TiObject *retVal
) {
  PREPARE_ARG(context, block, Block);

  // Is this a variadic function?
  if (block->getFunction()->getFunctionType()->isVariadic()) {
    llvm::Module *llvmModule = this->perFunctionModules ?
      block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
    // Add declaration for llvm.va_end function.
    llvm::Function *llvmVaEndFunc = llvmModule->getFunction("llvm.va_end");
    if (llvmVaEndFunc == 0) {
      // This function is in a different module, so we'll have to define it.
      llvmVaEndFunc = llvm::Function::Create(
        this->getVaStartEndFnType(), llvm::Function::ExternalLinkage, "llvm.va_end", llvmModule
      );
    }
    // Call llvm.va_end.
    auto int8Type = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), 8);
    auto int8PtrType = int8Type->getPointerTo();
    std::vector<llvm::Value*> vaArgs;
    vaArgs.push_back(block->getIrBuilder()->CreateBitCast(block->getFunction()->llvmVaList, int8PtrType));
    block->getIrBuilder()->CreateCall(llvmVaEndFunc, vaArgs);
  }

  if (retVal != 0) {
    PREPARE_ARG(retVal, retValBox, Value);
    block->getIrBuilder()->CreateRet(retValBox->getLlvmValue());
  } else {
    block->getIrBuilder()->CreateRetVoid();
  }
  block->setTerminated(true);
  return true;
}


//==============================================================================
// Logical Ops Generation Functions

Bool TargetGenerator::prepareLogicalOp(TiObject *context, TioSharedPtr &secondContext)
{
  PREPARE_ARG(context, block, Block);

  auto block2 = newSrdObj<Block>();
  block2->setLlvmBlock(llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  ));
  block2->setIrBuilder(new llvm::IRBuilder<>(block2->getLlvmBlock()));
  block2->setFunction(block->getFunction());
  secondContext = block2;

  return true;
}


Bool TargetGenerator::finishLogicalOr(
  TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(secondContext, block2, Block);
  PREPARE_ARG(val1, val1Wrapper, Value);
  PREPARE_ARG(val2, val2Wrapper, Value);

  // Create the merge block and jump to it.
  auto mergeLlvmBlock = llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  );
  block2->getIrBuilder()->CreateBr(mergeLlvmBlock);

  // Create the if statement.
  block->getIrBuilder()->CreateCondBr(val1Wrapper->getLlvmValue(), mergeLlvmBlock, block2->getLlvmEntryBlock());

  // Set insert point to the merge body.
  auto block1 = block->getLlvmBlock();
  block->getIrBuilder()->SetInsertPoint(mergeLlvmBlock);
  block->setLlvmBlock(mergeLlvmBlock);

  // Generate phi value.
  auto boolType = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), 1);
  llvm::PHINode *phi = block->getIrBuilder()->CreatePHI(boolType, 0);
  phi->addIncoming(val1Wrapper->getLlvmValue(), block1);
  phi->addIncoming(val2Wrapper->getLlvmValue(), block2->getLlvmBlock());

  result = newSrdObj<Value>(phi);
  return true;
}


Bool TargetGenerator::finishLogicalAnd(
  TiObject *context, TiObject *secondContext, TiObject *val1, TiObject *val2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(secondContext, block2, Block);
  PREPARE_ARG(val1, val1Wrapper, Value);
  PREPARE_ARG(val2, val2Wrapper, Value);

  // Create the merge block and jump to it.
  auto mergeLlvmBlock = llvm::BasicBlock::Create(
    *this->buildTarget->getLlvmContext(), this->getNewBlockName(), block->getFunction()->getLlvmFunction()
  );
  block2->getIrBuilder()->CreateBr(mergeLlvmBlock);

  // Create the if statement.
  block->getIrBuilder()->CreateCondBr(val1Wrapper->getLlvmValue(), block2->getLlvmEntryBlock(), mergeLlvmBlock);

  // Set insert point to the merge body.
  auto block1 = block->getLlvmBlock();
  block->getIrBuilder()->SetInsertPoint(mergeLlvmBlock);
  block->setLlvmBlock(mergeLlvmBlock);

  // Generate phi value.
  auto boolType = llvm::Type::getIntNTy(*this->buildTarget->getLlvmContext(), 1);
  llvm::PHINode *phi = block->getIrBuilder()->CreatePHI(boolType, 0);
  phi->addIncoming(val1Wrapper->getLlvmValue(), block1);
  phi->addIncoming(val2Wrapper->getLlvmValue(), block2->getLlvmBlock());

  result = newSrdObj<Value>(phi);
  return true;
}


//==============================================================================
// Math Ops Generation Functions

Bool TargetGenerator::generateAdd(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<PointerType>()) {
    auto llvmResult = block->getIrBuilder()->CreateGEP(
      srcVal1Box->getLlvmValue(),
      llvm::makeArrayRef(std::vector<llvm::Value*>({ srcVal2Box->getLlvmValue() })), ""
    );
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateSub(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<PointerType>()) {
    auto negIndex = block->getIrBuilder()->CreateNeg(srcVal2Box->getLlvmValue());
    auto llvmResult = block->getIrBuilder()->CreateGEP(
      srcVal1Box->getLlvmValue(),
      llvm::makeArrayRef(std::vector<llvm::Value*>({ negIndex })), ""
    );
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateMul(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateDiv(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateSDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateUDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateRem(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateSRem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateURem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFRem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateShr(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  llvm::Value *llvmResult;
  if (tgType->isSigned()) {
    llvmResult = block->getIrBuilder()->CreateAShr(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  } else {
    llvmResult = block->getIrBuilder()->CreateLShr(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  }
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateShl(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateShl(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateAnd(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateAnd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateOr(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateOr(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateXor(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateXor(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateNot(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateNot(srcValBox->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateNeg(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto llvmResult = block->getIrBuilder()->CreateNeg(srcValBox->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFNeg(srcValBox->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateEarlyInc(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    if (integerType->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWAdd(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateEarlyDec(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    if (integerType->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWSub(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateLateInc(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    if (integerType->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWAdd(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = newSrdObj<Value>(llvmVal, false);
  return true;
}


Bool TargetGenerator::generateLateDec(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    if (integerType->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWSub(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(
        llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = newSrdObj<Value>(llvmVal, false);
  return true;
}


Bool TargetGenerator::generateShrAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isSigned()) {
    llvmResult = block->getIrBuilder()->CreateAShr(llvmVal, srcValBox->getLlvmValue());
  } else {
    llvmResult = block->getIrBuilder()->CreateLShr(llvmVal, srcValBox->getLlvmValue());
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateShlAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  llvmResult = block->getIrBuilder()->CreateShl(llvmVal, srcValBox->getLlvmValue());
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateAndAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  llvmResult = block->getIrBuilder()->CreateAnd(llvmVal, srcValBox->getLlvmValue());
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateOrAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  llvmResult = block->getIrBuilder()->CreateOr(llvmVal, srcValBox->getLlvmValue());
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateXorAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  llvmResult = block->getIrBuilder()->CreateXor(llvmVal, srcValBox->getLlvmValue());
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateNextArg(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  llvm::Value *llvmResult;
  if (tgType->getLlvmType()->isStructTy()) {
    auto llvmPtrType = tgType->getLlvmType()->getPointerTo();
    auto llvmPtr = block->getIrBuilder()->CreateVAArg(srcValBox->getLlvmValue(), llvmPtrType);
    llvmResult = block->getIrBuilder()->CreateLoad(llvmPtr);
  } else {
    llvmResult = block->getIrBuilder()->CreateVAArg(srcValBox->getLlvmValue(), tgType->getLlvmType());
  }

  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


//==============================================================================
// Comparison Ops Generation Functions

Bool TargetGenerator::generateEqual(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>() || tgType->isDerivedFrom<PointerType>()) {
    auto llvmResult = block->getIrBuilder()->CreateICmpEQ(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOEQ(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateNotEqual(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>() || tgType->isDerivedFrom<PointerType>()) {
    auto llvmResult = block->getIrBuilder()->CreateICmpNE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpONE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateGreaterThan(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateICmpSGT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateICmpUGT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOGT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateGreaterThanOrEqual(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateICmpSGE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateICmpUGE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOGE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateLessThan(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateICmpSLT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateICmpULT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOLT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


Bool TargetGenerator::generateLessThanOrEqual(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcValBox1, Value);
  PREPARE_ARG(srcVal2, srcValBox2, Value);
  PREPARE_ARG(type, tgType, Type);
  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateICmpSLE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateICmpULE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOLE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
}


//==============================================================================
// Literal Generation Functions

Bool TargetGenerator::generateIntLiteral(
  TiObject *context, Word bitCount, Bool withSign, LongInt value, TioSharedPtr &destVal
) {
  if (bitCount == 0) bitCount = this->buildTarget->getPointerBitCount();
  auto llvmResult = llvm::ConstantInt::get(
    *this->buildTarget->getLlvmContext(), llvm::APInt(bitCount, value, withSign)
  );
  destVal = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateFloatLiteral(
  TiObject *context, Word bitCount, Double value, TioSharedPtr &destVal
) {
  llvm::Value *llvmResult;
  if (bitCount == 32) {
    llvmResult = llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)value));
  } else {
    llvmResult = llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat(value));
  }
  destVal = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateStringLiteral(
  TiObject *context, Char const* value, TiObject *charType, TiObject *strType, TioSharedPtr &destVal
) {
  PREPARE_ARG(charType, cgCharType, Type);
  PREPARE_ARG(strType, cgStrType, Type);
  PREPARE_ARG(context, block, Block);

  Word len = getStrLen(value);

  // Prepare the llvm constant array.
  std::vector<llvm::Constant*> llvmCharArray;
  llvmCharArray.reserve(len + 1);
  for (Word i = 0; i < len; i++) {
    llvmCharArray.push_back(llvm::ConstantInt::get(cgCharType->getLlvmType(), value[i]));
  }
  llvmCharArray.push_back(llvm::ConstantInt::get(cgCharType->getLlvmType(), 0));

  // Create an anonymous global variable.
  auto llvmStrConst = llvm::ConstantArray::get(static_cast<llvm::ArrayType*>(cgStrType->getLlvmType()), llvmCharArray);
  llvm::Module *llvmMod = this->perFunctionModules ?
    block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
  auto llvmVar = new llvm::GlobalVariable(
    *llvmMod, cgStrType->getLlvmType(), true,
    llvm::GlobalValue::PrivateLinkage, llvmStrConst, this->getAnonymouseVarName().c_str()
  );
  // TODO: Do we need this setAlignment call? It's marked as deprecated in LLVM 10.0.0.
  // llvmVar->setAlignment(1);

  destVal = newSrdObj<Value>(llvmVar, true);
  return true;
}


Bool TargetGenerator::generateNullPtrLiteral(
  TiObject *context, TiObject *type, TioSharedPtr &destVal
) {
  PREPARE_ARG(type, tgType, PointerType);
  auto llvmResult = llvm::ConstantPointerNull::get(static_cast<llvm::PointerType*>(tgType->getLlvmType()));
  destVal = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateStructLiteral(
  TiObject *context, TiObject *type, MapContaining<TiObject> *membersTypes, Containing<TiObject> *membersVals,
  TioSharedPtr &destVal
) {
  PREPARE_ARG(type, tgType, StructType);
  VALIDATE_NOT_NULL(membersVals);
  std::vector<llvm::Constant*> structVals;
  for (Int i = 0; i < membersVals->getElementCount(); ++i) {
    auto value = ti_cast<Value>(membersVals->getElement(i));
    if (value == 0) {
      throw EXCEPTION(GenericException, S("Unexpected member value received."));
    }
    structVals.push_back(value->getLlvmConstant());
  }
  auto llvmResult = llvm::ConstantStruct::get(static_cast<llvm::StructType*>(tgType->getLlvmType()), structVals);
  destVal = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateArrayLiteral(
  TiObject *context, TiObject *type, Containing<TiObject> *membersVals,
  TioSharedPtr &destVal
) {
  PREPARE_ARG(type, tgType, ArrayType);
  VALIDATE_NOT_NULL(membersVals);
  std::vector<llvm::Constant*> arrayVals;
  for (Int i = 0; i < membersVals->getElementCount(); ++i) {
    auto value = ti_cast<Value>(membersVals->getElement(i));
    if (value == 0) {
      throw EXCEPTION(GenericException, S("Unexpected member value received."));
    }
    arrayVals.push_back(value->getLlvmConstant());
  }
  auto llvmResult = llvm::ConstantArray::get(static_cast<llvm::ArrayType*>(tgType->getLlvmType()), arrayVals);
  destVal = newSrdObj<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generatePointerLiteral(TiObject *context, TiObject *type, void *value, TioSharedPtr &destVal)
{
  PREPARE_ARG(type, tgType, PointerType);
  auto llvmInt = llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(64, (uint64_t)value, false));
  auto llvmPtr = llvm::ConstantExpr::getIntToPtr(llvmInt, tgType->getLlvmType());
  destVal = newSrdObj<Value>(llvmPtr, true);
  return true;
}


//==============================================================================
// Helper Functions

std::string TargetGenerator::getNewBlockName()
{
  return std::string("#block") + std::to_string(this->blockIndex++);
}


std::string TargetGenerator::getAnonymouseVarName()
{
  return std::string("#anonymous") + std::to_string(this->anonymousVarIndex++);
}

} // namespace
