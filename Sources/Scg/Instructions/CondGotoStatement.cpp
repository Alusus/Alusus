/**
 * @file Scg/Instructions/CondGotoStatement.cpp
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
#include <Containers/Block.h>
#include <Instructions/CondGotoStatement.h>
#include <CodeGenUnit.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage CondGotoStatement::generateCode(CodeGenUnit *codeGenUnit)
{
  auto condition = getCondition()->getGeneratedLlvmValue();

  if (condition == nullptr)
    // TODO: This exception is being frequently used, with a similar statement
    // each time. A macro should be created for it to avoid duplication.
    throw EXCEPTION(InvalidValueException, ("The condition of the conditional "
                                            "goto statement doesn't evaluate to a value: " + getCondition()->toString()).c_str());

  auto irBuilder = this->findOwner<Block>()->getIRBuilder();
  this->branchInst = irBuilder->CreateCondBr(
                       condition, getTrueBlock()->getLlvmBB(), getFalseBlock()->getLlvmBB());

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage CondGotoStatement::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  SAFE_DELETE_LLVM_INST(this->branchInst);
  return AstNode::postGenerateCode(codeGenUnit);
}

} // namespace
