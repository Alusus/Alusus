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

// Boost header files
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Functions/UserDefinedFunction.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{
Block::Block(const ExpressionArray &body)
  : llvmBasicBlock(0)
  , irBuilder(0)
{
  this->preserveChildrenCodeGenerationOrder = true;
  this->children = body;
}

//------------------------------------------------------------------------------

const Variable *Block::getVariable(const std::string &name) const
{
  auto var = this->variableMap.find(name);

  if (var != this->variableMap.end()) {
    return var->second;
  } else if (getBlock() != nullptr) {
    return getBlock()->getVariable(name);
  } else if (getModule() != nullptr) {
    auto var = getModule()->getVariableMap().find(name);

    if (var != getModule()->getVariableMap().end())
      return var->second;
  }

  return nullptr;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage Block::preGenerateCode()
{
  FUNCTION_CHECK;

  for (auto expr : this->children)
    expr->setBlock(this);

  // Create LLVM basic block and IR builder.
  this->llvmBasicBlock = BasicBlock::Create(LlvmContainer::getContext(),
                         getNewBlockName(), getFunction()->getLlvmFunction());
  this->irBuilder = new IRBuilder<>(this->llvmBasicBlock);
  return Expression::preGenerateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Block::generateCode()
{
  // Iteratively generate the code of all contained expressions.
  for (auto expr : this->children) {
    if (expr->isTermInstGenerated())
      termInstGenerated = true;
  }

  return Expression::generateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Block::postGenerateCode()
{
  if (this->llvmBasicBlock != nullptr) {
    if (!this->llvmBasicBlock->hasNUses(0))
      // Cannot free the block yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;

    this->llvmBasicBlock->eraseFromParent();
    this->llvmBasicBlock = nullptr;
    delete this->irBuilder;
    this->irBuilder = nullptr;
  }

  return Expression::postGenerateCode();
}

//----------------------------------------------------------------------------

std::string Block::toString()
{
  std::string str;
  // TODO: This will definitely not be enough if the block statement is big,
  // yet in many cases it might be over allocation.
  str.reserve(10 * 1024);
  str.append("{\n");

  for (auto expr : this->children) {
    auto exprStr = expr->toString();
    str.append("  ");
    str.append(boost::replace_all_copy(exprStr, "\n", "  \n"));
  }

  str.append("}\n");
  return "";
}

//----------------------------------------------------------------------------

void Block::setBlock(Block *block)
{
  this->block = block;

  for (auto expr : this->children)
    expr->setBlock(this);
}

//----------------------------------------------------------------------------

std::string Block::getNewBlockName()
{
  return "block" + boost::lexical_cast<std::string>(getNewIndex());
}
}
