/**
 * @file Scg/Instructions/WhileStatement.cpp
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
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/CondGotoStatement.h>
#include <Instructions/GotoStatement.h>
#include <Instructions/WhileStatement.h>

using namespace llvm;

namespace Scg
{

//==============================================================================
// Constructors & Destructor

WhileStatement::WhileStatement(SharedPtr<AstNode> const &cond, SharedPtr<Block> const &body)
  : cond(cond.get())
  , brInst(0)
{
  if (cond == nullptr && body == nullptr)
    throw EXCEPTION(InfiniteLoopException, "This loop is infinite.");

  this->loopBlock = body;
  this->loopBlock->setOwner(this);

  // Create a block which is used to exit the loop if the condition is not
  // met.
  this->exitBlock = std::make_shared<Block>();
  this->exitBlock->setOwner(this);
  this->condBlock = this->cond != nullptr
    ? Block::create({ std::make_shared<CondGotoStatement>(cond,
                                                          this->loopBlock.get(),
                                                          this->exitBlock.get()) })
    : Block::create({ std::make_shared<GotoStatement>(this->loopBlock.get()) });
  this->condBlock->setOwner(this);

  // Add an expression at the end of the loop block to go back to the
  // block checking the condition.
  this->loopBlock->appendNode(std::make_shared<GotoStatement>(this->condBlock.get()));
}


WhileStatement::~WhileStatement()
{
  DISOWN_SHAREDPTR(this->loopBlock);
  DISOWN_SHAREDPTR(this->exitBlock);
  DISOWN_SHAREDPTR(this->condBlock);
}


//==============================================================================
// Member Functions

AstNode::CodeGenerationStage WhileStatement::generateCode(CodeGenUnit *codeGenUnit)
{
  BLOCK_CHECK;

  auto irBuilder = this->findOwner<Block>()->getIRBuilder();

  this->brInst = irBuilder->CreateBr(this->condBlock->getLlvmBB());

  // Generate the code of the condition of the for condition.
  if (this->condBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->condBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  // Generate the code of the loop code.
  if (this->loopBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->loopBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  // Generate the code of the exit block.
  if (this->exitBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->exitBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

  irBuilder->SetInsertPoint(this->exitBlock->getLlvmBB());

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage WhileStatement::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  SAFE_DELETE_LLVM_INST(this->brInst);
  return CodeGenerationStage::None;
}


std::string WhileStatement::toString()
{
  std::string str;
  str.append("while (");

  if (this->cond != 0) str.append(this->cond->toString());

  str.append(")\n");
  str.append(this->findOwner<Block>()->toString());
  return str;
}

} // namespace
