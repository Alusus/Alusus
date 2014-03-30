/**
 * @file Scg/Operators/AssignmentOperator.cpp
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
#include <Operators/AssignmentOperator.h>

using namespace llvm;

namespace Scg
{
CodeGenerationResult AssignmentOperator::GenerateCode()
{
  BLOCK_CHECK;

  auto irb = GetBlock()->GetIRBuilder();

  // TODO: Don't use dynamic_cast.
  auto lhs = dynamic_cast<Content*>(GetLHS());
  if (lhs == nullptr)
  THROW_EXCEPTION(InvalidOperationException, "The left-hand side of an "
      "assignment must be the content of a pointer.");

  // Find the value of the right-hand side expression.
  // TODO: Shouldn't the value be deleted?
  lhs->GenerateCode();
  auto value = GetRHS()->GenerateCode().exprValue;
  if (value == 0)
    THROW_EXCEPTION(InvalidValueException,
        "Right-hand side of '=' doesn't evaluate to a value: "
            + GetRHS()->ToString());

  // Add a store instruction to set the value of the variable.
  this->llvmStoreInst = irb->CreateStore(value, lhs->GetLlvmPointer());

  // Return right-hand side.
  return CodeGenerationResult(value);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage AssignmentOperator::PostGenerateCode()
{
  if (!this->llvmStoreInst->hasNUses(0)) {
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;
  }
  this->llvmStoreInst->eraseFromParent();
  this->llvmStoreInst = nullptr;

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string AssignmentOperator::ToString()
{
  return GetLHS()->ToString() + " := " + GetRHS()->ToString();
}
}
