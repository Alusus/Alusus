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
#include <Operators/CastToDouble.h>
#include <Operators/CastToFloat.h>
#include <Operators/CastToInt.h>
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

  //----------------------------------------------------------------------------

  llvm::Constant *FloatType::GetLlvmConstant(float value) const
  {
    return llvm::ConstantFP::get(LlvmContainer::GetContext(),
      llvm::APFloat(value));
  }

  //----------------------------------------------------------------------------

  const ValueTypeArray &FloatType::GetImplicitCastingTargets() const
  {
    if (this->implicitCastingTargets.size() == 0)
    {
      this->implicitCastingTargets.push_back(DoubleType::GetSingleton());
      this->implicitCastingTargets.push_back(FloatType::GetSingleton());
    }
    return this->implicitCastingTargets;
  }

  //----------------------------------------------------------------------------

  const ValueTypeArray &FloatType::GetExplicitCastingTargets() const
  {
    if (this->explicitCastingTargets.size() == 0)
    {
      this->explicitCastingTargets.push_back(DoubleType::GetSingleton());
      this->explicitCastingTargets.push_back(FloatType::GetSingleton());
      this->explicitCastingTargets.push_back(IntegerType::GetSingleton());
    }
    return this->explicitCastingTargets;
  }

  //----------------------------------------------------------------------------

  CastingOperator *FloatType::GetImplicitCastingOperator(
      const ValueType *targetType, Expression *expr) const
  {
    if (targetType == DoubleType::GetSingleton()) {
      return new CastToDouble(expr);
    }
    else if (targetType == FloatType::GetSingleton()) {
      return new CastToFloat(expr);
    }
    else {
      THROW_EXCEPTION(InvalidCastException, "Integer cannot be casted to " + targetType->ToString());
    }
  }

  //----------------------------------------------------------------------------

  CastingOperator *FloatType::GetExplicitCastingOperator(
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
      THROW_EXCEPTION(InvalidCastException, "Integer cannot be casted to " + targetType->ToString());
    }
  }
}
