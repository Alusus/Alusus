/**
 * @file Scg/Instructions/ForStatement.cpp
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
#include <Instructions/ForStatement.h>
#include <Instructions/GotoStatement.h>

using namespace llvm;

namespace Scg
{

//==============================================================================
// Constructors & Destructor

ForStatement::ForStatement(SharedPtr<AstNode> const &init,
                           SharedPtr<AstNode> const &cond,
                           SharedPtr<AstNode> const &loop,
                           SharedPtr<Block> const &body)
  : init(init.get())
  , cond(cond.get())
  , loop(loop.get())
  , brInst(nullptr)
{
  if (init == 0 && cond == 0 && loop == 0 && body == 0)
    throw EXCEPTION(InfiniteLoopException, "This loop is infinite.");

  // Prepare the blocks of the for loop.
  this->initBlock = this->init != 0 ? Block::create({ init }) : std::make_shared<Block>();
  this->initBlock->setOwner(this);
  // At the end of the loop, we need to go back to the condition block.
  this->loopBlock = body;
  this->loopBlock->setOwner(this);
  this->exitBlock = std::make_shared<Block>();
  this->exitBlock->setOwner(this);
  this->condBlock = cond != 0
    ? Block::create({ std::make_shared<CondGotoStatement>(cond,
                                                          this->loopBlock.get(),
                                                          this->exitBlock.get()) })
    : Block::create({ std::make_shared<GotoStatement>(this->loopBlock.get()) });
  this->condBlock->setOwner(this);
  this->loopBlock->appendNode(loop);
  this->loopBlock->appendNode(std::make_shared<GotoStatement>(this->condBlock.get()));
  this->initBlock->appendNode(std::make_shared<GotoStatement>(this->condBlock.get()));
}


ForStatement::~ForStatement()
{
  DISOWN_SHAREDPTR(this->initBlock);
  DISOWN_SHAREDPTR(this->loopBlock);
  DISOWN_SHAREDPTR(this->exitBlock);
  DISOWN_SHAREDPTR(this->condBlock);
}


//==============================================================================
// Member Functions

AstNode::CodeGenerationStage ForStatement::generateCode(CodeGenUnit *codeGenUnit)
{
  auto irBuilder = this->findOwner<Block>()->getIRBuilder();

  if (this->brInst == nullptr) {
    this->brInst = irBuilder->CreateBr(this->initBlock->getLlvmBB());
  }

  // Generate the code of the initial condition.
  if (this->initBlock->getCodeGenerationStage() ==
      AstNode::CodeGenerationStage::CodeGeneration) {
    if (this->initBlock->callGenerateCode(codeGenUnit) ==
        AstNode::CodeGenerationStage::CodeGeneration) {
      return AstNode::CodeGenerationStage::CodeGeneration;
    }
  }

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


AstNode::CodeGenerationStage ForStatement::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (this->brInst != 0) {
    if (!this->brInst->hasNUses(0))
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;

    this->brInst->eraseFromParent();
    this->brInst = 0;
  }

  return CodeGenerationStage::None;
}


std::string ForStatement::toString()
{
  std::string str;
  str.append("for (");

  if (this->init != 0) str.append(this->init->toString());

  str.append(";");

  if (this->cond != 0) str.append(this->cond->toString());

  str.append(";");

  if (this->loop != 0) str.append(this->loop->toString());

  str.append(")\n");
  str.append(this->findOwner<Block>()->toString());
  return str;
}

} // namespace
