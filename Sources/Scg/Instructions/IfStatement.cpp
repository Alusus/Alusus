/**
 * @file Scg/Instructions/IfStatement.cpp
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
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/IfStatement.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{
IfStatement::IfStatement(Expression *condition, Block *thenBlock, Block *elseBlock)
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
  this->mergeBlock = new Block();

  if (thenBlock == 0)
    throw EXCEPTION(ArgumentOutOfRangeException, "If statements must "
                    "have a 'then' block.");

  this->children.push_back(this->condition);
  this->children.push_back(this->thenBlock);

  if (this->elseBlock != 0)
    this->children.push_back(this->elseBlock);

  this->children.push_back(this->mergeBlock);
}

//----------------------------------------------------------------------------

IfStatement::~IfStatement()
{
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage IfStatement::generateCode()
{
  MODULE_CHECK;

  auto irBuilder = getBlock()->getIRBuilder();

  // Evaluate the condition to a boolean.
  // TODO: 'condition' should be freed in postGenerateCode().
  if (this->condition->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->condition->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
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
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->thenBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  if (this->elseBlock != nullptr) {
    if (this->elseBlock->getCodeGenerationStage() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      if (this->elseBlock->callGenerateCode() ==
          Expression::CodeGenerationStage::CodeGeneration) {
        return Expression::CodeGenerationStage::CodeGeneration;
      }
    }
  }

  if (this->mergeBlock->getCodeGenerationStage() ==
      Expression::CodeGenerationStage::CodeGeneration) {
    if (this->mergeBlock->callGenerateCode() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      return Expression::CodeGenerationStage::CodeGeneration;
    }
  }

  // Add branch instructions to the end of the 'then' and 'else' parts.
  if (!this->thenBlock->isTermInstGenerated())
    this->thenBranch = this->thenBlock->getIRBuilder()->CreateBr(this->mergeBlock->getLlvmBB());

  if (this->elseBlock != nullptr && !this->elseBlock->isTermInstGenerated())
    this->elseBranch = this->elseBlock->getIRBuilder()->CreateBr(this->mergeBlock->getLlvmBB());

  irBuilder->SetInsertPoint(this->mergeBlock->getLlvmBB());

  return Expression::generateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage IfStatement::postGenerateCode()
{
  SAFE_DELETE_LLVM_INST(this->cmpInst);
  SAFE_DELETE_LLVM_INST(this->ifBranch);
  SAFE_DELETE_LLVM_INST(this->thenBranch);
  SAFE_DELETE_LLVM_INST(this->elseBranch);
  return CodeGenerationStage::None;
}

//----------------------------------------------------------------------------

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
}
