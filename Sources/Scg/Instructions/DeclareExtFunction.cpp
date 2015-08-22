/**
 * @file Scg/Instructions/DeclareExtFunction.cpp
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

// Boost header files
#include <boost/foreach.hpp>

// Scg files
#include <Instructions/DeclareExtFunction.h>
#include <Instructions/DefineFunction.h>
#include <Containers/Module.h>
#include <Functions/ExternalFunction.h>

namespace Scg
{
using namespace Core::Basic;

DeclareExtFunction::DeclareExtFunction(Char const *name,
                                       ValueTypeSpec *returnType, const ValueTypeSpecArray &argTypes,
                                       bool varArgs) :
  name(name),
  returnType(returnType),
  argTypes(argTypes),
  varArgs(varArgs)
{
}

//------------------------------------------------------------------------------

DeclareExtFunction::DeclareExtFunction(DeclareExtFunction *decl) :
  name(decl->getName()),
  varArgs(decl->isVarArgs())
{

  if (decl->getReturnType() != nullptr)
    this->returnType = decl->getReturnType()->clone();
  else
    this->returnType = nullptr;

  for (auto arg : decl->getArgumentTypes()) {
    this->argTypes.push_back(arg->clone());
  }
}

//------------------------------------------------------------------------------

// TODO: We shouldn't be really cloning variable types because that is just
// more memory allocation. Instead, we should have an allocator for variable
// types that doesn't allocate the same variable type twice, and takes the
// responsibility of freeing the types at the end.
DeclareExtFunction::DeclareExtFunction(DefineFunction *definition) :
  name(definition->getName()),
  varArgs(false)
{
  if (definition->getReturnType() != nullptr)
    this->returnType = definition->getReturnType()->clone();
  else
    this->returnType = nullptr;

  for (auto arg : definition->getArguments()) {
    this->argTypes.push_back(arg.getTypeSpec()->clone());
  }
}

//------------------------------------------------------------------------------

DeclareExtFunction::~DeclareExtFunction()
{
  delete this->returnType;

  for (auto type : this->argTypes)
    delete type;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DeclareExtFunction::preGenerateCode()
{
  MODULE_CHECK;
  BLOCK_CHECK;
  FUNCTION_CHECK;

  auto function = new ExternalFunction(
    this->name, this->returnType, this->argTypes, this->varArgs);
  ((Expression*)function)->setModule(getModule());
  ((Expression*)function)->setFunction(getFunction());
  ((Expression*)function)->setBlock(getBlock());
  this->children.push_back(function);

  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DeclareExtFunction::postGenerateCode()
{
  delete this->children[0];
  this->children.clear();
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string DeclareExtFunction::toString()
{
  // TODO: Implement this.
  return "";
}
}
