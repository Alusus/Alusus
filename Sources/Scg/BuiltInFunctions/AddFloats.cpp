/**
 * @file Scg/BuiltInFunctions/AddFloats.cpp
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

// Scg files
#include <BuiltInFunctions/AddFloats.h>
#include <Containers/Block.h>
#include <Types/FloatType.h>

namespace Scg
{
CodeGenerationResult AddFloats::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  // TODO: Should we throw an InvalidArgumentException and handle it
  // somewhere and throw a CompilationErrorException?
  if (GetLHS()->GetValueType() != FloatType::GetSingleton()) {
    THROW_EXCEPTION(CompilationErrorException, "Left hand-side should be a float.");
  }
  if (GetRHS()->GetValueType() != FloatType::GetSingleton()) {
    THROW_EXCEPTION(CompilationErrorException, "Right hand-side should be a float.");
  }

  auto leftValue = GetLHS()->GenerateCode().exprValue;
  auto rightValue = GetRHS()->GenerateCode().exprValue;

  PrintLlvmValue(leftValue);
  PrintLlvmValue(rightValue);

  this->llvmValue = irb->CreateFAdd(leftValue, rightValue);

  return CodeGenerationResult(this->llvmValue);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage AddFloats::PostGenerateCode()
{
  if (!this->llvmValue->hasNUses(0)) {
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;
  }
  // TODO: Is it possible anymore for llvmValue to be of Instruction type?
  auto asInst = llvm::dyn_cast<llvm::Instruction>(this->llvmValue);
  if (asInst != nullptr) {
    asInst->eraseFromParent();
  } else {
    delete this->llvmValue;
  }
  this->llvmValue = nullptr;

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string AddFloats::ToString()
{
  return GetLHS()->ToString() + " + " + GetRHS()->ToString();
}
}

