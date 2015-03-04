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
#include <Operators/CastToDouble.h>
#include <Operators/CastToFloat.h>
#include <Operators/CastToInt.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

// LLVM header files

namespace Scg
{
  IntegerType *IntegerType::s_singleton = nullptr;

  //----------------------------------------------------------------------------

  IntegerType::IntegerType() : typeSpec("int")
  {
    this->llvmType = llvm::Type::getInt32Ty(LlvmContainer::GetContext());
    if (s_singleton == nullptr)
      s_singleton = this;
  }

  void IntegerType::InitCastingTargets() const
  {

    this->implicitCastingTargets.push_back(DoubleType::Get());
    this->implicitCastingTargets.push_back(FloatType::Get());
    this->implicitCastingTargets.push_back(IntegerType::Get());

    this->explicitCastingTargets.push_back(DoubleType::Get());
    this->explicitCastingTargets.push_back(FloatType::Get());
    this->explicitCastingTargets.push_back(IntegerType::Get());
  }

  //----------------------------------------------------------------------------

  llvm::Constant *IntegerType::GetLlvmConstant(int value) const
  {
    return llvm::ConstantInt::get(LlvmContainer::GetContext(),
      llvm::APInt(sizeof(value) * 8, value, true));
  }

  //----------------------------------------------------------------------------

  llvm::Value *IntegerType::CreateCastInst(llvm::IRBuilder<> *irb,
    llvm::Value *value, const ValueType *targetType) const
  {
    if (targetType == IntegerType::Get()) {
      // Target type is the same type, just return the input.
      return value;
    } else if (targetType == FloatType::Get()) {
      // The operand is an integer, so we need to add SItoFP instruction.
      return irb->CreateSIToFP(value,
          FloatType::Get()->Get()->GetLlvmType());
    } else if (targetType == DoubleType::Get()) {
      // The operand is an integer, so we need to add SItoFP instruction.
      return irb->CreateSIToFP(value,
          DoubleType::Get()->Get()->GetLlvmType());
    } else {
      // TODO: Improve the message.
      throw EXCEPTION(InvalidCastException, "Invalid cast.");
    }
  }

  //----------------------------------------------------------------------------

  CastingOperator *IntegerType::GetImplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::Get()) {
      return new CastToDouble(expr);
    }
    else if (targetType == FloatType::Get()) {
      return new CastToFloat(expr);
    }
    else if (targetType == IntegerType::Get()) {
      return new CastToInt(expr);
    }
    else {
      throw EXCEPTION(InvalidCastException,
          ("Integer cannot be casted to " + targetType->ToString()).c_str());
    }
  }

  //----------------------------------------------------------------------------

  CastingOperator *IntegerType::GetExplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::Get()) {
      return new CastToDouble(expr);
    }
    else if (targetType == FloatType::Get()) {
      return new CastToFloat(expr);
    }
    else if (targetType == IntegerType::Get()) {
      return new CastToInt(expr);
    }
    else {
      throw EXCEPTION(InvalidCastException,
          (std::string("Integer cannot be casted to ") + targetType->ToString()).c_str());
    }
  }
}
