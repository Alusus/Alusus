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
  this->llvmType = llvm::Type::getFloatTy(LlvmContainer::GetContext());
  if (s_singleton == nullptr)
    s_singleton = this;
}

void FloatType::InitCastingTargets() const
{

  this->implicitCastingTargets.push_back(DoubleType::Get());
  this->implicitCastingTargets.push_back(FloatType::Get());

  this->explicitCastingTargets.push_back(DoubleType::Get());
  this->explicitCastingTargets.push_back(FloatType::Get());
  this->explicitCastingTargets.push_back(IntegerType::Get());
}

//----------------------------------------------------------------------------

llvm::Constant *FloatType::GetLlvmConstant(float value) const
{
  return llvm::ConstantFP::get(LlvmContainer::GetContext(),
      llvm::APFloat(value));
}

llvm::Value *FloatType::CreateCastInst(llvm::IRBuilder<> *irb,
    llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == IntegerType::Get()) {
    // The operand is a float, so we need to use FPTrunc instruction.
    return irb->CreateFPToSI(value,
        IntegerType::Get()->Get()->GetLlvmType());
  } else if (targetType == FloatType::Get()) {
    // Target type is the same type, just return the input.
    return value;
  } else if (targetType == DoubleType::Get()) {
    // The operand is a double, so we need to use FPExt instruction.
    return irb->CreateFPExt(value,
        DoubleType::Get()->Get()->GetLlvmType());
  } else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}

FloatType *FloatType::Get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new FloatType();
  }
  return s_singleton;
}
}
