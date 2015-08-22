/**
 * @file Scg/Instructions/DefineVariable.cpp
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

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/DefineVariable.h>
#include <Operators/AssignmentOperator.h>
#include <Operators/Content.h>
#include <Types/ValueType.h>
#include <Values/Variable.h>

// LLVM header files

using namespace llvm;

namespace Scg
{
DefineVariable::~DefineVariable()
{
  delete this->typeSpec;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage DefineVariable::preGenerateCode()
{
  MODULE_CHECK;
  BLOCK_CHECK;
  FUNCTION_CHECK;

  // Fetch the type of the variable by name from the module.
  if (this->typeSpec == nullptr) {
    if (this->value->getCodeGenerationStage() != CodeGenerationStage::CodeGeneration) {
      return CodeGenerationStage::PreCodeGeneration;
    }

    this->typeSpec = this->value->getValueTypeSpec()->clone();
  }

  this->type = this->typeSpec->toValueType(*getModule());
  this->var = this->type->newVariable(this->name);
  this->var->setModule(getModule());
  this->var->setFunction(getFunction());
  this->var->setBlock(getBlock());
  this->children.push_back(this->var);
  // We added a new child, so we need to make sure that we let
  // Expression::CallPreGenerateCode() that we are still in
  // PreCodeGeneration stage.
  this->childrenCodeGenStage = CodeGenerationStage::PreCodeGeneration;
  return CodeGenerationStage::CodeGeneration;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage DefineVariable::postGenerateCode()
{
  this->type->deleteVariable(this->var);
  this->type = nullptr;
  this->var = nullptr;
  this->children.clear();
  return CodeGenerationStage::None;
}

//----------------------------------------------------------------------------

std::string DefineVariable::toString()
{
  return "def " + this->name + " : " + this->typeSpec->toString() + ";";
}
}
