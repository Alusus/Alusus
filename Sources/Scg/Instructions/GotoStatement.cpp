/**
 * @file Scg/Instructions/GotoStatement.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Instructions/GotoStatement.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage GotoStatement::generateCode(CodeGenUnit *codeGenUnit)
{
  auto targetBlock = getTargetBlock();

  auto irBuilder = this->findOwner<Block>()->getIRBuilder();
  this->branchInst = irBuilder->CreateBr(targetBlock->getLlvmBB());

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage GotoStatement::postGenerateCode(CodeGenUnit *codeGenUnit)
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

} // namespace
