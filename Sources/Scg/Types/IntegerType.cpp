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

    this->implicitCastingTargets.push_back(DoubleType::GetSingleton());
    this->implicitCastingTargets.push_back(FloatType::GetSingleton());
    this->implicitCastingTargets.push_back(IntegerType::GetSingleton());

    this->explicitCastingTargets.push_back(DoubleType::GetSingleton());
    this->explicitCastingTargets.push_back(FloatType::GetSingleton());
    this->explicitCastingTargets.push_back(IntegerType::GetSingleton());
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
    if (targetType == IntegerType::GetSingleton()) {
      // Target type is the same type, just return the input.
      return value;
    } else if (targetType == FloatType::GetSingleton()) {
      // The operand is an integer, so we need to add SItoFP instruction.
      return irb->CreateSIToFP(value,
          FloatType::GetSingleton()->GetSingleton()->GetLlvmType());
    } else if (targetType == DoubleType::GetSingleton()) {
      // The operand is an integer, so we need to add SItoFP instruction.
      return irb->CreateSIToFP(value,
          DoubleType::GetSingleton()->GetSingleton()->GetLlvmType());
    } else {
      // TODO: Improve the message.
      throw EXCEPTION(InvalidCastException, "Invalid cast.");
    }
  }

  //----------------------------------------------------------------------------

  CastingOperator *IntegerType::GetImplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::GetSingleton()) {
      return new CastToDouble(expr);
    }
    else if (targetType == FloatType::GetSingleton()) {
      return new CastToFloat(expr);
    }
    else if (targetType == IntegerType::GetSingleton()) {
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
    if (targetType == DoubleType::GetSingleton()) {
      return new CastToDouble(expr);
    }
    else if (targetType == FloatType::GetSingleton()) {
      return new CastToFloat(expr);
    }
    else if (targetType == IntegerType::GetSingleton()) {
      return new CastToInt(expr);
    }
    else {
      throw EXCEPTION(InvalidCastException,
          (std::string("Integer cannot be casted to ") + targetType->ToString()).c_str());
    }
  }
}
