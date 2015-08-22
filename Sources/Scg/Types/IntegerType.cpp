/**
* @file Scg/Types/IntegerType.cpp
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

#include <llvm/IR/Constants.h>

// Scg header files
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>
#include <Types/CharType.h>

// LLVM header files

namespace Scg
{
IntegerType *IntegerType::s_singleton = nullptr;

//----------------------------------------------------------------------------

IntegerType::IntegerType() : typeSpec("int")
{
  this->llvmType = llvm::Type::getInt32Ty(LlvmContainer::getContext());

  if (s_singleton == nullptr)
    s_singleton = this;
}

void IntegerType::initCastingTargets() const
{

  this->implicitCastingTargets.push_back(DoubleType::get());
  this->implicitCastingTargets.push_back(FloatType::get());
  this->implicitCastingTargets.push_back(IntegerType::get());
  this->implicitCastingTargets.push_back(CharType::get());

  this->explicitCastingTargets.push_back(DoubleType::get());
  this->explicitCastingTargets.push_back(FloatType::get());
  this->explicitCastingTargets.push_back(IntegerType::get());
  this->explicitCastingTargets.push_back(CharType::get());
}

//----------------------------------------------------------------------------

llvm::Constant *IntegerType::getLlvmConstant(int value) const
{
  return llvm::ConstantInt::get(LlvmContainer::getContext(),
                                llvm::APInt(sizeof(value) * 8, value, true));
}

//----------------------------------------------------------------------------

llvm::Value *IntegerType::createCastInst(llvm::IRBuilder<> *irb,
    llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == IntegerType::get()) {
    // Target type is the same type, just return the input.
    return value;
  } else if (targetType == FloatType::get()) {
    // The operand is an integer, so we need to add SItoFP instruction.
    return irb->CreateSIToFP(value,
                             FloatType::get()->get()->getLlvmType());
  } else if (targetType == DoubleType::get()) {
    // The operand is an integer, so we need to add SItoFP instruction.
    return irb->CreateSIToFP(value,
                             DoubleType::get()->get()->getLlvmType());
  } else if (targetType == CharType::get()) {
    // The operand is an integer, so we need to add SItoFP instruction.
    return irb->CreateIntCast(value,
                              CharType::get()->get()->getLlvmType(), true);
  } else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}

//----------------------------------------------------------------------------

IntegerType *IntegerType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new IntegerType();
  }

  return s_singleton;
}
}
