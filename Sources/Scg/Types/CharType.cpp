/**
* @file Scg/Types/CharType.cpp
*
* @copyright Copyright (C) 2014 Hicham OUALI ALAMI
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
#include <Types/CharType.h>
#include <Operators/CastToInt.h>
#include <Types/IntegerType.h>
#include <Operators/CastToChar.h>

// LLVM header files

namespace Scg
{
CharType *CharType::s_singleton = nullptr;

//----------------------------------------------------------------------------

CharType::CharType() : typeSpec("char")
{
  this->llvmType = llvm::Type::getInt8Ty(LlvmContainer::GetContext());
  if (s_singleton == nullptr)
    s_singleton = this;
}

void CharType::InitCastingTargets() const
{
  this->implicitCastingTargets.push_back(CharType::Get());
  this->implicitCastingTargets.push_back(IntegerType::Get());

  this->explicitCastingTargets.push_back(CharType::Get());
  this->explicitCastingTargets.push_back(IntegerType::Get());
}

//----------------------------------------------------------------------------

llvm::Constant *CharType::GetLlvmConstant(char value) const
{
  return llvm::ConstantInt::get(LlvmContainer::GetContext(),
      llvm::APInt(sizeof(value) * 8, value, true));
}

//----------------------------------------------------------------------------

llvm::Value *CharType::CreateCastInst(llvm::IRBuilder<> *irb,
    llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == CharType::Get()) {
    // Target type is the same type, just return the input.
    return value;
  }
  else if (targetType == IntegerType::Get()) {
        // The operand is an integer, so we need to add SItoFP instruction.
        return irb->CreateIntCast(value,
            IntegerType::Get()->Get()->GetLlvmType(), true);
  }
  else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}

//----------------------------------------------------------------------------

CastingOperator *CharType::GetImplicitCastingOperator(
    const ValueType *targetType, Expression *expr) const
{
  if (targetType == CharType::Get()) {
    return new CastToChar(expr);
  }
  else if (targetType == IntegerType::Get()) {
    return new CastToInt(expr);
  }
  else {
    throw EXCEPTION(InvalidCastException,
        ("Char cannot be casted to " + targetType->ToString()).c_str());
  }
}

//----------------------------------------------------------------------------

CastingOperator *CharType::GetExplicitCastingOperator(
    const ValueType *targetType, Expression *expr) const
{
  if (targetType == CharType::Get()) {
    return new CastToChar(expr);
  }
  else {
    throw EXCEPTION(InvalidCastException,
        (std::string("Char cannot be casted to ") + targetType->ToString()).c_str());
  }
}

CharType *CharType::Get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new CharType();
  }
  return s_singleton;
}
}
