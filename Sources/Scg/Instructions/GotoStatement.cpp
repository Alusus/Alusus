/**
 * @file Scg/Instructions/GotoStatement.cpp
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
#include <Instructions/GotoStatement.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{
  CodeGenerationResult GotoStatement::GenerateCode()
  {
    auto targetBlock = GetTargetBlock();

    auto irBuilder = GetBlock()->GetIRBuilder();
    this->branchInst = irBuilder->CreateBr(targetBlock->GetLlvmBB());

    // Goto statement keyword doesn't evaluate to a value.
    return CodeGenerationResult(0, false);
  }

  //------------------------------------------------------------------------------------------------

  Expression::CodeGenerationStage GotoStatement::PostGenerateCode()
  {
    if (this->branchInst == nullptr)
      // Nothing to do!
      return CodeGenerationStage::None;
    if (!this->branchInst->hasNUses(0))
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;
    // Delete the branch instruction.
    this->branchInst->eraseFromParent();
    this->branchInst = nullptr;
    return CodeGenerationStage::None;
  }
}
