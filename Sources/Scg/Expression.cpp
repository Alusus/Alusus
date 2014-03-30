/**
 * @file Scg/Expression.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// Scg header files
#include <Expression.h>

namespace Scg
{
  Expression::~Expression()
  {
    if (this->autoDeleteChildren == true)
      for (auto expr : this->children)
        delete expr;
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage Expression::CallPreGenerateCode()
  {
    // Marks the beginning of code generation if we haven't started yet.
    if (this->codeGenStage == CodeGenerationStage::None) {
      this->codeGenStage = CodeGenerationStage::PreCodeGeneration;
    }
    if (this->childrenCodeGenStage == CodeGenerationStage::None) {
      this->childrenCodeGenStage = CodeGenerationStage::PreCodeGeneration;
    }

    // Have we already finished pre-code generation for this expression and its
    // children?
    if (this->codeGenStage == CodeGenerationStage::CodeGeneration &&
        this->childrenCodeGenStage == CodeGenerationStage::CodeGeneration) {
      return CodeGenerationStage::CodeGeneration;
    }

    if (this->codeGenStage == CodeGenerationStage::PreCodeGeneration) {
      // The expression didn't yet finish pre-code generation.
      this->codeGenStage = PreGenerateCode();
    }
    // Commented the condition that the parent expression should be finish
    // pre-code generation before we can call the pre-code generation step
    // of the children. This is because we sometimes need to finish the
    // pre-code generation step of the children before we can finish that of
    // the parent. For example, in a call to a function, we need to know the
    // types of the arguments, i.e. the children, so that we can find the right
    // function to call. NOTE that we need to ensure that all expressions we
    // have take the responsibility themselves of checking whether the parent
    // has finished pre-code generation step if they depend on it.
    if (/*this->codeGenStage == CodeGenerationStage::CodeGeneration &&*/
        this->childrenCodeGenStage == CodeGenerationStage::PreCodeGeneration) {
      // The expression finished pre-code generation but the children didn't.
      this->childrenCodeGenStage = CodeGenerationStage::CodeGeneration;
      for (auto expr : this->children)
        this->childrenCodeGenStage = std::min(this->childrenCodeGenStage,
            expr->CallPreGenerateCode());
    }

    return std::min(this->codeGenStage, this->childrenCodeGenStage);
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage Expression::CallPostGenerateCode()
  {
    // Have we already finished post-code generation for this expression and its
    // children?
    if (this->codeGenStage == CodeGenerationStage::None &&
        this->childrenCodeGenStage == CodeGenerationStage::None)
      return CodeGenerationStage::None;

    if (this->childrenCodeGenStage == CodeGenerationStage::PostCodeGeneration)
    {
      // The children didn't yet finish post-code generation.
      this->childrenCodeGenStage = CodeGenerationStage::None;
      for (auto expr : this->children)
        this->childrenCodeGenStage = std::max(this->childrenCodeGenStage,
            expr->CallPostGenerateCode());
    }
    if (this->childrenCodeGenStage == CodeGenerationStage::None &&
        this->codeGenStage == CodeGenerationStage::PreCodeGeneration)
    {
      // The children finished post-code generation but the expression didn't.
      this->codeGenStage = PostGenerateCode();
    }

    return std::min(this->codeGenStage, this->childrenCodeGenStage);
  }

  //----------------------------------------------------------------------------

  void Expression::SetModule(Module *module)
  {
    this->module = module;
    for (auto expr : this->children)
      expr->SetModule(module);
  }

  //----------------------------------------------------------------------------

  void Expression::SetFunction(Function *function)
  {
    this->function = function;
    for (auto expr : this->children)
      expr->SetFunction(function);
  }

  //----------------------------------------------------------------------------

  void Expression::SetBlock(Block *block)
  {
    this->block = block;
    for (auto expr : this->children)
      expr->SetBlock(block);
  }
}
