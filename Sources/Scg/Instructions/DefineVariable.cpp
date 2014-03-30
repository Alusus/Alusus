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
#include <iostream>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/DefineVariable.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <Values/Variable.h>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

namespace Scg
{
  DefineVariable::~DefineVariable()
  {
    delete this->typeSpec;
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage DefineVariable::PreGenerateCode()
  {
    MODULE_CHECK;
    BLOCK_CHECK;
    FUNCTION_CHECK;

    // Fetch the type of the variable by name from the module.
    this->type = this->typeSpec->ToValueType(*GetModule());
    this->var = this->type->NewVariable(this->name);
    this->var->SetModule(GetModule());
    this->var->SetFunction(GetFunction());
    this->var->SetBlock(GetBlock());
    this->children.push_back(this->var);
    return CodeGenerationStage::CodeGeneration;
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage DefineVariable::PostGenerateCode()
  {
    this->type->DeleteVariable(this->var);
    this->type = nullptr;
    this->var = nullptr;
    this->children.clear();
    return CodeGenerationStage::None;
  }

  //----------------------------------------------------------------------------

  std::string DefineVariable::ToString()
  {
    return "def " + this->name + " : " + this->typeSpec->ToString() + ";";
  }
}
