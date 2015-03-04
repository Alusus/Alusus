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
#include <Operators/CastToDouble.h>
#include <Operators/CastToFloat.h>
#include <Operators/CastToInt.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

// LLVM header files

namespace Scg
{
  DoubleType *DoubleType::s_singleton = nullptr;

  DoubleType::DoubleType() : typeSpec("double")
  {
    this->llvmType = llvm::Type::getDoubleTy(LlvmContainer::GetContext());
    if (s_singleton == nullptr)
      s_singleton = this;

    this->implicitCastingTargets.push_back(DoubleType::GetSingleton());

    this->explicitCastingTargets.push_back(DoubleType::GetSingleton());
    this->explicitCastingTargets.push_back(FloatType::GetSingleton());
    this->explicitCastingTargets.push_back(IntegerType::GetSingleton());
  }

  llvm::Constant *DoubleType::GetLlvmConstant(double value) const
  {
    return llvm::ConstantFP::get(LlvmContainer::GetContext(),
      llvm::APFloat(value));
  }

  CastingOperator *DoubleType::GetImplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::GetSingleton()) {
      return new CastToDouble(expr);
    }
    else {
      throw EXCEPTION(InvalidCastException, ("Integer cannot be casted to " + targetType->ToString()).c_str());
    }
  }

  CastingOperator *DoubleType::GetExplicitCastingOperator(
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
}
