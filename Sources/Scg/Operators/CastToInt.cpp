/**
 * @file Scg/Operators/CastToInt.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
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
#include <Operators/CastToInt.h>
#include <Operators/Content.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

namespace Scg
{
const ValueType *CastToInt::GetValueType() const
{
  return IntegerType::GetSingleton();
}

//------------------------------------------------------------------------------

CodeGenerationResult CastToInt::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  // TODO: This need to be updated when other standard data types are supported.
  if (this->GetOperand()->GetValueType() == IntegerType::GetSingleton()) {
    // The operand is already an integer, no need to cast.
    return CodeGenerationResult(GetOperand()->GenerateCode().exprValue);
  } else if (this->GetOperand()->GetValueType() == FloatType::GetSingleton() ||
      this->GetOperand()->GetValueType() == DoubleType::GetSingleton()) {
    // The operand is integer or float, so we need to add FPToSI instruction.
    this->llvmCastInst = static_cast<llvm::CastInst*>(irb->CreateFPToSI(
        GetOperand()->GenerateCode().exprValue,
        IntegerType::GetSingleton()->GetSingleton()->GetLlvmType()));
    return CodeGenerationResult(this->llvmCastInst);
  } else {
    THROW_EXCEPTION(ArgumentOutOfRangeException,
        "The operand of CastToInt operator should be integer, float, or double.");
  }
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CastToInt::PostGenerateCode()
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

std::string CastToInt::ToString()
{
  return GetOperand()->ToString() + "~cast[int]";
}
}
