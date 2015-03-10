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
#include <Containers/Program.h>

namespace Scg
{
  Expression::~Expression()
  {
    if (this->autoDeleteChildren == true)
      for (auto expr : this->children)
        delete expr;
  }

  //----------------------------------------------------------------------------

  // TODO: CallPreGenerateCode(), CallGenerateCode(), and CallPostGenerateCode() now
  // do the same functionality. Combine them into one function that accepts a
  // a parameter specifying the stage of code generation so that we remove duplication
  // and we make it easier to increase the number of stages if we need to.

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
    Bool errors = false;
    if (/*this->codeGenStage == CodeGenerationStage::CodeGeneration &&*/
        this->childrenCodeGenStage == CodeGenerationStage::PreCodeGeneration) {
      // The expression finished pre-code generation but the children didn't.
      this->childrenCodeGenStage = CodeGenerationStage::CodeGeneration;
      for (auto expr : this->children) {
        try {
          this->childrenCodeGenStage = std::min(this->childrenCodeGenStage,
              expr->CallPreGenerateCode());
        } catch (Core::Basic::Exception &e) {
          GetModule()->GetProgram()->GetBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                                                               STR("SCG1030"), 1, expr->GetSourceLocation()));
          errors = true;
        }
        if (this->childrenCodeGenStage == CodeGenerationStage::PreCodeGeneration &&
            this->preserveChildrenCodeGenerationOrder) {
          break;
        }
      }
    }
    if (errors) return CodeGenerationStage::None;

    return std::min(this->codeGenStage, this->childrenCodeGenStage);
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage Expression::CallGenerateCode()
  {
    // Have we already finished code generation for this expression and its
    // children?
    if (this->codeGenStage == CodeGenerationStage::PostCodeGeneration &&
        this->childrenCodeGenStage == CodeGenerationStage::PostCodeGeneration) {
      return CodeGenerationStage::PostCodeGeneration;
    }

    // Unlike CallPreGenerateCode() and CallPostGenerateCode(), we require here
    // that we generate the code for the children before generating the code of
    // the parent, e.g. you can't generate the addition before generating the
    // operands.
    // This is not always the case and sometimes we want to manually generate the
    // code for the children like in the IfStatement and ForStatement instructions.
    // In those instructions, we override CallGenerateCode() and make it call
    // GenerateCode() only, and then we do what we want in GenerateCode().
    Bool errors = false;
    if (this->childrenCodeGenStage == CodeGenerationStage::CodeGeneration) {
      // The expression finished code generation but the children didn't.
      this->childrenCodeGenStage = CodeGenerationStage::PostCodeGeneration;
      for (auto expr : this->children) {
        try {
          this->childrenCodeGenStage = std::min(this->childrenCodeGenStage,
              expr->CallGenerateCode());
        } catch (Core::Basic::Exception &e) {
          GetModule()->GetProgram()->GetBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                                                               STR("SCG1030"), 1, expr->GetSourceLocation()));
          errors = true;
        }
        if (expr->IsTermInstGenerated())
          // If an instruction that terminates execution of a block of code, e.g.
          // return statement, is generated, then we don't try to generate the
          // code for more children, as LLVM seems to object that.
          break;
      }
    }
    if (errors) return CodeGenerationStage::None;

    if (this->childrenCodeGenStage != CodeGenerationStage::PostCodeGeneration) {
      return CodeGenerationStage::CodeGeneration;
    }

    if (this->codeGenStage == CodeGenerationStage::CodeGeneration) {
      // The expression didn't yet finish code generation.
      this->codeGenStage = GenerateCode();
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

    Bool errors = false;
    if (this->childrenCodeGenStage == CodeGenerationStage::PostCodeGeneration) {
      // The children didn't yet finish post-code generation.
      this->childrenCodeGenStage = CodeGenerationStage::None;
      for (auto expr : this->children) {
        try {
          this->childrenCodeGenStage = std::max(this->childrenCodeGenStage,
              expr->CallPostGenerateCode());
        } catch (Core::Basic::Exception &e) {
          GetModule()->GetProgram()->GetBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                                                               STR("SCG1030"), 1, expr->GetSourceLocation()));
          errors = true;
        }
      }
    }
    if (errors) return CodeGenerationStage::None;
    if (this->childrenCodeGenStage == CodeGenerationStage::None &&
        this->codeGenStage == CodeGenerationStage::PreCodeGeneration) {
      // The children finished post-code generation but the expression didn't.
      this->codeGenStage = PostGenerateCode();
    }

    return std::min(this->codeGenStage, this->childrenCodeGenStage);
  }

  //----------------------------------------------------------------------------

  const Program *Expression::GetProgram() const
  {
    if (module == nullptr)
      return nullptr;
    return module->GetProgram();
  }

  //----------------------------------------------------------------------------

  void Expression::SetModule(Module *module)
  {
    this->module = module;
    for (auto expr : this->children)
      expr->SetModule(module);
  }

  //----------------------------------------------------------------------------

  void Expression::SetFunction(UserDefinedFunction *function)
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
