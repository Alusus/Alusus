/**
* @file Scg/Types/FloatType.cpp
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

// LLVM header files

namespace Scg
{
FloatType *FloatType::s_singleton = nullptr;

//----------------------------------------------------------------------------

FloatType::FloatType() : typeSpec("float")
{
  this->llvmType = llvm::Type::getFloatTy(LlvmContainer::getContext());

  if (s_singleton == nullptr)
    s_singleton = this;
}

void FloatType::initCastingTargets() const
{

  this->implicitCastingTargets.push_back(DoubleType::get());
  this->implicitCastingTargets.push_back(FloatType::get());

  this->explicitCastingTargets.push_back(DoubleType::get());
  this->explicitCastingTargets.push_back(FloatType::get());
  this->explicitCastingTargets.push_back(IntegerType::get());
}

//----------------------------------------------------------------------------

llvm::Constant *FloatType::getLlvmConstant(float value) const
{
  return llvm::ConstantFP::get(LlvmContainer::getContext(),
                               llvm::APFloat(value));
}

llvm::Value *FloatType::createCastInst(llvm::IRBuilder<> *irb,
                                       llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == IntegerType::get()) {
    // The operand is a float, so we need to use FPTrunc instruction.
    return irb->CreateFPToSI(value,
                             IntegerType::get()->get()->getLlvmType());
  } else if (targetType == FloatType::get()) {
    // Target type is the same type, just return the input.
    return value;
  } else if (targetType == DoubleType::get()) {
    // The operand is a double, so we need to use FPExt instruction.
    return irb->CreateFPExt(value,
                            DoubleType::get()->get()->getLlvmType());
  } else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}

FloatType *FloatType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new FloatType();
  }

  return s_singleton;
}
}
