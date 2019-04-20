/**
 * @file Spp/LlvmCodeGen/TargetGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::TargetGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace LlvmCodeGen
{

// Static global context
static llvm::LLVMContext myContext;

//==============================================================================
// Initialization Functions

void TargetGenerator::initBindings()
{
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(this);

  // Property Getters
  targetGeneration->getExecutionContext = &TargetGenerator::getExecutionContext;

  // Type Generation Functions
  targetGeneration->generateVoidType = &TargetGenerator::generateVoidType;
  targetGeneration->generateIntType = &TargetGenerator::generateIntType;
  targetGeneration->generateFloatType = &TargetGenerator::generateFloatType;
  targetGeneration->generatePointerType = &TargetGenerator::generatePointerType;
  targetGeneration->generateArrayType = &TargetGenerator::generateArrayType;
  targetGeneration->generateStructTypeDecl = &TargetGenerator::generateStructTypeDecl;
  targetGeneration->generateStructTypeBody = &TargetGenerator::generateStructTypeBody;
  targetGeneration->getTypeAllocationSize = &TargetGenerator::getTypeAllocationSize;

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
  targetGeneration->generateAddAssign = &TargetGenerator::generateAddAssign;
  targetGeneration->generateSubAssign = &TargetGenerator::generateSubAssign;
  targetGeneration->generateMulAssign = &TargetGenerator::generateMulAssign;
  targetGeneration->generateDivAssign = &TargetGenerator::generateDivAssign;
  targetGeneration->generateRemAssign = &TargetGenerator::generateRemAssign;
  targetGeneration->generateShrAssign = &TargetGenerator::generateShrAssign;
  targetGeneration->generateShlAssign = &TargetGenerator::generateShlAssign;
  targetGeneration->generateAndAssign = &TargetGenerator::generateAndAssign;
  targetGeneration->generateOrAssign = &TargetGenerator::generateOrAssign;
  targetGeneration->generateXorAssign = &TargetGenerator::generateXorAssign;

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

void TargetGenerator::prepareBuild()
{
  llvm::InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  this->llvmDataLayout = std::make_shared<llvm::DataLayout>("");
  this->llvmModule = std::make_unique<llvm::Module>("AlususProgram", myContext);
  this->llvmModule->setDataLayout(this->llvmDataLayout->getStringRepresentation());
  this->executionContext = std::make_shared<ExecutionContext>(llvmDataLayout->getPointerSizeInBits());
}


void TargetGenerator::resetBuild()
{
  this->llvmModule = std::make_unique<llvm::Module>("AlususProgram", myContext);
  this->llvmModule->setDataLayout(this->llvmDataLayout->getStringRepresentation());
}


void TargetGenerator::dumpIr(OutStream &out)
{
  if (this->llvmModule == 0) {
    throw EXCEPTION(GenericException, S("LLVM module is not generated yet."));
  }

  llvm::raw_os_ostream ostream(out);
  llvm::createPrintModulePass(ostream)->runOnModule(*(this->llvmModule.get()));
}


void TargetGenerator::execute(Char const *entry)
{
  if (this->llvmModule == 0) {
    throw EXCEPTION(GenericException, S("LLVM module is not generated yet."));
  }

  auto engineBuilder = llvm::EngineBuilder(llvm::CloneModule(*this->llvmModule));
  std::string errorStr;
  engineBuilder.setErrorStr(&errorStr);
  auto ee = engineBuilder.create();

  // Add global variable mappings.
  if (this->globalItemRepo != 0) {
    for (Int i = 0; i < this->globalItemRepo->getItemCount(); ++i) {
      auto varName = this->globalItemRepo->getItemName(i).c_str();
      auto varPtr = (uint64_t)this->globalItemRepo->getItemPtr(i);
      ee->addGlobalMapping(varName, varPtr);
    }
  }

  typedef void (*FuncType)();
  auto funcPtr = (FuncType)ee->getFunctionAddress(entry);

  funcPtr();

  delete ee;
}


//==============================================================================
// Type Generation Functions

Bool TargetGenerator::generateVoidType(TioSharedPtr &type)
{
  auto llvmType = llvm::Type::getVoidTy(myContext);
  type = std::make_shared<VoidType>(llvmType);
  return true;
}


Bool TargetGenerator::generateIntType(Word bitCount, Bool withSign, TioSharedPtr &type)
{
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    this->noticeStore->add(std::make_shared<Spp::Notices::InvalidIntegerBitCountNotice>());
    return false;
  }
  auto llvmType = llvm::Type::getIntNTy(myContext, bitCount);
  type = std::make_shared<IntegerType>(llvmType, bitCount, withSign);
  return true;
}


Bool TargetGenerator::generateFloatType(Word bitCount, TioSharedPtr &type)
{
  llvm::Type *llvmType;
  switch (bitCount) {
    case 32:
      llvmType = llvm::Type::getFloatTy(myContext);
      break;
    case 64:
      llvmType = llvm::Type::getDoubleTy(myContext);
      break;
    // TODO: Support 128 bits?
    // case 128:
    //   llvmType = llvm::Type::getFP128Ty(myContext);
    //   break;
    default:
      this->noticeStore->add(std::make_shared<Spp::Notices::InvalidFloatBitCountNotice>());
      return false;
  }
  type = std::make_shared<FloatType>(llvmType, bitCount);
  return true;
}


Bool TargetGenerator::generatePointerType(TiObject *contentType, TioSharedPtr &type)
{
  PREPARE_ARG(contentType, contentTypeWrapper, Type);
  auto llvmType = contentTypeWrapper->getLlvmType()->getPointerTo();
  type = std::make_shared<PointerType>(llvmType, getSharedPtr(contentTypeWrapper));
  return true;
}


Bool TargetGenerator::generateArrayType(TiObject *contentType, Word size, TioSharedPtr &type)
{
  PREPARE_ARG(contentType, contentTypeWrapper, Type);
  auto llvmType = llvm::ArrayType::get(contentTypeWrapper->getLlvmType(), size);
  type = std::make_shared<ArrayType>(llvmType, getSharedPtr(contentTypeWrapper), size);
  return true;
}


Bool TargetGenerator::generateStructTypeDecl(
  Char const *name, TioSharedPtr &type
) {
  auto llvmType = llvm::StructType::create(myContext, name);
  type = std::make_shared<StructType>(llvmType, name);
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

    SharedPtr<Variable> var = std::make_shared<Variable>();
    var->setLlvmStructIndex(i);
    members->add(var);
  }
  static_cast<llvm::StructType*>(tgType->getLlvmType())->setBody(structMembers);
  return true;
}


Word TargetGenerator::getTypeAllocationSize(TiObject *type)
{
  PREPARE_ARG(type, tgType, Type);
  return this->llvmDataLayout->getTypeAllocSize(tgType->getLlvmType());
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
    args->add(argTypes->getElementKey(i).c_str(), getSharedPtr(contentTypeWrapper));
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
  functionType = std::make_shared<FunctionType>(llvmFuncType, args, getSharedPtr(retTypeWrapper), variadic);
  return true;
}


Bool TargetGenerator::generateFunctionDecl(Char const *name, TiObject *functionType, TioSharedPtr &function)
{
  VALIDATE_NOT_NULL(name, functionType);

  PREPARE_ARG(functionType, funcTypeWrapper, FunctionType);
  auto llvmFuncType = funcTypeWrapper->getLlvmFunctionType();

  // Create the function.
  auto llvmFunc = llvm::Function::Create(
    llvmFuncType, llvm::Function::ExternalLinkage, name, this->llvmModule.get()
  );
  function = Box<llvm::Function*>::create(llvmFunc);
  return true;
}


Bool TargetGenerator::prepareFunctionBody(
  TiObject *function, TiObject *functionType, SharedList<TiObject> *args, TioSharedPtr &context
) {
  VALIDATE_NOT_NULL(function, args);

  // Create the block
  PREPARE_ARG(function, funcWrapper, Box<llvm::Function*>);
  auto llvmFunc = funcWrapper->get();
  auto block = std::make_shared<Block>();
  block->setLlvmBlock(llvm::BasicBlock::Create(myContext, this->getNewBlockName(), llvmFunc));
  block->setIrBuilder(new llvm::IRBuilder<>(block->getLlvmBlock()));
  block->setLlvmFunction(llvmFunc);

  // Prepare the function arguments.
  PREPARE_ARG(functionType, funcTypeWrapper, FunctionType);
  auto argTypes = funcTypeWrapper->getArgs();
  auto i = 0;
  for (auto iter = llvmFunc->arg_begin(); i != argTypes->getElementCount(); ++iter, ++i) {
    auto argType = ti_cast<Type>(argTypes->getElement(i));
    ASSERT(argType != 0);

    iter->setName(argTypes->getElementKey(i).c_str());
    auto llvmAlloca = block->getIrBuilder()->CreateAlloca(
      argType->getLlvmType(), 0, argTypes->getElementKey(i).c_str()
    );
    block->getIrBuilder()->CreateStore(iter, llvmAlloca);

    SharedPtr<Variable> var = std::make_shared<Variable>();
    var->setLlvmAllocaInst(llvmAlloca);
    args->add(var);
  }

  context = block;
  return true;
}


Bool TargetGenerator::finishFunctionBody(
  TiObject *function, TiObject *functionType, DynamicContaining<TiObject> *args, TiObject *context
) {
  PREPARE_ARG(functionType, functionTypeWrapper, FunctionType);
  auto voidRetType = functionTypeWrapper->getRetType().ti_cast_get<VoidType>();
  if (voidRetType != 0) {
    return this->generateReturn(context, 0, 0);
  } else {
    return true;
  }
}


Bool TargetGenerator::deleteFunction(TiObject *function)
{
  PREPARE_ARG(function, funcWrapper, Box<llvm::Function*>);
  funcWrapper->get()->eraseFromParent();
  return true;
}


//==============================================================================
// Variable Definition Generation Functions

Bool TargetGenerator::generateGlobalVariable(
  TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
) {
  PREPARE_ARG(type, typeWrapper, Type);
  auto valWrapper = ti_cast<Value>(defaultValue);

  SharedPtr<Variable> var = std::make_shared<Variable>();
  var->setLlvmGlobalVariable(new llvm::GlobalVariable(
    *(this->llvmModule.get()), typeWrapper->getLlvmType(), false, llvm::GlobalVariable::ExternalLinkage,
    valWrapper != 0 ? valWrapper->getLlvmConstant() : 0, name
  ));

  result = var;
  return true;
}


Bool TargetGenerator::generateLocalVariable(
  TiObject *context, TiObject *type, Char const* name, TiObject *defaultValue, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(type, typeWrapper, Type);
  auto valWrapper = ti_cast<Value>(defaultValue);

  SharedPtr<Variable> var = std::make_shared<Variable>();
  var->setLlvmAllocaInst(block->getIrBuilder()->CreateAlloca(typeWrapper->getLlvmType(), 0, name));
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

  SharedPtr<IfContext> ifContext = std::make_shared<IfContext>();

  // Prepare condition context.
  ifContext->setConditionBlock(getSharedPtr(block));

  // Prepare body context.
  auto bodyBlock = std::make_shared<Block>();
  bodyBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setLlvmFunction(block->getLlvmFunction());
  ifContext->setBodyBlock(bodyBlock);

  // Prepare else context.
  if (withElse) {
    auto elseBlock = std::make_shared<Block>();
    elseBlock->setLlvmBlock(
      llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
    );
    elseBlock->setIrBuilder(new llvm::IRBuilder<>(elseBlock->getLlvmBlock()));
    elseBlock->setLlvmFunction(block->getLlvmFunction());
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
      myContext, this->getNewBlockName(), block->getLlvmFunction()
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
  }

  return true;
}


Bool TargetGenerator::prepareWhileStatement(TiObject *context, SharedPtr<CodeGen::LoopTgContext> &loopTgContext)
{
  PREPARE_ARG(context, block, Block);

  SharedPtr<LoopContext> loopContext = std::make_shared<LoopContext>();

  // Prepare condition context.
  auto condBlock = std::make_shared<Block>();
  condBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  condBlock->setIrBuilder(new llvm::IRBuilder<>(condBlock->getLlvmBlock()));
  condBlock->setLlvmFunction(block->getLlvmFunction());
  loopContext->setConditionBlock(condBlock);

  // Prepare body context.
  auto bodyBlock = std::make_shared<Block>();
  bodyBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setLlvmFunction(block->getLlvmFunction());
  loopContext->setBodyBlock(bodyBlock);

  // Prepare exit block.
  auto exitBlock = std::make_shared<Block>();
  exitBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  exitBlock->setLlvmFunction(block->getLlvmFunction());
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

  SharedPtr<LoopContext> loopContext = std::make_shared<LoopContext>();

  // Prepare condition context.
  auto condBlock = std::make_shared<Block>();
  condBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  condBlock->setIrBuilder(new llvm::IRBuilder<>(condBlock->getLlvmBlock()));
  condBlock->setLlvmFunction(block->getLlvmFunction());
  loopContext->setConditionBlock(condBlock);

  // Prepare increment context.
  auto updaterBlock = std::make_shared<Block>();
  updaterBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  updaterBlock->setIrBuilder(new llvm::IRBuilder<>(updaterBlock->getLlvmBlock()));
  updaterBlock->setLlvmFunction(block->getLlvmFunction());
  loopContext->setUpdaterBlock(updaterBlock);

  // Prepare body context.
  auto bodyBlock = std::make_shared<Block>();
  bodyBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  bodyBlock->setIrBuilder(new llvm::IRBuilder<>(bodyBlock->getLlvmBlock()));
  bodyBlock->setLlvmFunction(block->getLlvmFunction());
  loopContext->setBodyBlock(bodyBlock);

  // Prepare exit block.
  auto exitBlock = std::make_shared<Block>();
  exitBlock->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  exitBlock->setLlvmFunction(block->getLlvmFunction());
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
  destVal = std::make_shared<Value>(llvmCastedValue, false);
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
  destVal = std::make_shared<Value>(llvmCastedValue, false);
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
  destVal = std::make_shared<Value>(llvmCastedValue, false);
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
  destVal = std::make_shared<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastIntToPointer(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreateIntToPtr(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = std::make_shared<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastPointerToInt(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreatePtrToInt(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = std::make_shared<Value>(llvmCastedValue, false);
  return true;
}


Bool TargetGenerator::generateCastPointerToPointer(
  TiObject *context, TiObject *srcType, TiObject *destType, TiObject *srcVal, TioSharedPtr &destVal
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destType, typeWrapper, Type);
  auto llvmCastedValue = block->getIrBuilder()->CreateBitCast(cgSrcVal->getLlvmValue(), typeWrapper->getLlvmType());
  destVal = std::make_shared<Value>(llvmCastedValue, false);
  return true;
}


//==============================================================================
// Operation Generation Functions

Bool TargetGenerator::generateVarReference(
  TiObject *context, TiObject *varType, TiObject *varDefinition, TioSharedPtr &result
) {
  PREPARE_ARG(varDefinition, var, Variable);
  if (var->getLlvmAllocaInst() != 0) {
    result = std::make_shared<Value>(var->getLlvmAllocaInst(), false);
  } else if (var->getLlvmGlobalVariable() != 0) {
    result = std::make_shared<Value>(var->getLlvmGlobalVariable(), false);
  } else {
    throw EXCEPTION(GenericException, S("Variable definition was not generated correctly."));
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

  auto zero = llvm::ConstantInt::get(myContext, llvm::APInt(32, 0, true));
  auto index = llvm::ConstantInt::get(
    myContext, llvm::APInt(32, tgMemberVarDef->getLlvmStructIndex(), true)
  );
  auto llvmResult = block->getIrBuilder()->CreateGEP(
    llvmPtr, llvm::makeArrayRef(std::vector<llvm::Value*>({ zero, index })), ""
  );
  result = std::make_shared<Value>(llvmResult, false);
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

  auto zero = llvm::ConstantInt::get(myContext, llvm::APInt(32, 0, true));
  auto llvmResult = block->getIrBuilder()->CreateGEP(
    llvmPtr, llvm::makeArrayRef(std::vector<llvm::Value*>({ zero, tgIndex->getLlvmValue() })), ""
  );
  result = std::make_shared<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateDereference(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  auto llvmResult = block->getIrBuilder()->CreateLoad(cgSrcVal->getLlvmValue());
  result = std::make_shared<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateAssign(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destRef, cgDestRef, Value);
  auto llvmResult = block->getIrBuilder()->CreateStore(cgSrcVal->getLlvmValue(), cgDestRef->getLlvmValue());
  result = std::make_shared<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateFunctionPointer(
  TiObject *context, TiObject *function, TiObject *functionPtrType, TioSharedPtr &result
) {
  PREPARE_ARG(functionPtrType, functionPtrTypeWrapper, PointerType);
  PREPARE_ARG(function, funcWrapper, Box<llvm::Function*>);

  auto llvmResult = llvm::ConstantExpr::getBitCast(funcWrapper->get(), functionPtrTypeWrapper->getLlvmType());
  result = std::make_shared<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateFunctionCall(
  TiObject *context, TiObject *function,
  Containing<TiObject>* arguments, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(function, llvmFuncBox, Box<llvm::Function*>);

  // Create function call.
  std::vector<llvm::Value*> args;
  for (Int i = 0; i < arguments->getElementCount(); ++i) {
    auto llvmValBox = ti_cast<Value>(arguments->getElement(i));
    if (llvmValBox == 0) {
      throw EXCEPTION(InvalidArgumentException, S("arguments"), S("Some elements are null or of invalid type."));
    }
    args.push_back(llvmValBox->getLlvmValue());
  }
  auto llvmCall = block->getIrBuilder()->CreateCall(llvmFuncBox->get(), args);
  result = std::make_shared<Value>(llvmCall, false);
  return true;
}


Bool TargetGenerator::generateFunctionPtrCall(
  TiObject *context, TiObject *functionPtr, TiObject *functionPtrType,
  Containing<TiObject>* arguments, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(functionPtr, llvmFuncPtrBox, Value);

  // TODO: Validate provided args against functionptrType.

  // Create function call.
  std::vector<llvm::Value*> args;
  for (Int i = 0; i < arguments->getElementCount(); ++i) {
    auto llvmValBox = ti_cast<Value>(arguments->getElement(i));
    if (llvmValBox == 0) {
      throw EXCEPTION(InvalidArgumentException, S("arguments"), S("Some elements are null or of invalid type."));
    }
    args.push_back(llvmValBox->getLlvmValue());
  }
  auto llvmCall = block->getIrBuilder()->CreateCall(llvmFuncPtrBox->getLlvmValue(), args);
  result = std::make_shared<Value>(llvmCall, false);
  return true;
}


Bool TargetGenerator::generateReturn(
  TiObject *context, TiObject *retType, TiObject *retVal
) {
  PREPARE_ARG(context, block, Block);
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

  auto block2 = std::make_shared<Block>();
  block2->setLlvmBlock(
    llvm::BasicBlock::Create(myContext, this->getNewBlockName(), block->getLlvmFunction())
  );
  block2->setIrBuilder(new llvm::IRBuilder<>(block2->getLlvmBlock()));
  block2->setLlvmFunction(block->getLlvmFunction());
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
    myContext, this->getNewBlockName(), block->getLlvmFunction()
  );
  block2->getIrBuilder()->CreateBr(mergeLlvmBlock);

  // Create the if statement.
  block->getIrBuilder()->CreateCondBr(val1Wrapper->getLlvmValue(), mergeLlvmBlock, block2->getLlvmEntryBlock());

  // Set insert point to the merge body.
  auto block1 = block->getLlvmBlock();
  block->getIrBuilder()->SetInsertPoint(mergeLlvmBlock);
  block->setLlvmBlock(mergeLlvmBlock);

  // Generate phi value.
  auto boolType = llvm::Type::getIntNTy(myContext, 1);
  llvm::PHINode *phi = block->getIrBuilder()->CreatePHI(boolType, 0);
  phi->addIncoming(val1Wrapper->getLlvmValue(), block1);
  phi->addIncoming(val2Wrapper->getLlvmValue(), block2->getLlvmBlock());

  result = std::make_shared<Value>(phi);
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
    myContext, this->getNewBlockName(), block->getLlvmFunction()
  );
  block2->getIrBuilder()->CreateBr(mergeLlvmBlock);

  // Create the if statement.
  block->getIrBuilder()->CreateCondBr(val1Wrapper->getLlvmValue(), block2->getLlvmEntryBlock(), mergeLlvmBlock);

  // Set insert point to the merge body.
  auto block1 = block->getLlvmBlock();
  block->getIrBuilder()->SetInsertPoint(mergeLlvmBlock);
  block->setLlvmBlock(mergeLlvmBlock);

  // Generate phi value.
  auto boolType = llvm::Type::getIntNTy(myContext, 1);
  llvm::PHINode *phi = block->getIrBuilder()->CreatePHI(boolType, 0);
  phi->addIncoming(val1Wrapper->getLlvmValue(), block1);
  phi->addIncoming(val2Wrapper->getLlvmValue(), block2->getLlvmBlock());

  result = std::make_shared<Value>(phi);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFRem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
  result = std::make_shared<Value>(llvmResult, false);
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
  result = std::make_shared<Value>(llvmResult, false);
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
  result = std::make_shared<Value>(llvmResult, false);
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
  result = std::make_shared<Value>(llvmResult, false);
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
  result = std::make_shared<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateNot(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateNot(srcValBox->getLlvmValue());
  result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFNeg(srcValBox->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = std::make_shared<Value>(llvmResult, false);
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
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = std::make_shared<Value>(llvmResult, false);
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
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFAdd(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = std::make_shared<Value>(llvmVal, false);
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
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(
        llvmVal, llvm::ConstantInt::get(myContext, llvm::APInt(size, 1, true))
      );
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    if (size == 32) {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Float)1))
      );
    } else {
      llvmResult = block->getIrBuilder()->CreateFSub(
        llvmVal, llvm::ConstantFP::get(myContext, llvm::APFloat((Double)1))
      );
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = std::make_shared<Value>(llvmVal, false);
  return true;
}


Bool TargetGenerator::generateAddAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWAdd(llvmVal, srcValBox->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateAdd(llvmVal, srcValBox->getLlvmValue());
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    llvmResult = block->getIrBuilder()->CreateFAdd(llvmVal, srcValBox->getLlvmValue());
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateSubAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWSub(llvmVal, srcValBox->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateSub(llvmVal, srcValBox->getLlvmValue());
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    llvmResult = block->getIrBuilder()->CreateFSub(llvmVal, srcValBox->getLlvmValue());
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateMulAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateNSWMul(llvmVal, srcValBox->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateMul(llvmVal, srcValBox->getLlvmValue());
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    llvmResult = block->getIrBuilder()->CreateFMul(llvmVal, srcValBox->getLlvmValue());
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateDivAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateSDiv(llvmVal, srcValBox->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateUDiv(llvmVal, srcValBox->getLlvmValue());
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    llvmResult = block->getIrBuilder()->CreateFDiv(llvmVal, srcValBox->getLlvmValue());
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateRemAssign(
  TiObject *context, TiObject *type, TiObject *destVar, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);
  auto llvmVal = block->getIrBuilder()->CreateLoad(destVarBox->getLlvmValue());
  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = block->getIrBuilder()->CreateSRem(llvmVal, srcValBox->getLlvmValue());
    } else {
      llvmResult = block->getIrBuilder()->CreateURem(llvmVal, srcValBox->getLlvmValue());
    }
  } else if (tgType->isDerivedFrom<FloatType>()) {
    llvmResult = block->getIrBuilder()->CreateFRem(llvmVal, srcValBox->getLlvmValue());
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }
  block->getIrBuilder()->CreateStore(llvmResult, destVarBox->getLlvmValue());
  result = getSharedPtr(destVar);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOEQ(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpONE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOGT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOGE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOLT(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
    result = std::make_shared<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = block->getIrBuilder()->CreateFCmpOLE(srcValBox1->getLlvmValue(), srcValBox2->getLlvmValue());
    result = std::make_shared<Value>(llvmResult, false);
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
  auto llvmResult = llvm::ConstantInt::get(myContext, llvm::APInt(bitCount, value, withSign));
  destVal = std::make_shared<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateFloatLiteral(
  TiObject *context, Word bitCount, Double value, TioSharedPtr &destVal
) {
  llvm::Value *llvmResult;
  if (bitCount == 32) {
    llvmResult = llvm::ConstantFP::get(myContext, llvm::APFloat((Float)value));
  } else {
    llvmResult = llvm::ConstantFP::get(myContext, llvm::APFloat(value));
  }
  destVal = std::make_shared<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generateStringLiteral(
  TiObject *context, Char const* value, TiObject *charType, TiObject *strType, TioSharedPtr &destVal
) {
  PREPARE_ARG(charType, cgCharType, Type);
  PREPARE_ARG(strType, cgStrType, Type);

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
  auto llvmVar = new llvm::GlobalVariable(
    *(this->llvmModule.get()), cgStrType->getLlvmType(), true,
    llvm::GlobalValue::PrivateLinkage, llvmStrConst, this->getAnonymouseVarName().c_str()
  );
  llvmVar->setAlignment(1);

  destVal = std::make_shared<Value>(llvmVar, true);
  return true;
}


Bool TargetGenerator::generateNullPtrLiteral(
  TiObject *context, TiObject *type, TioSharedPtr &destVal
) {
  PREPARE_ARG(type, tgType, PointerType);
  auto llvmResult = llvm::ConstantPointerNull::get(static_cast<llvm::PointerType*>(tgType->getLlvmType()));
  destVal = std::make_shared<Value>(llvmResult, true);
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
  destVal = std::make_shared<Value>(llvmResult, true);
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
  destVal = std::make_shared<Value>(llvmResult, true);
  return true;
}


Bool TargetGenerator::generatePointerLiteral(TiObject *context, TiObject *type, void *value, TioSharedPtr &destVal)
{
  PREPARE_ARG(type, tgType, PointerType);
  auto llvmInt = llvm::ConstantInt::get(myContext, llvm::APInt(64, (uint64_t)value, false));
  auto llvmPtr = llvm::ConstantExpr::getIntToPtr(llvmInt, tgType->getLlvmType());
  destVal = std::make_shared<Value>(llvmPtr, true);
  return true;
}


//==============================================================================
// Helper Functions

Str TargetGenerator::getNewBlockName()
{
  return Str("__block") + std::to_string(this->blockIndex++);
}


Str TargetGenerator::getAnonymouseVarName()
{
  return Str("__anonymous") + std::to_string(this->anonymousVarIndex++);
}

} } // namespace
