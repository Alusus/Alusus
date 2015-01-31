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
#include <iostream>

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
      THROW_EXCEPTION(ArgumentOutOfRangeException, "If statements must "
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

  Expression::CodeGenerationStage IfStatement::GenerateCode()
  {
    MODULE_CHECK;

    auto irBuilder = GetBlock()->GetIRBuilder();

    // Evaluate the condition to a boolean.
    // TODO: 'condition' should be freed in PostGenerateCode().
    if (this->condition->GetCodeGenerationStage() ==
    		Expression::CodeGenerationStage::CodeGeneration) {
      if (this->condition->CallGenerateCode() ==
      		Expression::CodeGenerationStage::CodeGeneration) {
  		  return Expression::CodeGenerationStage::CodeGeneration;
      }
    }
    auto condition = this->condition->GetGeneratedLlvmValue();
    if (condition == nullptr)
      // TODO: This exception is being frequently used, with a similar statement each time. A macro
      // should be created for it to avoid duplication.
      THROW_EXCEPTION(InvalidValueException, "The condition of the if statement "
          "doesn't evaluate to a value: " + this->condition->ToString());
    this->cmpInst = static_cast<llvm::ICmpInst*>(irBuilder->CreateICmpNE(condition,
      ConstantInt::get(condition->getType(), 0), "ifcond"));

    this->ifBranch = irBuilder->CreateCondBr(condition, this->thenBlock->GetLlvmBB(),
      this->elseBlock != 0 ? this->elseBlock->GetLlvmBB() : this->mergeBlock->GetLlvmBB());

    // Generate the code of the 'then', 'else', and merge parts.
    if (this->thenBlock->GetCodeGenerationStage() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      if (this->thenBlock->CallGenerateCode() ==
          Expression::CodeGenerationStage::CodeGeneration) {
        return Expression::CodeGenerationStage::CodeGeneration;
      }
    }
    if (this->elseBlock != nullptr)
    {
      if (this->elseBlock->GetCodeGenerationStage() ==
          Expression::CodeGenerationStage::CodeGeneration) {
        if (this->elseBlock->CallGenerateCode() ==
            Expression::CodeGenerationStage::CodeGeneration) {
          return Expression::CodeGenerationStage::CodeGeneration;
        }
      }
    }
    if (this->mergeBlock->GetCodeGenerationStage() ==
        Expression::CodeGenerationStage::CodeGeneration) {
      if (this->mergeBlock->CallGenerateCode() ==
          Expression::CodeGenerationStage::CodeGeneration) {
        return Expression::CodeGenerationStage::CodeGeneration;
      }
    }

    // Add branch instructions to the end of the 'then' and 'else' parts.
    if (!this->thenBlock->IsTermInstGenerated())
      this->thenBranch = this->thenBlock->GetIRBuilder()->CreateBr(this->mergeBlock->GetLlvmBB());
    if (this->elseBlock != nullptr && !this->elseBlock->IsTermInstGenerated())
      this->elseBranch = this->elseBlock->GetIRBuilder()->CreateBr(this->mergeBlock->GetLlvmBB());
    irBuilder->SetInsertPoint(this->mergeBlock->GetLlvmBB());

    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage IfStatement::PostGenerateCode()
  {
    SAFE_DELETE_LLVM_INST(this->cmpInst);
    SAFE_DELETE_LLVM_INST(this->ifBranch);
    SAFE_DELETE_LLVM_INST(this->thenBranch);
    SAFE_DELETE_LLVM_INST(this->elseBranch);
    return CodeGenerationStage::None;
  }

  //----------------------------------------------------------------------------

  std::string IfStatement::ToString()
  {
    std::string str;
    str.append("if (" + this->condition->ToString() + ")\n");
    str.append(this->thenBlock->ToString());
    if (this->elseBlock != 0)
    {
      str.append("else\n");
      str.append(this->elseBlock->ToString());
    }
    return str;
  }
}
