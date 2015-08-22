/**
 * @file Scg/Instructions/WhileStatement.cpp
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
#include <Instructions/GotoStatement.h>
#include <Instructions/WhileStatement.h>

using namespace llvm;

namespace Scg
{
WhileStatement::WhileStatement(Expression *cond, Block *body)
  : cond(cond)
  , brInst(0)
{
  if (cond == nullptr && body == nullptr)
    throw EXCEPTION(InfiniteLoopException, "This loop is infinite.");

  this->loopBlock = body;

  // Create a block which is used to exit the loop if the condition is not
  // met.
  this->exitBlock = new Block();
  this->condBlock = this->cond != nullptr
  ? new Block({ new CondGotoStatement(this->cond, this->loopBlock, this->exitBlock) })
    : new Block({ new GotoStatement(this->loopBlock) });

  // Add an expression at the end of the loop block to go back to the
  // block checking the condition.
  this->loopBlock->appendExpression(new GotoStatement(this->condBlock));

  this->children.push_back(this->condBlock);
  this->children.push_back(this->loopBlock);
  this->children.push_back(this->exitBlock);
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage WhileStatement::generateCode()
{
  MODULE_CHECK;

  auto irBuilder = getBlock()->getIRBuilder();

  this->brInst = irBuilder->CreateBr(this->condBlock->getLlvmBB());

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

//----------------------------------------------------------------------------

Expression::CodeGenerationStage WhileStatement::postGenerateCode()
{
  SAFE_DELETE_LLVM_INST(this->brInst);
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------------------------

std::string WhileStatement::toString()
{
  std::string str;
  str.append("while (");

  if (this->cond != 0) str.append(this->cond->toString());

  str.append(")\n");
  str.append(getBlock()->toString());
  return str;
}
}
