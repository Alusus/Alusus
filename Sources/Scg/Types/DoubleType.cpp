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
  }

  void DoubleType::InitCastingTargets() const
  {
    this->implicitCastingTargets.push_back(DoubleType::Get());

    this->explicitCastingTargets.push_back(DoubleType::Get());
    this->explicitCastingTargets.push_back(FloatType::Get());
    this->explicitCastingTargets.push_back(IntegerType::Get());
  }

  llvm::Constant *DoubleType::GetLlvmConstant(double value) const
  {
    return llvm::ConstantFP::get(LlvmContainer::GetContext(),
      llvm::APFloat(value));
  }

  CastingOperator *DoubleType::GetImplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::Get()) {
      return new CastToDouble(expr);
    }
    else {
      throw EXCEPTION(InvalidCastException, ("Integer cannot be casted to " + targetType->ToString()).c_str());
    }
  }

  CastingOperator *DoubleType::GetExplicitCastingOperator(
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

  DoubleType *DoubleType::Get()
  {
    // PERFORMANCE: What is the impact of running an unnecessary if statement
    // thousands of times?
    if (s_singleton == nullptr) {
      s_singleton = new DoubleType();
    }
    return s_singleton;
  }
}
