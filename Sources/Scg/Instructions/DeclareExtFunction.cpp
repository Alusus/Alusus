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

namespace Scg
{
DeclareExtFunction::DeclareExtFunction(Char const *name,
    ValueTypeSpec *returnType, const ValueTypeSpecArray &argTypes,
    bool isVarArgs) :
        name(name),
        returnType(returnType),
        argTypes(argTypes),
        isVarArgs(isVarArgs)
{
}

//------------------------------------------------------------------------------

DeclareExtFunction::DeclareExtFunction(DeclareExtFunction *decl) :
    name(decl->GetName()),
    isVarArgs(decl->IsVarArgs())
{

  if (decl->GetReturnType() != nullptr)
    this->returnType = decl->GetReturnType()->Clone();
  else
    this->returnType = nullptr;
  for (auto arg : decl->GetArgumentTypes())
  {
    this->argTypes.push_back(arg->Clone());
  }
}

//------------------------------------------------------------------------------

// TODO: We shouldn't be really cloning variable types because that is just
// more memory allocation. Instead, we should have an allocator for variable
// types that doesn't allocate the same variable type twice, and takes the
// responsibility of freeing the types at the end.
DeclareExtFunction::DeclareExtFunction(DefineFunction *definition) :
    name(definition->GetName()),
    isVarArgs(false)
{
  if (definition->GetReturnType() != nullptr)
    this->returnType = definition->GetReturnType()->Clone();
  else
    this->returnType = nullptr;
  for (auto arg : definition->GetArguments())
  {
    this->argTypes.push_back(arg.GetTypeSpec()->Clone());
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

Expression::CodeGenerationStage DeclareExtFunction::PreGenerateCode()
{
  MODULE_CHECK;
  BLOCK_CHECK;
  FUNCTION_CHECK;

  auto function = new UserDefinedFunction(
        this->name, this->returnType, this->argTypes, this->isVarArgs);
  ((Expression*)function)->SetModule(GetModule());
  ((Expression*)function)->SetFunction(GetFunction());
  ((Expression*)function)->SetBlock(GetBlock());
  this->children.push_back(function);

  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DeclareExtFunction::PostGenerateCode()
{
  delete this->children[0];
  this->children.clear();
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string DeclareExtFunction::ToString()
{
  // TODO: Implement this.
  return "";
}
}
