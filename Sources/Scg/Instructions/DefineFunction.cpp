/**
 * @file Scg/Instructions/DefineFunction.cpp
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
#include <Instructions/DefineFunction.h>
#include <Containers/Module.h>

using namespace llvm;

namespace Scg
{
using namespace Core::Basic;

DefineFunction::DefineFunction(Char const *name, ValueTypeSpec *returnType,
                               const VariableDefinitionArray &arguments, Block *body) :
  name(name),
  returnType(returnType),
  arguments(arguments),
  body(body)
{
}

//------------------------------------------------------------------------------

DefineFunction::~DefineFunction()
{
  delete this->returnType;

  for (auto arg : this->arguments)
    delete arg.getTypeSpec();

  delete this->body;

}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DefineFunction::preGenerateCode()
{
  MODULE_CHECK;
  BLOCK_CHECK;
  FUNCTION_CHECK;

  auto function = new UserDefinedFunction(this->name, this->returnType, this->arguments, this->body);
  ((Expression*)function)->setModule(getModule());
  ((Expression*)function)->setFunction(getFunction());
  ((Expression*)function)->setBlock(getBlock());
  this->children.push_back(function);

  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DefineFunction::postGenerateCode()
{
  delete this->children[0];
  this->children.clear();
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string DefineFunction::toString()
{
  // TODO: Implement this.
  return "";
}
}
