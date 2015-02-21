/**
 * @file Scg/Instructions/CondGotoStatement.cpp
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
#include <Instructions/CondGotoStatement.h>

using namespace llvm;

namespace Scg
{
  Expression::CodeGenerationStage CondGotoStatement::GenerateCode()
  {
    auto condition = GetCondition()->GetGeneratedLlvmValue();
    if (condition == nullptr)
      // TODO: This exception is being frequently used, with a similar statement
      // each time. A macro should be created for it to avoid duplication.
      THROW_EXCEPTION(InvalidValueException, "The condition of the conditional "
      "goto statement doesn't evaluate to a value: " + GetCondition()->ToString());

    auto irBuilder = GetBlock()->GetIRBuilder();
    this->branchInst = irBuilder->CreateCondBr(
          condition, GetTrueBlock()->GetLlvmBB(), GetFalseBlock()->GetLlvmBB());

    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage  CondGotoStatement::PostGenerateCode()
  {
    SAFE_DELETE_LLVM_INST(this->branchInst);
    return Expression::PostGenerateCode();
  }
}
