/**
 * @file Scg/Operators/CastToDouble.cpp
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
#include <Operators/CastToDouble.h>
#include <Operators/Content.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

namespace Scg
{
const ValueType *CastToDouble::GetValueType() const
{
  return FloatType::GetSingleton();
}

//------------------------------------------------------------------------------

CodeGenerationResult CastToDouble::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  // TODO: This need to be updated when other standard data types are supported.
  if (this->GetOperand()->GetValueType() == IntegerType::GetSingleton()) {
    // The operand is an integer, so we need to add SItoFP instruction.
    this->llvmCastInst = static_cast<llvm::CastInst*>(irb->CreateSIToFP(
        GetOperand()->GenerateCode().exprValue,
        DoubleType::GetSingleton()->GetSingleton()->GetLlvmType()));
    return CodeGenerationResult(this->llvmCastInst);
  } else if (this->GetOperand()->GetValueType() == FloatType::GetSingleton()) {
    // The operand is a float, so we need to add FPExt instruction.
    this->llvmCastInst = static_cast<llvm::CastInst*>(irb->CreateFPExt(
        GetOperand()->GenerateCode().exprValue,
        DoubleType::GetSingleton()->GetSingleton()->GetLlvmType()));
    return CodeGenerationResult(this->llvmCastInst);
  } else if (this->GetOperand()->GetValueType() == DoubleType::GetSingleton()) {
    // The operand is already a double, no need to cast.
    return CodeGenerationResult(GetOperand()->GenerateCode().exprValue);
  } else {
    THROW_EXCEPTION(ArgumentOutOfRangeException,
        "The operand of CastToDouble operator should be integer, float, or double.");
  }
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CastToDouble::PostGenerateCode()
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

std::string CastToDouble::ToString()
{
  return GetOperand()->ToString() + "~cast[double]";
}
}
