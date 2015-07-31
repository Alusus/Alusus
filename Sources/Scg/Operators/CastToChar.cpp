/**
 * @file Scg/Operators/CastToChar.cpp
 *
 * @copyright Copyright (C) 2015 Hicham OUALI ALAMI
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// SCG files
#include <Containers/Block.h>
#include <Operators/CastToChar.h>
#include <Operators/Content.h>
#include <Types/CharType.h>
#include <Types/IntegerType.h>

namespace Scg
{
const ValueTypeSpec * CastToChar::GetValueTypeSpec() const
{
  return CharType::Get()->GetValueTypeSpec();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CastToChar::GenerateCode()
{
  BLOCK_CHECK;

  auto module = GetModule();
  //auto irb = GetBlock()->GetIRBuilder();

  // TODO: This need to be updated when other standard data types are supported.
  // TODO: Add the following helper methods in ValueTypeSpec (and ValueType?)
  // IsInteger
  // IsFloat
  // IsDouble
  // IsString
  // IsVoid
  // Anything else?
  if (this->GetOperand()->GetValueTypeSpec()->ToValueType(*module) == CharType::Get() ||
      this->GetOperand()->GetValueTypeSpec()->ToValueType(*module) == IntegerType::Get()) {
    // The operand is already an integer, no need to cast.
    this->generatedLlvmValue = GetOperand()->GetGeneratedLlvmValue();
  } else {
    throw EXCEPTION(ArgumentOutOfRangeException,
        "The operand of CastToInt operator should be integer, float, or double.");
  }

  return Expression::GenerateCode();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CastToChar::PostGenerateCode()
{
  if (this->llvmCastInst != nullptr) {
    if (!this->llvmCastInst->hasNUses(0)) {
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;
    }
    this->llvmCastInst->eraseFromParent();
    this->llvmCastInst = nullptr;
  }

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string CastToChar::ToString()
{
  return GetOperand()->ToString() + "~cast[char]";
}
}
