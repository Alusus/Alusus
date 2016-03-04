/**
 * @file Scg/Instructions/DefineVariable.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/DefineVariable.h>
#include <Operators/AssignmentOperator.h>
#include <Operators/Content.h>
#include <Types/ValueType.h>
#include <Values/Variable.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage DefineVariable::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;

  // Fetch the type of the variable by name from the module.
  if (this->typeSpec == nullptr) {
    if (this->value->getCodeGenerationStage() != CodeGenerationStage::CodeGeneration) {
      return CodeGenerationStage::PreCodeGeneration;
    }

    this->typeSpec = this->value->getValueTypeSpec()->clone();
  }

  this->type = this->typeSpec->toValueType(*this->findOwner<Module>());
  this->var = this->type->newVariable(this->name);
  this->var->setOwner(this);
  // We added a new child, so we need to make sure that we let
  // AstNode::CallPreGenerateCode() that we are still in
  // PreCodeGeneration stage.
  this->childrenCodeGenStage = CodeGenerationStage::PreCodeGeneration;
  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage DefineVariable::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  this->var->setOwner(0);
  this->type->releaseVariable(this->var.get());
  this->type = nullptr;
  this->var = nullptr;
  return CodeGenerationStage::None;
}


std::string DefineVariable::toString()
{
  return "def " + this->name + " : " + this->typeSpec->toString() + ";";
}

} // namespace
