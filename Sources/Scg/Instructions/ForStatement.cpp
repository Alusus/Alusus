/**
 * @file Scg/Instructions/ForStatement.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/CondGotoStatement.h>
#include <Instructions/ForStatement.h>
#include <Instructions/GotoStatement.h>

using namespace llvm;

namespace Scg
{
ForStatement::ForStatement(Expression *init, Expression *cond, Expression *loop, Block *body)
  : init(init)
  , cond(cond)
  , loop(loop)
  , brInst(nullptr)
{
  if (init == 0 && cond == 0 && loop == 0 && body == 0)
    throw EXCEPTION(InfiniteLoopException, "This loop is infinite.");

  // Prepare the blocks of the for loop.
  this->initBlock = this->init != 0 ? new Block({ this->init }) : new Block();
  // At the end of the loop, we need to go back to the condition block.
  this->loopBlock = body;
  this->exitBlock = new Block();
  this->condBlock = this->cond != 0
  ? new Block({ new CondGotoStatement(this->cond, this->loopBlock, this->exitBlock) })
    : new Block({ new GotoStatement(this->loopBlock) });
  this->loopBlock->appendExpression(loop);
  this->loopBlock->appendExpression(new GotoStatement(this->condBlock));
  this->initBlock->appendExpression(new GotoStatement(this->condBlock));

  this->children.push_back(this->initBlock);
  this->children.push_back(this->condBlock);
  this->children.push_back(this->loopBlock);
  this->children.push_back(this->exitBlock);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage ForStatement::generateCode()
{
  MODULE_CHECK;

  auto irBuilder = getBlock()->getIRBuilder();

  if (this->brInst == nullptr) {
    this->brInst = irBuilder->CreateBr(this->initBlock->getLlvmBB());
  }

  // Generate the code of the initial condition.
  if (this->initBlock->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->initBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  // Generate the code of the condition of the for condition.
  if (this->condBlock->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->condBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  // Generate the code of the loop code.
  if (this->loopBlock->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->loopBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  // Generate the code of the exit block.
  if (this->exitBlock->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->exitBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  irBuilder->SetInsertPoint(this->exitBlock->getLlvmBB());

  return Expression::generateCode();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage ForStatement::postGenerateCode()
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

//------------------------------------------------------------------------------

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
  str.append(getBlock()->toString());
  return str;
}
}
