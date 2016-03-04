/**
 * @file Scg/Instructions/IfStatement.cpp
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
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/IfStatement.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{

//==============================================================================
// Constructors & Destructor

IfStatement::IfStatement(SharedPtr<AstNode> const &condition,
                         SharedPtr<Block> const &thenBlock,
                         SharedPtr<Block> const &elseBlock)
  : condition(condition)
  , thenBlock(thenBlock)
  , elseBlock(elseBlock)
  , cmpInst(0)
  , ifBranch(0)
  , thenBranch(0)
  , elseBranch(0)
{
  // Create an empty block to serve as a label for branching from either of
  // the 'then' or 'else' parts.
  this->mergeBlock = std::make_shared<Block>();

  if (thenBlock == 0)
    throw EXCEPTION(ArgumentOutOfRangeException, "If statements must "
                    "have a 'then' block.");

  OWN_SHAREDPTR(this->condition);
  OWN_SHAREDPTR(this->thenBlock);
  OWN_SHAREDPTR(this->elseBlock);
  OWN_SHAREDPTR(this->mergeBlock);
}

IfStatement::~IfStatement()
{
  DISOWN_SHAREDPTR(this->condition);
  DISOWN_SHAREDPTR(this->thenBlock);
  DISOWN_SHAREDPTR(this->elseBlock);
  DISOWN_SHAREDPTR(this->mergeBlock);
}


//==============================================================================
// Member Functions

AstNode::CodeGenerationStage IfStatement::generateCode(CodeGenUnit *codeGenUnit)
{
  BLOCK_CHECK;

  auto irBuilder = this->findOwner<Block>()->getIRBuilder();

  // Evaluate the condition to a boolean.
  // TODO: 'condition' should be freed in postGenerateCode().
  if (this->condition->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->condition->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  auto condition = this->condition->getGeneratedLlvmValue();

  if (condition == nullptr)
    // TODO: This exception is being frequently used, with a similar statement each time. A macro
    // should be created for it to avoid duplication.
    throw EXCEPTION(InvalidValueException, ("The condition of the if statement "
                                            "doesn't evaluate to a value: " + this->condition->toString()).c_str());

  this->cmpInst = static_cast<llvm::ICmpInst*>(irBuilder->CreateICmpNE(condition,
                  ConstantInt::get(condition->getType(), 0), "ifcond"));

  this->ifBranch = irBuilder->CreateCondBr(condition, this->thenBlock->getLlvmBB(),
                   this->elseBlock != 0 ? this->elseBlock->getLlvmBB() : this->mergeBlock->getLlvmBB());

  // Generate the code of the 'then', 'else', and merge parts.
  if (this->thenBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->thenBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  if (this->elseBlock != nullptr) {
    if (this->elseBlock->getCodeGenerationStage() ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      if (this->elseBlock->callGenerateCode(codeGenUnit) ==
          AstNode::CodeGenerationStage::CodeGeneration) {
        return AstNode::CodeGenerationStage::CodeGeneration;
      }
    }
  }

  if (this->mergeBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->mergeBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  // Add branch instructions to the end of the 'then' and 'else' parts.
  if (!this->thenBlock->isTermInstGenerated())
    this->thenBranch = this->thenBlock->getIRBuilder()->CreateBr(this->mergeBlock->getLlvmBB());

  if (this->elseBlock != nullptr && !this->elseBlock->isTermInstGenerated())
    this->elseBranch = this->elseBlock->getIRBuilder()->CreateBr(this->mergeBlock->getLlvmBB());

  irBuilder->SetInsertPoint(this->mergeBlock->getLlvmBB());

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage IfStatement::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  SAFE_DELETE_LLVM_INST(this->cmpInst);
  SAFE_DELETE_LLVM_INST(this->ifBranch);
  SAFE_DELETE_LLVM_INST(this->thenBranch);
  SAFE_DELETE_LLVM_INST(this->elseBranch);
  return CodeGenerationStage::None;
}


std::string IfStatement::toString()
{
  std::string str;
  str.append("if (" + this->condition->toString() + ")\n");
  str.append(this->thenBlock->toString());

  if (this->elseBlock != 0) {
    str.append("else\n");
    str.append(this->elseBlock->toString());
  }

  return str;
}

} // namespace
