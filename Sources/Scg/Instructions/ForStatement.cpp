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
#include <iostream>

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/CondGotoStatement.h>
#include <Instructions/ForStatement.h>
#include <Instructions/GotoStatement.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{
ForStatement::ForStatement(Expression *init, Expression *cond, Expression *loop, Block *body)
  : init(init)
  , cond(cond)
  , loop(loop)
  , brInst(0)
{
  if (init == 0 && cond == 0 && loop == 0 && body == 0)
    THROW_EXCEPTION(InfiniteLoopException, "This loop is infinite.");

  // Prepare the blocks of the for loop.
  this->initBlock = this->init != 0 ? new Block({this->init}) : new Block();
  // At the end of the loop, we need to go back to the condition block.
  this->loopBlock = body;
  this->exitBlock = new Block();
  this->condBlock = this->cond != 0
    ? new Block({new CondGotoStatement(this->cond, this->loopBlock, this->exitBlock)})
    : new Block({new GotoStatement(this->loopBlock)});
  this->loopBlock->AppendExpression(loop);
  this->loopBlock->AppendExpression(new GotoStatement(this->condBlock));
  this->initBlock->AppendExpression(new GotoStatement(this->condBlock));

  this->children.push_back(this->initBlock);
  this->children.push_back(this->condBlock);
  this->children.push_back(this->loopBlock);
  this->children.push_back(this->exitBlock);
}

//------------------------------------------------------------------------------

CodeGenerationResult ForStatement::GenerateCode()
{
  MODULE_CHECK;

  auto irBuilder = GetBlock()->GetIRBuilder();

  this->brInst = irBuilder->CreateBr(this->initBlock->GetLlvmBB());
  this->initBlock->GenerateCode();
  this->condBlock->GenerateCode();
  this->loopBlock->GenerateCode();
  this->exitBlock->GenerateCode();
  irBuilder->SetInsertPoint(this->exitBlock->GetLlvmBB());

  // A for statement doesn't evaluate to a value.
  return CodeGenerationResult();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage ForStatement::PostGenerateCode()
{
  if (this->brInst != 0)
  {
    if (!this->brInst->hasNUses(0))
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;
    this->brInst->eraseFromParent(); this->brInst = 0;
  }

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string ForStatement::ToString()
{
  std::string str;
  str.append("for (");
  if (this->init != 0) str.append(this->init->ToString());
  str.append(";");
  if (this->cond != 0) str.append(this->cond->ToString());
  str.append(";");
  if (this->loop != 0) str.append(this->loop->ToString());
  str.append(")\n");
  str.append(GetBlock()->ToString());
  return str;
}
}
