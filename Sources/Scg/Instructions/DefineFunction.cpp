/**
 * @file Scg/Instructions/DefineFunction.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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
#include <Instructions/DefineFunction.h>
#include <Containers/Module.h>
#include <CodeGenUnit.h>

using namespace llvm;

namespace Scg
{

using namespace Core::Basic;

//==============================================================================
// Member Functions

AstNode::CodeGenerationStage DefineFunction::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  this->function = std::make_shared<UserDefinedFunction>(this->name, this->returnType, this->arguments, this->body);
  this->function->setOwner(this);

  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage DefineFunction::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  this->function->setOwner(0);
  this->function.reset();

  return CodeGenerationStage::None;
}


std::string DefineFunction::toString()
{
  // TODO: Implement this.
  return "";
}

} // namespace
