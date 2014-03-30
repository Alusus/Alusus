/**
 * @file Scg/Containers/Block.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

#include <stdarg.h>

// Boost header files
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Instructions/Return.h>
#include <LlvmContainer.h>
#include <Values/Function.h>

using namespace llvm;

namespace Scg
{
  Block::Block(const ExpressionArray &body)
    : llvmBasicBlock(0)
    , irBuilder(0)
  {
    this->children = body;
  }

//------------------------------------------------------------------------------

const Variable *Block::GetVariable(const std::string &name) const
{
  auto var = this->variableMap.find(name);
  if (var != this->variableMap.end()) {
    return var->second;
  } else if (GetBlock() != nullptr) {
    return GetBlock()->GetVariable(name);
  } else if (GetModule() != nullptr) {
    auto var = GetModule()->GetVariableMap().find(name);
    if (var != GetModule()->GetVariableMap().end()) return var->second;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

  Expression::CodeGenerationStage Block::PreGenerateCode()
  {
    FUNCTION_CHECK;

    for (auto expr : this->children)
      expr->SetBlock(this);

    // Create LLVM basic block and IR builder.
    this->llvmBasicBlock = BasicBlock::Create(LlvmContainer::GetContext(),
      GetNewBlockName(), GetFunction()->GetLlvmFunction());
    this->irBuilder = new IRBuilder<>(this->llvmBasicBlock);
    return CodeGenerationStage::CodeGeneration;
  }

  //----------------------------------------------------------------------------

  CodeGenerationResult Block::GenerateCode()
  {
    // Iteratively generate the code of all contained expressions.
    for (auto expr : this->children)
    {
      auto result = expr->GenerateCode();
      if (result.termInstGenerated)
        return CodeGenerationResult(0, true);
    }

    // A block doesn't evaluate to a value.
    return CodeGenerationResult();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage  Block::PostGenerateCode()
  {
    if (this->llvmBasicBlock != nullptr)
    {
      if (!this->llvmBasicBlock->hasNUses(0))
        // Cannot free the block yet; stay in PostCodeGeneration stage.
        return CodeGenerationStage::PostCodeGeneration;
      this->llvmBasicBlock->eraseFromParent(); this->llvmBasicBlock = 0;
      delete this->irBuilder; this->irBuilder = 0;
    }

    return CodeGenerationStage::None;
  }

  //----------------------------------------------------------------------------

  std::string Block::ToString()
  {
    std::string str;
    // TODO: This will definitely not be enough if the block statement is big,
    // yet in many cases it might be over allocation.
    str.reserve(10*1024);
    str.append("{\n");
    for (auto expr : this->children)
    {
      auto exprStr = expr->ToString();
      str.append("  ");
      str.append(boost::replace_all_copy(exprStr, "\n", "  \n"));
    }
    str.append("}\n");
    return "";
  }

  //----------------------------------------------------------------------------

  void Block::SetBlock(Block *block)
  {
    this->block = block;
    for (auto expr : this->children)
      expr->SetBlock(this);
  }

  //----------------------------------------------------------------------------

  std::string Block::GetNewBlockName()
  {
    return "block" + boost::lexical_cast<std::string>(GetNewIndex());
  }
}
