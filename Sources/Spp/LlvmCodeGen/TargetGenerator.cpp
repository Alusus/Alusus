/**
 * @file Spp/LlvmCodeGen/TargetGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::TargetGenerator.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
// C ABI Helper Functions

namespace
{
  /// Per-eightbyte classification used by the x86-64 System V ABI.
  struct EightbyteInfo
  {
    enum class Cls { NONE, INTEGER, SSE };
    Cls cls = Cls::NONE;
    Bool hasDouble = false;
  };

  /// Classify the scalar leaves of a type, merging them into the eightbytes they occupy.
  /// Returns false if the type must be passed in memory (unaligned fields or unsupported types).
  Bool classifyLeaves(llvm::DataLayout *dl, llvm::Type *type, uint64_t offset, EightbyteInfo *eightbytes)
  {
    if (offset % dl->getABITypeAlign(type).value() != 0) return false;

    if (auto structType = llvm::dyn_cast<llvm::StructType>(type)) {
      auto layout = dl->getStructLayout(structType);
      for (unsigned i = 0; i < structType->getNumElements(); ++i) {
        if (!classifyLeaves(dl, structType->getElementType(i), offset + layout->getElementOffset(i), eightbytes)) {
          return false;
        }
      }
      return true;
    }

    if (auto arrayType = llvm::dyn_cast<llvm::ArrayType>(type)) {
      auto elementSize = dl->getTypeAllocSize(arrayType->getElementType());
      for (uint64_t i = 0; i < arrayType->getNumElements(); ++i) {
        if (!classifyLeaves(dl, arrayType->getElementType(), offset + i * elementSize, eightbytes)) return false;
      }
      return true;
    }

    auto &eb = eightbytes[offset / 8];
    if (type->isIntegerTy() || type->isPointerTy()) {
      if (dl->getTypeStoreSize(type) > 8) return false;
      eb.cls = EightbyteInfo::Cls::INTEGER;
      return true;
    }
    if (type->isFloatTy()) {
      if (eb.cls == EightbyteInfo::Cls::NONE) eb.cls = EightbyteInfo::Cls::SSE;
      return true;
    }
    if (type->isDoubleTy()) {
      if (eb.cls == EightbyteInfo::Cls::NONE) eb.cls = EightbyteInfo::Cls::SSE;
      eb.hasDouble = true;
      return true;
    }
    // Everything else (long double, vectors, ...) is not supported in registers.
    return false;
  }
}


AbiInfo TargetGenerator::classifyStruct(llvm::Type *llvmType, Bool isArg, Int &intRegs, Int &sseRegs)
{
  AbiInfo abi;
  abi.kind = AbiInfo::Kind::INDIRECT;
  if (!this->buildTarget->isSysVX86_64Abi()) return abi;

  auto dl = this->buildTarget->getLlvmDataLayout();
  auto size = dl->getTypeAllocSize(llvmType).getFixedValue();
  if (size == 0 || size > 16) return abi;

  EightbyteInfo eightbytes[2];
  if (!classifyLeaves(dl, llvmType, 0, eightbytes)) return abi;

  auto &llvmContext = *this->buildTarget->getLlvmContext();
  Int neededInt = 0;
  Int neededSse = 0;
  std::vector<llvm::Type*> parts;
  for (Int i = 0; i < (Int)((size + 7) / 8); ++i) {
    auto bytes = std::min<uint64_t>(8, size - i * 8);
    if (eightbytes[i].cls == EightbyteInfo::Cls::SSE) {
      ++neededSse;
      if (eightbytes[i].hasDouble) parts.push_back(llvm::Type::getDoubleTy(llvmContext));
      else if (bytes > 4) parts.push_back(llvm::FixedVectorType::get(llvm::Type::getFloatTy(llvmContext), 2));
      else parts.push_back(llvm::Type::getFloatTy(llvmContext));
    } else {
      ++neededInt;
      parts.push_back(llvm::Type::getIntNTy(llvmContext, bytes * 8));
    }
  }

  // An argument that doesn't fit in the remaining registers is passed in memory as a whole.
  if (isArg) {
    if (neededInt > intRegs || neededSse > sseRegs) return abi;
    intRegs -= neededInt;
    sseRegs -= neededSse;
  }

  abi.kind = AbiInfo::Kind::COERCED;
  abi.parts = std::move(parts);
  abi.paramCount = (Int)abi.parts.size();
  return abi;
}


template <class T> void TargetGenerator::applyAbiAttributes(T *target, FunctionType *funcType)
{
  auto &llvmContext = *this->buildTarget->getLlvmContext();
  auto dl = this->buildTarget->getLlvmDataLayout();

  // The sret attribute goes on the hidden first parameter.
  auto &retAbi = funcType->getRetAbi();
  if (retAbi.kind == AbiInfo::Kind::INDIRECT) {
    target->addParamAttr(0, llvm::Attribute::get(
      llvmContext, llvm::Attribute::StructRet, funcType->getRetType()->getLlvmType()
    ));
  }

  // The byval attribute makes the backend copy the struct to the stack.
  auto argTypes = funcType->getArgs();
  for (Int i = 0; i < argTypes->getElementCount(); ++i) {
    auto &abi = funcType->getArgAbi(i);
    if (abi.kind != AbiInfo::Kind::INDIRECT) continue;
    auto llvmType = argTypes->getElement(i)->getLlvmType();
    target->addParamAttr(abi.firstParam, llvm::Attribute::getWithByValType(llvmContext, llvmType));
    target->addParamAttr(abi.firstParam, llvm::Attribute::getWithAlignment(
      llvmContext, std::max(llvm::Align(8), dl->getABITypeAlign(llvmType))
    ));
  }
}


void TargetGenerator::lowerStructArg(
  llvm::IRBuilder<> *builder, llvm::Value *structValue, AbiInfo const &abi, std::vector<llvm::Value*> &args
) {
  auto dl = this->buildTarget->getLlvmDataLayout();
  auto structType = structValue->getType();
  auto structAlign = dl->getABITypeAlign(structType);

  // Get the struct into memory. If the value at hand is a load instruction that nothing else uses, we can reuse its
  // pointer operand to avoid an unnecessary load/store pair.
  llvm::Value *ptr;
  auto loadInst = llvm::dyn_cast<llvm::LoadInst>(structValue);
  if (loadInst != nullptr && loadInst->use_empty()) {
    ptr = loadInst->getPointerOperand();
    loadInst->eraseFromParent();
  } else {
    auto allocaInst = builder->CreateAlloca(structType, nullptr, "");
    allocaInst->setAlignment(structAlign);
    builder->CreateStore(structValue, allocaInst)->setAlignment(structAlign);
    ptr = allocaInst;
  }

  if (abi.kind == AbiInfo::Kind::INDIRECT) {
    args.push_back(ptr);
    return;
  }

  // Load each eightbyte as its own argument.
  auto int8Type = llvm::Type::getInt8Ty(*this->buildTarget->getLlvmContext());
  for (Int i = 0; i < (Int)abi.parts.size(); ++i) {
    auto partPtr = i == 0 ? ptr : builder->CreateConstInBoundsGEP1_64(int8Type, ptr, i * 8);
    auto loadInst = builder->CreateLoad(abi.parts[i], partPtr);
    loadInst->setAlignment(llvm::commonAlignment(structAlign, i * 8));
    args.push_back(loadInst);
  }
}


llvm::Value* TargetGenerator::raiseStructArg(
  llvm::IRBuilder<> *builder, llvm::Type *structType, AbiInfo const &abi, llvm::Value **params
) {
  auto dl = this->buildTarget->getLlvmDataLayout();
  auto structAlign = dl->getABITypeAlign(structType);

  if (abi.kind == AbiInfo::Kind::INDIRECT) {
    auto loadInst = builder->CreateLoad(structType, params[0]);
    loadInst->setAlignment(structAlign);
    return loadInst;
  }

  // Store the eightbytes into a temporary struct and load that.
  auto int8Type = llvm::Type::getInt8Ty(*this->buildTarget->getLlvmContext());
  auto allocaInst = builder->CreateAlloca(structType, nullptr, "");
  allocaInst->setAlignment(structAlign);
  for (Int i = 0; i < (Int)abi.parts.size(); ++i) {
    auto partPtr = i == 0 ?
      (llvm::Value*)allocaInst : builder->CreateConstInBoundsGEP1_64(int8Type, allocaInst, i * 8);
    builder->CreateStore(params[i], partPtr)->setAlignment(llvm::commonAlignment(structAlign, i * 8));
  }
  auto loadInst = builder->CreateLoad(structType, allocaInst);
  loadInst->setAlignment(structAlign);
  return loadInst;
}


llvm::Value* TargetGenerator::lowerStructRet(llvm::IRBuilder<> *builder, llvm::Value *structValue, AbiInfo const &abi)
{
  std::vector<llvm::Value*> parts;
  this->lowerStructArg(builder, structValue, abi, parts);
  if (parts.size() == 1) return parts[0];
  llvm::Value *result = llvm::PoisonValue::get(
    llvm::StructType::get(*this->buildTarget->getLlvmContext(), abi.parts)
  );
  for (Int i = 0; i < (Int)parts.size(); ++i) result = builder->CreateInsertValue(result, parts[i], i);
  return result;
}


llvm::Value* TargetGenerator::raiseStructRet(
  llvm::IRBuilder<> *builder, llvm::Type *structType, AbiInfo const &abi, llvm::Value *retValue
) {
  std::vector<llvm::Value*> parts;
  if (abi.parts.size() == 1) {
    parts.push_back(retValue);
  } else {
    for (Int i = 0; i < (Int)abi.parts.size(); ++i) parts.push_back(builder->CreateExtractValue(retValue, i));
  }
  return this->raiseStructArg(builder, structType, abi, parts.data());
}


llvm::Value* TargetGenerator::generateAbiCall(
  llvm::IRBuilder<> *builder, FunctionType *funcType, llvm::Value *callee, Containing<TiObject>* arguments
) {
  auto dl = this->buildTarget->getLlvmDataLayout();
  auto retType = funcType->getRetType();
  auto &retAbi = funcType->getRetAbi();
  std::vector<llvm::Value*> args;

  // If the result is returned in memory, allocate space and pass it as the first sret pointer argument.
  llvm::Value *sretPtr = nullptr;
  if (retAbi.kind == AbiInfo::Kind::INDIRECT) {
    auto allocaInst = builder->CreateAlloca(retType->getLlvmType(), nullptr, "");
    allocaInst->setAlignment(dl->getABITypeAlign(retType->getLlvmType()));
    sretPtr = allocaInst;
    args.push_back(sretPtr);
  }

  auto argCount = funcType->getArgs()->getElementCount();
  for (Int i = 0; i < arguments->getElementCount(); ++i) {
    auto llvmValBox = ti_cast<Value>(arguments->getElement(i));
    if (llvmValBox == 0) {
      throw EXCEPTION(InvalidArgumentException, S("arguments"), S("Some elements are null or of invalid type."));
    }
    auto llvmValue = llvmValBox->getLlvmValue();
    // Variadic arguments beyond the declared ones are always passed as is.
    if (i < argCount && llvmValue->getType()->isStructTy()) {
      this->lowerStructArg(builder, llvmValue, funcType->getArgAbi(i), args);
    } else if (llvmValue->getType()->isStructTy()) {
      // Variadic struct arguments are passed by pointer.
      std::vector<llvm::Value*> tmp;
      AbiInfo abi;
      abi.kind = AbiInfo::Kind::INDIRECT;
      this->lowerStructArg(builder, llvmValue, abi, tmp);
      args.push_back(tmp[0]);
    } else {
      args.push_back(llvmValue);
    }
  }

  auto llvmCall = builder->CreateCall(funcType->getLlvmFunctionType(), callee, args);
  this->applyAbiAttributes(llvmCall, funcType);

  // Get the result as a normal value.
  if (retAbi.kind == AbiInfo::Kind::INDIRECT) {
    auto loadInst = builder->CreateLoad(retType->getLlvmType(), sretPtr);
    loadInst->setAlignment(dl->getABITypeAlign(retType->getLlvmType()));
    return loadInst;
  } else if (retAbi.kind == AbiInfo::Kind::COERCED) {
    return this->raiseStructRet(builder, retType->getLlvmType(), retAbi, llvmCall);
  }
  return llvmCall;
}


//==============================================================================
// Function Generation Functions

Bool TargetGenerator::generateFunctionType(
  MapContaining<TiObject>* argTypes, TiObject *retType, Bool variadic, TioSharedPtr &functionType
) {
  VALIDATE_NOT_NULL(argTypes, retType);

  // Prepare ret type.
  auto retTypeWrapper = ti_cast<Type>(retType);
  if (retTypeWrapper == 0) {
    throw EXCEPTION(
      InvalidArgumentException, S("retType"), S("Not an instance of LlvmCodeGen::Type")
    );
  }

  // Prepare args and apply the C ABI.
  auto args = SharedMap<Type>::create({});
  std::vector<llvm::Type*> llvmArgTypes;
  llvmArgTypes.reserve(argTypes->getElementCount() + 1); // +1 for possible sret parameter

  // Registers still available for passing arguments. The sret pointer takes the first integer register.
  Int intRegs = 6;
  Int sseRegs = 8;

  // Return type.
  auto llvmRetType = retTypeWrapper->getLlvmType();
  llvm::Type *llvmFuncRetType = llvmRetType;
  AbiInfo retAbi;
  if (llvmRetType->isStructTy()) {
    retAbi = this->classifyStruct(llvmRetType, false, intRegs, sseRegs);
    if (retAbi.kind == AbiInfo::Kind::INDIRECT) {
      // The result is returned through a hidden pointer passed as the first argument.
      llvmArgTypes.push_back(llvmRetType->getPointerTo());
      llvmFuncRetType = llvm::Type::getVoidTy(*this->buildTarget->getLlvmContext());
      --intRegs;
    } else if (retAbi.parts.size() == 1) {
      llvmFuncRetType = retAbi.parts[0];
    } else {
      llvmFuncRetType = llvm::StructType::get(*this->buildTarget->getLlvmContext(), retAbi.parts);
    }
  }

  // Arguments.
  std::vector<AbiInfo> argAbis;
  for (Int i = 0; i < argTypes->getElementCount(); ++i) {
    auto contentTypeWrapper = ti_cast<Type>(argTypes->getElement(i));
    if (contentTypeWrapper == 0) {
      throw EXCEPTION(
        InvalidArgumentException, S("argTypes"), S("Not all elements are instances of LlvmCodeGen::Type")
      );
    }
    args->add(argTypes->getElementKey(i), getSharedPtr(contentTypeWrapper));

    auto llvmType = contentTypeWrapper->getLlvmType();
    AbiInfo abi;
    if (llvmType->isStructTy()) {
      abi = this->classifyStruct(llvmType, true, intRegs, sseRegs);
    } else if (llvmType->isFloatingPointTy()) {
      if (sseRegs > 0) --sseRegs;
    } else if (intRegs > 0) {
      --intRegs;
    }
    abi.firstParam = (Int)llvmArgTypes.size();
    if (abi.kind == AbiInfo::Kind::INDIRECT) {
      // Passed by pointer using the byval attribute.
      llvmArgTypes.push_back(llvmType->getPointerTo());
      abi.paramCount = 1;
    } else if (abi.kind == AbiInfo::Kind::COERCED) {
      for (auto part : abi.parts) llvmArgTypes.push_back(part);
      abi.paramCount = (Int)abi.parts.size();
    } else {
      llvmArgTypes.push_back(llvmType);
      abi.paramCount = 1;
    }
    argAbis.push_back(std::move(abi));
  }

  // Create the function.
  auto llvmFuncType = llvm::FunctionType::get(llvmFuncRetType, llvmArgTypes, variadic);
  functionType = newSrdObj<FunctionType>(
    llvmFuncType, args, getSharedPtr(retTypeWrapper), variadic, retAbi, argAbis
  );
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
    // C ABI compatibility.
    this->applyAbiAttributes(llvmFunc, funcTypeWrapper);
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
    // C ABI compatibility.
    this->applyAbiAttributes(llvmFunc, funcWrapper->getFunctionType());
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
  auto iter = llvmFunc->arg_begin();

  // C ABI compatibility:
  // If the return value is passed in memory, the first parameter is the sret pointer.
  if (funcTypeWrapper->getRetAbi().kind == AbiInfo::Kind::INDIRECT) {
    iter->setName("__sret");
    funcWrapper->llvmSretPtr = &*iter;
    ++iter;
  }

  for (Int i = 0; i != argTypes->getElementCount(); ++i) {
    auto &abi = funcTypeWrapper->getArgAbi(i);
    auto argType = argTypes->getElement(i);
    // Name the LLVM parameters that make up this argument.
    std::vector<llvm::Value*> params;
    for (Int j = 0; j < abi.paramCount; ++j, ++iter) {
      std::string paramName = argTypes->getElementKey(i).getBuf();
      if (abi.paramCount > 1) paramName += "." + std::to_string(j);
      iter->setName(paramName);
      params.push_back(&*iter);
    }
    // C ABI compatibility:
    // Structs are passed by pointer or split into registers, but the Alusus code generator expects a value, so we'll
    // rebuild the value here.
    if (argType->getLlvmType()->isStructTy()) {
      auto structValue = this->raiseStructArg(block->getIrBuilder(), argType->getLlvmType(), abi, params.data());
      args->add(newSrdObj<Value>(structValue, false));
    } else {
      args->add(newSrdObj<Value>(params[0], false));
    }
  }

  // Is this a variadic funciton?
  if (funcWrapper->getFunctionType()->isVariadic()) {
    // Declare va_list var.
    auto vaListAlloca = block->getIrBuilder()->CreateAlloca(this->buildTarget->getVaListType(), 0, "__vaList");
    vaListAlloca->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(this->buildTarget->getVaListType()));
    funcWrapper->llvmVaList = vaListAlloca;
    
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
  auto allocaInst = block->getIrBuilder()->CreateAlloca(typeWrapper->getLlvmType(), 0, name);
  allocaInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(typeWrapper->getLlvmType()));
  var->setLlvmAllocaInst(allocaInst);
  #if __APPLE__
    // TODO: This alignment should depend on build target rather than current OS.
    var->getLlvmAllocaInst()->setAlignment(llvm::MaybeAlign(16));
  #endif
  if (valWrapper != 0) {
    auto storeInst = block->getIrBuilder()->CreateStore(valWrapper->getLlvmValue(), var->getLlvmAllocaInst());
    storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(typeWrapper->getLlvmType()));
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

  llvm::IRBuilder<> *builder = block->getIrBuilder();
  auto &ctx = *this->buildTarget->getLlvmContext();

  llvm::Value *basePtr = nullptr;
  llvm::Type *structLlvmTy = nullptr;

  if (tgStructType->isDerivedFrom<PointerType>()) {
    auto ptrType = ti_cast<PointerType>(tgStructType);
    ASSERT(ptrType != nullptr);
    structLlvmTy = ptrType->getContentType()->getLlvmType();
    basePtr = tgStructRef->getLlvmValue(); // already ptr-to-struct
  } else {
    structLlvmTy = tgStructType->getLlvmType(); // struct type
    auto allocaInst = builder->CreateAlloca(structLlvmTy, nullptr, "");
    allocaInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(structLlvmTy));
    basePtr = allocaInst;
    auto storeInst = builder->CreateStore(tgStructRef->getLlvmValue(), basePtr);
    storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(structLlvmTy));
  }

  auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
  auto index = llvm::ConstantInt::get(
    llvm::Type::getInt32Ty(ctx),
    tgMemberVarDef->getLlvmStructIndex()
  );

  llvm::Value *llvmResult = builder->CreateGEP(
    structLlvmTy,          // pointee type: the struct
    basePtr,               // ptr-to-struct
    { zero, index },       // [0, fieldIndex]
    ""
  );

  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateArrayElementReference(
  TiObject *context, TiObject *arrayType, TiObject *elementType,
  TiObject *index, TiObject *arrayRef, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(arrayRef, tgArrayRef, Value);
  PREPARE_ARG(arrayType, tgArrayType, Type);
  PREPARE_ARG(elementType, tgElementType, Type);
  PREPARE_ARG(index, tgIndex, Value);

  llvm::IRBuilder<> *builder = block->getIrBuilder();
  auto &ctx = *this->buildTarget->getLlvmContext();

  llvm::Value *basePtr = nullptr;
  llvm::Type *pointeeTy = nullptr;

  // CASE 1: array value ([N x T])
  if (auto arrTy = ti_cast<ArrayType>(tgArrayType)) {
    pointeeTy = arrTy->getLlvmType(); // [N x T]
    auto allocaInst = builder->CreateAlloca(pointeeTy);
    allocaInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(pointeeTy));
    basePtr = allocaInst;
    auto storeInst = builder->CreateStore(tgArrayRef->getLlvmValue(), basePtr);
    storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(pointeeTy));

    auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);

    llvm::Value *elemPtr = builder->CreateGEP(
      pointeeTy,
      basePtr,
      { zero, tgIndex->getLlvmValue() }
    );

    result = newSrdObj<Value>(elemPtr, true);
    return true;
  }

  // CASE 2: pointer to array ([N x T]*)
  if (auto ptrTy = ti_cast<PointerType>(tgArrayType)) {
    auto contentTy = ptrTy->getContentType()->getLlvmType();

    if (contentTy->isArrayTy()) {
      pointeeTy = contentTy; // [N x T]
      basePtr = tgArrayRef->getLlvmValue();

      auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);

      llvm::Value *elemPtr = builder->CreateGEP(
        pointeeTy,
        basePtr,
        { zero, tgIndex->getLlvmValue() }
      );

      result = newSrdObj<Value>(elemPtr, true);
      return true;
    }

    // CASE 3: pointer to element (T*)
    pointeeTy = contentTy; // T
    basePtr = tgArrayRef->getLlvmValue();

    llvm::Value *elemPtr = builder->CreateGEP(
      pointeeTy,
      basePtr,
      tgIndex->getLlvmValue()
    );

    result = newSrdObj<Value>(elemPtr, true);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid array reference type."));
}


Bool TargetGenerator::generateDereference(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(contentType, cgContentType, Type);

  auto llvmResult = block->getIrBuilder()->CreateLoad(
    cgContentType->getLlvmType(),
    cgSrcVal->getLlvmValue()
  );
  llvmResult->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(cgContentType->getLlvmType()));

  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateAssign(
  TiObject *context, TiObject *contentType, TiObject *srcVal, TiObject *destRef, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, cgSrcVal, Value);
  PREPARE_ARG(destRef, cgDestRef, Value);
  PREPARE_ARG(contentType, cgContentType, Type);

  auto storeInst = block->getIrBuilder()->CreateStore(
    cgSrcVal->getLlvmValue(),
    cgDestRef->getLlvmValue()
  );
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(cgContentType->getLlvmType()));

  result = getSharedPtr(destRef);
  return true;
}


Bool TargetGenerator::generateFunctionPointer(
  TiObject *context, TiObject *function, TiObject *functionPtrType, TioSharedPtr &result
) {
  PREPARE_ARG(functionPtrType, functionPtrTypeWrapper, PointerType);
  PREPARE_ARG(function, funcWrapper, Function);
  PREPARE_ARG(context, block, Block);

  llvm::Module *llvmMod = this->perFunctionModules ?
    block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();

  llvm::Function *llvmFunc = llvmMod->getFunction(funcWrapper->getName().getBuf());
  if (llvmFunc == nullptr) {
    // This function is in a different module, so we'll have to declare it.
    llvmFunc = llvm::Function::Create(
      funcWrapper->getFunctionType()->getLlvmFunctionType(),
      llvm::Function::ExternalLinkage,
      funcWrapper->getName().getBuf(),
      llvmMod
    );

    // C ABI compatibility.
    this->applyAbiAttributes(llvmFunc, funcWrapper->getFunctionType());
  }

  // Generate the func pointer.
  auto llvmResult = llvm::ConstantExpr::getBitCast(
    llvmFunc,
    functionPtrTypeWrapper->getLlvmType()
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

  llvm::IRBuilder<> *builder = block->getIrBuilder();

  // Make sure a declaration of this function exists in the current module.
  llvm::Module *llvmMod = this->perFunctionModules ?
    block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();
  llvm::Function *llvmFunc = llvmMod->getFunction(funcWrapper->getName().getBuf());
  if (llvmFunc == nullptr) {
    // This function is in a different module, so we'll have to define it.
    llvmFunc = llvm::Function::Create(
      funcWrapper->getFunctionType()->getLlvmFunctionType(),
      llvm::Function::ExternalLinkage,
      funcWrapper->getName().getBuf(),
      llvmMod
    );
    // C ABI compatibility.
    this->applyAbiAttributes(llvmFunc, funcWrapper->getFunctionType());
  }

  // Create the call, applying the C ABI to the arguments and result.
  auto llvmResult = this->generateAbiCall(builder, funcWrapper->getFunctionType(), llvmFunc, arguments);
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateFunctionPtrCall(
  TiObject *context, TiObject *functionPtr, TiObject *functionPtrType,
  Containing<TiObject>* arguments, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(functionPtr, llvmFuncPtrBox, Value);
  PREPARE_ARG(functionPtrType, llvmFuncPtrTypeBox, PointerType);

  llvm::IRBuilder<> *builder = block->getIrBuilder();

  // TODO: Validate provided args against functionPtrType.

  auto llvmFuncTypeBox = llvmFuncPtrTypeBox->getContentType().ti_cast_get<FunctionType>();
  if (llvmFuncTypeBox == 0) {
    throw EXCEPTION(InvalidArgumentException, S("functionPtrType"), S("Argument is not a function pointer type."));
  }

  // Create the call, applying the C ABI to the arguments and result.
  auto llvmResult = this->generateAbiCall(builder, llvmFuncTypeBox, llvmFuncPtrBox->getLlvmValue(), arguments);
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateReturn(
  TiObject *context, TiObject *retType, TiObject *retVal
) {
  PREPARE_ARG(context, block, Block);

  llvm::IRBuilder<> *builder = block->getIrBuilder();

  // Handle variadic cleanup (llvm.va_end)
  if (block->getFunction()->getFunctionType()->isVariadic()) {
    llvm::Module *llvmModule = this->perFunctionModules ?
      block->getFunction()->llvmModule.get() : this->buildTarget->getGlobalLlvmModule();

    llvm::Function *llvmVaEndFunc = llvmModule->getFunction("llvm.va_end");
    if (llvmVaEndFunc == nullptr) {
      // This function is in a different module, so we'll have to declare it.
      llvmVaEndFunc = llvm::Function::Create(
        this->getVaStartEndFnType(),
        llvm::Function::ExternalLinkage,
        "llvm.va_end",
        llvmModule
      );
    }

    auto int8Type = llvm::Type::getInt8Ty(*this->buildTarget->getLlvmContext());
    auto int8PtrType = int8Type->getPointerTo();

    llvm::Value *vaListPtr =
      builder->CreateBitCast(block->getFunction()->llvmVaList, int8PtrType);

    builder->CreateCall(
      this->getVaStartEndFnType(),
      llvmVaEndFunc,
      { vaListPtr }
    );
  }

  // Handle return value
  if (retVal != nullptr) {
    PREPARE_ARG(retVal, retValBox, Value);
    llvm::Value *llvmRetVal = retValBox->getLlvmValue();

    // C ABI compatibility:
    // A struct is either stored to the sret pointer (with void returned), or returned in registers.
    if (llvmRetVal->getType()->isStructTy()) {
      auto &retAbi = block->getFunction()->getFunctionType()->getRetAbi();
      if (retAbi.kind == AbiInfo::Kind::COERCED) {
        builder->CreateRet(this->lowerStructRet(builder, llvmRetVal, retAbi));
      } else if (block->getFunction()->llvmSretPtr != nullptr) {
        auto storeInst = builder->CreateStore(llvmRetVal, block->getFunction()->llvmSretPtr);
        storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(llvmRetVal->getType()));
        builder->CreateRetVoid();
      } else {
        throw EXCEPTION(GenericException,
          S("Struct return value provided but no sret pointer available."));
      }
    } else {
      builder->CreateRet(llvmRetVal);
    }
  } else {
    builder->CreateRetVoid();
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

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = builder->CreateNSWAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = builder->CreateAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFAdd(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<PointerType>()) {
    auto ptrType = static_cast<PointerType*>(tgType);
    auto elemTy = ptrType->getContentType()->getLlvmType();

    auto llvmResult = builder->CreateGEP(
      elemTy,
      srcVal1Box->getLlvmValue(),
      { srcVal2Box->getLlvmValue() }
    );
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateSub(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = builder->CreateNSWSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = builder->CreateSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFSub(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<PointerType>()) {
    auto ptrType = static_cast<PointerType*>(tgType);
    auto elemTy = ptrType->getContentType()->getLlvmType();

    auto negIndex = builder->CreateNeg(srcVal2Box->getLlvmValue());
    auto llvmResult = builder->CreateGEP(
      elemTy,
      srcVal1Box->getLlvmValue(),
      { negIndex }
    );
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateMul(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = builder->CreateNSWMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = builder->CreateMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFMul(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateDiv(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = builder->CreateSDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = builder->CreateUDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFDiv(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateRem(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    llvm::Value *llvmResult;
    if (static_cast<IntegerType*>(tgType)->isSigned()) {
      llvmResult = builder->CreateSRem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    } else {
      llvmResult = builder->CreateURem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    }
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFRem(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateShr(
  TiObject *context, TiObject *type, TiObject *srcVal1, TiObject *srcVal2, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal1, srcVal1Box, Value);
  PREPARE_ARG(srcVal2, srcVal2Box, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto builder = block->getIrBuilder();

  llvm::Value *llvmResult;
  if (tgType->isSigned()) {
    llvmResult = builder->CreateAShr(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
  } else {
    llvmResult = builder->CreateLShr(srcVal1Box->getLlvmValue(), srcVal2Box->getLlvmValue());
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

  auto llvmResult = block->getIrBuilder()->CreateShl(
    srcVal1Box->getLlvmValue(),
    srcVal2Box->getLlvmValue()
  );

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

  auto llvmResult = block->getIrBuilder()->CreateAnd(
    srcVal1Box->getLlvmValue(),
    srcVal2Box->getLlvmValue()
  );

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

  auto llvmResult = block->getIrBuilder()->CreateOr(
    srcVal1Box->getLlvmValue(),
    srcVal2Box->getLlvmValue()
  );

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

  auto llvmResult = block->getIrBuilder()->CreateXor(
    srcVal1Box->getLlvmValue(),
    srcVal2Box->getLlvmValue()
  );

  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateNot(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, IntegerType);

  auto llvmResult = block->getIrBuilder()->CreateNot(
    srcValBox->getLlvmValue()
  );

  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateNeg(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();

  if (tgType->isDerivedFrom<IntegerType>()) {
    auto llvmResult = builder->CreateNeg(srcValBox->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto llvmResult = builder->CreateFNeg(srcValBox->getLlvmValue());
    result = newSrdObj<Value>(llvmResult, false);
    return true;
  }

  throw EXCEPTION(GenericException, S("Invalid operation."));
}


Bool TargetGenerator::generateEarlyInc(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    llvmResult = integerType->isSigned()
      ? builder->CreateNSWAdd(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)))
      : builder->CreateAdd(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)));
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    llvmResult = builder->CreateFAdd(
      llvmVal,
      size == 32
        ? llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
        : llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
    );
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }

  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateEarlyDec(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    llvmResult = integerType->isSigned()
      ? builder->CreateNSWSub(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)))
      : builder->CreateSub(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)));
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    llvmResult = builder->CreateFSub(
      llvmVal,
      size == 32
        ? llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
        : llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
    );
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }

  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
  result = newSrdObj<Value>(llvmResult, false);
  return true;
}


Bool TargetGenerator::generateLateInc(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    llvmResult = integerType->isSigned()
      ? builder->CreateNSWAdd(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)))
      : builder->CreateAdd(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)));
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    llvmResult = builder->CreateFAdd(
      llvmVal,
      size == 32
        ? llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
        : llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
    );
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }

  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
  result = newSrdObj<Value>(llvmVal, false);
  return true;
}


Bool TargetGenerator::generateLateDec(
  TiObject *context, TiObject *type, TiObject *destVar, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(destVar, destVarBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  llvm::Value *llvmResult;
  if (tgType->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType*>(tgType);
    auto size = integerType->getSize();
    llvmResult = integerType->isSigned()
      ? builder->CreateNSWSub(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)))
      : builder->CreateSub(llvmVal, llvm::ConstantInt::get(*this->buildTarget->getLlvmContext(), llvm::APInt(size, 1, true)));
  } else if (tgType->isDerivedFrom<FloatType>()) {
    auto size = static_cast<FloatType*>(tgType)->getSize();
    llvmResult = builder->CreateFSub(
      llvmVal,
      size == 32
        ? llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Float)1))
        : llvm::ConstantFP::get(*this->buildTarget->getLlvmContext(), llvm::APFloat((Double)1))
    );
  } else {
    throw EXCEPTION(GenericException, S("Invalid operation."));
  }

  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
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

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  llvm::Value *llvmResult = tgType->isSigned()
    ? builder->CreateAShr(llvmVal, srcValBox->getLlvmValue())
    : builder->CreateLShr(llvmVal, srcValBox->getLlvmValue());

  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
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

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  auto llvmResult = builder->CreateShl(llvmVal, srcValBox->getLlvmValue());
  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
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

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  auto llvmResult = builder->CreateAnd(llvmVal, srcValBox->getLlvmValue());
  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
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

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  auto llvmResult = builder->CreateOr(llvmVal, srcValBox->getLlvmValue());
  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
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

  auto builder = block->getIrBuilder();
  auto llvmVal = builder->CreateLoad(tgType->getLlvmType(), destVarBox->getLlvmValue());
  llvmVal->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));

  auto llvmResult = builder->CreateXor(llvmVal, srcValBox->getLlvmValue());
  auto storeInst = builder->CreateStore(llvmResult, destVarBox->getLlvmValue());
  storeInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
  result = getSharedPtr(destVar);
  return true;
}


Bool TargetGenerator::generateNextArg(
  TiObject *context, TiObject *type, TiObject *srcVal, TioSharedPtr &result
) {
  PREPARE_ARG(context, block, Block);
  PREPARE_ARG(srcVal, srcValBox, Value);
  PREPARE_ARG(type, tgType, Type);

  auto builder = block->getIrBuilder();
  llvm::Value *llvmResult;

  if (tgType->getLlvmType()->isStructTy()) {
    auto llvmPtrType = tgType->getLlvmType()->getPointerTo();
    auto llvmPtr = builder->CreateVAArg(srcValBox->getLlvmValue(), llvmPtrType);
    auto loadInst = builder->CreateLoad(tgType->getLlvmType(), llvmPtr);
    loadInst->setAlignment(this->buildTarget->getLlvmDataLayout()->getABITypeAlign(tgType->getLlvmType()));
    llvmResult = loadInst;
  }
  else {
    llvmResult = builder->CreateVAArg(srcValBox->getLlvmValue(), tgType->getLlvmType());
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
