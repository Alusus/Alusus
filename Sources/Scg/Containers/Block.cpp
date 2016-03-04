/**
 * @file Scg/Containers/Block.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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

Block::Block(AstNodeSharedArray const &body)
  : llvmBasicBlock(0)
  , irBuilder(0)
{
  this->preserveChildrenCodeGenerationOrder = true;
  for (auto node : body) {
    this->children.add(node);
    OWN_SHAREDPTR(node);
  }
}


Block::~Block()
{
  for (Int i = 0; i < this->getCount(); ++i) {
    DISOWN_PLAINPTR(this->get(i));
  }
}

//------------------------------------------------------------------------------

const Variable *Block::getVariable(const std::string &name) const
{
  auto var = this->variableMap.find(name);

  Block *block;
  Module *module;

  if (var != this->variableMap.end()) {
    return var->second;
  } else if ((block=this->findOwner<Block>()) != nullptr) {
    return block->getVariable(name);
  } else if ((module=this->findOwner<Module>()) != nullptr) {
    auto var = module->getVariableMap().find(name);
    if (var != module->getVariableMap().end())
      return var->second;
  }

  return nullptr;
}

//------------------------------------------------------------------------------

AstNode::CodeGenerationStage Block::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  FUNCTION_CHECK;

  // Create LLVM basic block and IR builder.
  this->llvmBasicBlock = BasicBlock::Create(LlvmContainer::getContext(),
                         getNewBlockName(), this->findOwner<UserDefinedFunction>()->getLlvmFunction());
  this->irBuilder = new IRBuilder<>(this->llvmBasicBlock);
  return AstNode::preGenerateCode(codeGenUnit);
}

//----------------------------------------------------------------------------

AstNode::CodeGenerationStage Block::generateCode(CodeGenUnit *codeGenUnit)
{
  // Iteratively generate the code of all contained expressions.
  for (Int i = 0; i < this->children.getCount(); ++i) {
    auto expr = io_cast<AstNode>(this->children.get(i));
    if (expr->isTermInstGenerated())
      termInstGenerated = true;
  }

  return AstNode::generateCode(codeGenUnit);
}

//----------------------------------------------------------------------------

AstNode::CodeGenerationStage Block::postGenerateCode(CodeGenUnit *codeGenUnit)
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

  return AstNode::postGenerateCode(codeGenUnit);
}

//----------------------------------------------------------------------------

std::string Block::toString()
{
  std::string str;
  // TODO: This will definitely not be enough if the block statement is big,
  // yet in many cases it might be over allocation.
  str.reserve(10 * 1024);
  str.append("{\n");

  for (Int i = 0; i < this->children.getCount(); ++i) {
    auto expr = io_cast<AstNode>(this->children.get(i));
    auto exprStr = expr->toString();
    str.append("  ");
    str.append(boost::replace_all_copy(exprStr, "\n", "  \n"));
  }

  str.append("}\n");
  return "";
}

//----------------------------------------------------------------------------

std::string Block::getNewBlockName()
{
  return "block" + boost::lexical_cast<std::string>(getNewIndex());
}

} // namespace
