/**
* @file Scg/Types/CharType.cpp
*
* @copyright Copyright (C) 2016 Hicham OUALI ALAMI
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
#include <Types/IntegerType.h>

// LLVM header files

namespace Scg
{

//==============================================================================
// Static Variables

CharType *CharType::s_singleton = nullptr;



//==============================================================================
// Constructors & Destructor

CharType::CharType()
{
  this->typeSpec = std::make_shared<ValueTypeSpecByName>(STR("char"));

  this->llvmType = llvm::Type::getInt8Ty(LlvmContainer::getContext());

  if (s_singleton == nullptr)
    s_singleton = this;
}


//==============================================================================
// Member Functions

void CharType::initCastingTargets() const
{
  this->implicitCastingTargets.push_back(CharType::get());
  this->implicitCastingTargets.push_back(IntegerType::get());

  this->explicitCastingTargets.push_back(CharType::get());
  this->explicitCastingTargets.push_back(IntegerType::get());
}


llvm::Constant* CharType::getLlvmConstant(char value) const
{
  return llvm::ConstantInt::get(LlvmContainer::getContext(),
                                llvm::APInt(sizeof(value) * 8, value, true));
}


llvm::Value* CharType::createCastInst(llvm::IRBuilder<> *irb,
                                      llvm::Value *value, const ValueType *targetType) const
{
  if (targetType == CharType::get()) {
    // Target type is the same type, just return the input.
    return value;
  } else if (targetType == IntegerType::get()) {
    // The operand is an integer, so we need to add SItoFP instruction.
    return irb->CreateIntCast(value,
                              IntegerType::get()->get()->getLlvmType(), true);
  } else {
    // TODO: Improve the message.
    throw EXCEPTION(InvalidCastException, "Invalid cast.");
  }
}


CharType *CharType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new CharType();
  }

  return s_singleton;
}

} // namespace
