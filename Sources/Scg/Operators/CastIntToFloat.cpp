/**
 * @file Scg/Operators/CastIntToFloat.cpp
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
#include <Operators/CastIntToFloat.h>
#include <Operators/Content.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

namespace Scg
{
const ValueType *CastIntToFloat::GetValueType() const
{
  return FloatType::GetSingleton();
}

//------------------------------------------------------------------------------

CodeGenerationResult CastIntToFloat::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  if (this->GetOperand()->GetValueType() != IntegerType::GetSingleton()) {
    THROW_EXCEPTION(ArgumentOutOfRangeException,
        "The operand of a CastIntToFloat operator should be integer.");
  }

  // Add a store instruction to set the value of the variable.
  this->llvmCastInst = static_cast<llvm::CastInst*>(irb->CreateSIToFP(
      GetOperand()->GenerateCode().exprValue,
      FloatType::GetSingleton()->GetSingleton()->GetLlvmType()));

  // Return right-hand side.
  return CodeGenerationResult(this->llvmCastInst);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CastIntToFloat::PostGenerateCode()
{
  if (!this->llvmCastInst->hasNUses(0)) {
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;
  }
  this->llvmCastInst->eraseFromParent();
  this->llvmCastInst = nullptr;

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string CastIntToFloat::ToString()
{
  return GetOperand()->ToString() + "~cast[float]";
}
}
