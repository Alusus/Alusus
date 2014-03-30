/**
 * @file Scg/Operators/BinaryOperator.cpp
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
#include <Containers/Block.h>
#include <Operators/Content.h>
#include <Operators/BinaryOperator.h>
#include <Types/IntegerType.h>

using namespace llvm;

namespace Scg
{
const ValueType *BinaryOperator::GetValueType() const
{
  return IntegerType::GetSingleton();
}

//------------------------------------------------------------------------------

CodeGenerationResult BinaryOperator::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  auto leftValue = GetLHS()->GenerateCode().exprValue;
  auto rightValue = GetRHS()->GenerateCode().exprValue;
  if (leftValue == nullptr || rightValue == nullptr)
    // TODO: Shouldn't we throw an exception here?
    return CodeGenerationResult();

  switch (this->opType)
  {
  case ADD:
    this->llvmValue = irb->CreateAdd(leftValue, rightValue);
    break;
  case SUBTRACT:
    this->llvmValue = irb->CreateSub(leftValue, rightValue);
    break;
  case MULTIPLY:
    this->llvmValue = irb->CreateMul(leftValue, rightValue);
    break;
  case DIVISION:
    this->llvmValue = irb->CreateSDiv(leftValue, rightValue);
    break;
  case GREATERTHAN:
    this->llvmValue = irb->CreateICmpSGT(leftValue, rightValue);
    break;
  case GREATERTHAN_EQUAL:
    this->llvmValue = irb->CreateICmpSGE(leftValue, rightValue);
    break;
  case LESSTHAN:
    this->llvmValue = irb->CreateICmpSLT(leftValue, rightValue);
    break;
  case LESSTHAN_EQUAL:
    this->llvmValue = irb->CreateICmpSLE(leftValue, rightValue);
    break;
  case EQUAL:
    this->llvmValue = irb->CreateICmpEQ(leftValue, rightValue);
    break;
  case NOTEQUAL:
    this->llvmValue = irb->CreateICmpNE(leftValue, rightValue);
    break;
  default:
    THROW_EXCEPTION(UnreachableCodeException, "This code shouldn't be reached.");
  }

  return CodeGenerationResult(this->llvmValue);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage BinaryOperator::PostGenerateCode()
{
  if (!this->llvmValue->hasNUses(0))
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;
  // TODO: Is it possible anymore for this->llvmValue to be of Instruction type?
  auto asInst = dyn_cast<llvm::Instruction>(this->llvmValue);
  if (asInst != nullptr)
    asInst->eraseFromParent();
  else
    delete this->llvmValue;
  this->llvmValue = nullptr;

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string BinaryOperator::ToString()
{
  switch (this->opType)
  {
  case ADD:
    return GetLHS()->ToString() + " + " + GetRHS()->ToString();
  case SUBTRACT:
    return GetLHS()->ToString() + " - " + GetRHS()->ToString();
  case MULTIPLY:
    return GetLHS()->ToString() + " * " + GetRHS()->ToString();
  case DIVISION:
    return GetLHS()->ToString() + " / " + GetRHS()->ToString();
  case GREATERTHAN:
    return GetLHS()->ToString() + " > " + GetRHS()->ToString();
  case GREATERTHAN_EQUAL:
    return GetLHS()->ToString() + " >= " + GetRHS()->ToString();
  case LESSTHAN:
    return GetLHS()->ToString() + " < " + GetRHS()->ToString();
  case LESSTHAN_EQUAL:
    return GetLHS()->ToString() + " <= " + GetRHS()->ToString();
  case EQUAL:
    return GetLHS()->ToString() + " == " + GetRHS()->ToString();
  case NOTEQUAL:
    return GetLHS()->ToString() + " != " + GetRHS()->ToString();
  default:
    return "";
  }
}
}
