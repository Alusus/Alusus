/**
* @file Scg/AstNode.cpp
*
* @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

// Scg header files
#include <AstNode.h>
#include <CodeGenUnit.h>

namespace Scg
{

// TODO: callPreGenerateCode(), callGenerateCode(), and callPostGenerateCode() now
// do the same functionality. Combine them into one function that accepts a
// a parameter specifying the stage of code generation so that we remove duplication
// and we make it easier to increase the number of stages if we need to.

AstNode::CodeGenerationStage AstNode::callPreGenerateCode(CodeGenUnit *codeGenUnit)
{
  // Marks the beginning of code generation if we haven't started yet.
  if (this->codeGenStage == CodeGenerationStage::None) {
    this->codeGenStage = CodeGenerationStage::PreCodeGeneration;
  }

  if (this->childrenCodeGenStage == CodeGenerationStage::None) {
    this->childrenCodeGenStage = CodeGenerationStage::PreCodeGeneration;
  }

  // Have we already finished pre-code generation for this object and its
  // children?
  if (this->codeGenStage == CodeGenerationStage::CodeGeneration &&
      this->childrenCodeGenStage == CodeGenerationStage::CodeGeneration) {
    return CodeGenerationStage::CodeGeneration;
  }

  if (this->codeGenStage == CodeGenerationStage::PreCodeGeneration) {
    // The object didn't yet finish pre-code generation.
    this->codeGenStage = preGenerateCode(codeGenUnit);
  }

  // Commented the condition that the parent object should be finish
  // pre-code generation before we can call the pre-code generation step
  // of the children. This is because we sometimes need to finish the
  // pre-code generation step of the children before we can finish that of
  // the parent. For example, in a call to a function, we need to know the
  // types of the arguments, i.e. the children, so that we can find the right
  // function to call. NOTE that we need to ensure that all objects we
  // have take the responsibility themselves of checking whether the parent
  // has finished pre-code generation step if they depend on it.
  Bool errors = false;

  if (/*this->codeGenStage == CodeGenerationStage::CodeGeneration &&*/
    this->childrenCodeGenStage == CodeGenerationStage::PreCodeGeneration) {
    // The object finished pre-code generation but the children didn't.
    this->childrenCodeGenStage = CodeGenerationStage::CodeGeneration;

    auto children = this->getInterface<Core::Data::Container>();
    if (children != 0) {
      for (Int i = 0; i < children->getCount(); ++i) {
        auto child = tio_cast<AstNode>(children->get(i));
        if (child == 0) continue;
        try {
          this->childrenCodeGenStage = std::min(this->childrenCodeGenStage,
                                                child->callPreGenerateCode(codeGenUnit));
        } catch (Core::Basic::Exception &e) {
          codeGenUnit->getBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                STR("SCG1030"), 1, child->getSourceLocation()));
          errors = true;
        }

        if (this->childrenCodeGenStage == CodeGenerationStage::PreCodeGeneration &&
            this->preserveChildrenCodeGenerationOrder) {
          break;
        }
      }
    }
  }

  if (errors) return CodeGenerationStage::None;

  return std::min(this->codeGenStage, this->childrenCodeGenStage);
}


AstNode::CodeGenerationStage AstNode::callGenerateCode(CodeGenUnit *codeGenUnit)
{
  // Have we already finished code generation for this object and its
  // children?
  if (this->codeGenStage == CodeGenerationStage::PostCodeGeneration &&
      this->childrenCodeGenStage == CodeGenerationStage::PostCodeGeneration) {
    return CodeGenerationStage::PostCodeGeneration;
  }

  // Unlike callPreGenerateCode() and callPostGenerateCode(), we require here
  // that we generate the code for the children before generating the code of
  // the parent, e.g. you can't generate the addition before generating the
  // operands.
  // This is not always the case and sometimes we want to manually generate the
  // code for the children like in the IfStatement and ForStatement instructions.
  // In those instructions, we override callGenerateCode() and make it call
  // generateCode() only, and then we do what we want in generateCode().
  Bool errors = false;

  if (this->childrenCodeGenStage == CodeGenerationStage::CodeGeneration) {
    // The object finished code generation but the children didn't.
    this->childrenCodeGenStage = CodeGenerationStage::PostCodeGeneration;

    auto children = this->getInterface<Core::Data::Container>();
    if (children != 0) {
      for (Int i = 0; i < children->getCount(); ++i) {
        auto child = tio_cast<AstNode>(children->get(i));
        if (child == 0) continue;
        try {
          this->childrenCodeGenStage = std::min(this->childrenCodeGenStage,
                                                child->callGenerateCode(codeGenUnit));
        } catch (Core::Basic::Exception &e) {
          codeGenUnit->getBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                STR("SCG1030"), 1, child->getSourceLocation()));
          errors = true;
        }

        if (child->isTermInstGenerated()) {
          // If an instruction that terminates execution of a block of code, e.g.
          // return statement, is generated, then we don't try to generate the
          // code for more children, as LLVM seems to object that.
          break;
        }
      }
    }
  }

  if (errors) return CodeGenerationStage::None;

  if (this->childrenCodeGenStage != CodeGenerationStage::PostCodeGeneration) {
    return CodeGenerationStage::CodeGeneration;
  }

  if (this->codeGenStage == CodeGenerationStage::CodeGeneration) {
    // The object didn't yet finish code generation.
    this->codeGenStage = generateCode(codeGenUnit);
  }

  return std::min(this->codeGenStage, this->childrenCodeGenStage);
}


AstNode::CodeGenerationStage AstNode::callPostGenerateCode(CodeGenUnit *codeGenUnit)
{
  // Have we already finished post-code generation for this object and its
  // children?
  if (this->codeGenStage == CodeGenerationStage::None &&
      this->childrenCodeGenStage == CodeGenerationStage::None)
    return CodeGenerationStage::None;

  Bool errors = false;

  if (this->childrenCodeGenStage == CodeGenerationStage::PostCodeGeneration) {
    // The children didn't yet finish post-code generation.
    this->childrenCodeGenStage = CodeGenerationStage::None;

    auto children = this->getInterface<Core::Data::Container>();
    if (children != 0) {
      for (Int i = 0; i < children->getCount(); ++i) {
        auto child = tio_cast<AstNode>(children->get(i));
        if (child == 0) continue;
        try {
          this->childrenCodeGenStage = std::max(this->childrenCodeGenStage,
                                                child->callPostGenerateCode(codeGenUnit));
        } catch (Core::Basic::Exception &e) {
          codeGenUnit->getBuildMsgStore()->add(
            std::make_shared<Core::Processing::CustomBuildMsg>(e.getErrorMessage().c_str(),
                STR("SCG1030"), 1, child->getSourceLocation()));
          errors = true;
        }
      }
    }
  }

  if (errors) return CodeGenerationStage::None;

  if (this->childrenCodeGenStage == CodeGenerationStage::None &&
      this->codeGenStage == CodeGenerationStage::PreCodeGeneration) {
    // The children finished post-code generation but the object didn't.
    this->codeGenStage = postGenerateCode(codeGenUnit);
  }

  return std::min(this->codeGenStage, this->childrenCodeGenStage);
}

} // namespace
