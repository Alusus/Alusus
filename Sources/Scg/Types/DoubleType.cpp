/**
* @file Scg/Types/DoubleType.cpp
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
DoubleType *DoubleType::s_singleton = nullptr;

DoubleType::DoubleType() : typeSpec("double")
{
  this->llvmType = llvm::Type::getDoubleTy(LlvmContainer::getContext());
}

void DoubleType::initCastingTargets() const
{
  this->implicitCastingTargets.push_back(DoubleType::get());

  this->explicitCastingTargets.push_back(DoubleType::get());
  this->explicitCastingTargets.push_back(FloatType::get());
  this->explicitCastingTargets.push_back(IntegerType::get());
}

llvm::Constant *DoubleType::getLlvmConstant(double value) const
{
  return llvm::ConstantFP::get(LlvmContainer::getContext(),
                               llvm::APFloat(value));
}

llvm::Value *DoubleType::createCastInst(llvm::IRBuilder<> *irb,
                                        llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == IntegerType::get()) {
    // The operand is a float, so we need to use FPTrunc instruction.
    return irb->CreateFPToSI(value,
                             IntegerType::get()->get()->getLlvmType());
  } else if (targetType == FloatType::get()) {
    // The operand is a float, so we need to use FPTrunc instruction.
    return irb->CreateFPTrunc(value,
                              FloatType::get()->get()->getLlvmType());
  } else if (targetType == DoubleType::get()) {
    // Target type is the same type, just return the input.
    return value;
  } else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}

DoubleType *DoubleType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new DoubleType();
  }

  return s_singleton;
}
}
