/**
 * @file Scg/Values/Function.cpp
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

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Functions/Function.h>
#include <Containers/Module.h>
#include <Values/Variable.h>

using namespace llvm;

namespace Scg
{
Function::Function(const std::string &name, ValueTypeSpec *returnType,
    const VariableDefinitionArray &argDefs, Block *body) :
        funcType(FunctionDefinitionSource::Internal),
        name(name),
        returnType(returnType),
        argDefs(argDefs)
{
  this->children.push_back(body);
  for (auto def : this->argDefs) {
    // TODO: Is there anyway to avoid casting away the constness?
    this->argTypes.push_back(const_cast<ValueTypeSpec*>(def.GetTypeSpec()));
  }
  // Instances of this class are defined during code generation time by
  // DefineFunction and DeclareExtFunction, and they take the responsibility
  // of deleting the body, so we shouldn't allow Expression class to delete it.
  this->autoDeleteChildren = false;
}

//------------------------------------------------------------------------------

Function::Function(const std::string &name, ValueTypeSpec *returnType,
    const ValueTypeSpecArray &argTypes, bool isVarArgs) :
        funcType(FunctionDefinitionSource::External),
        name(name),
        returnType(returnType),
        argTypes(argTypes),
        isVarArgs(isVarArgs)
{
}

//------------------------------------------------------------------------------

Function::~Function()
{
  // We don't delete this->returnType, etc., because they should be deleted by
  // the defining instruction.
  // TODO: This goes against the convention I have been following that the
  // class should delete the pointers it receives, but I have to do this way
  // because otherwise the pointers will be deleted more than once if the
  // code is compiled twice, because DefineFunction defines a new Function
  // instance each time its PreCodeGeneration() function called.
}

//------------------------------------------------------------------------------

void Function::CreateFunction()
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->argDefs.size());
  for (auto i = 0; i < argTypes.size(); i++)
  {
    auto type = this->argDefs[i].GetTypeSpec()->ToValueType(*GetModule());
    argTypes[i] = type->GetLlvmType();
  }
  auto retType =
      (this->returnType != nullptr ?
          this->returnType->ToValueType(*GetModule()) :
          GetModule()->GetValueTypeByName(""))->GetLlvmType();

  // Constructs the LLVM function type.
  auto funcType = llvm::FunctionType::get(retType, argTypes,
      false);

  // Creates the LLVM function representing this function.
  std::string fullName = GetModule()->GetName() + std::string("_") + this->name;
  this->llvmFunction = llvm::Function::Create(funcType,
      llvm::Function::ExternalLinkage, fullName, GetModule()->GetLlvmModule());

  // Creates the Alusus variables wrapping the LLVM the arguments of the function.
  auto i = 0;
  for (auto iter = this->llvmFunction->arg_begin(); i != this->argDefs.size();
      ++iter, ++i)
  {
    iter->setName(this->argDefs[i].GetVariableName());
    auto var =
        this->argDefs[i].GetTypeSpec()->ToValueType(*GetModule())->NewVariable(
            this->argDefs[i].GetVariableName(), iter);
    GetBody()->PrependExpression(var);
    this->args.push_back(var);
  }

  ((Expression*) GetBody())->SetModule(this->module);
  ((Expression*) GetBody())->SetFunction(this);
  ((Expression*) GetBody())->SetBlock(this->block);
}

//------------------------------------------------------------------------------

void Function::CreateLinkToExternalFunction()
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->argTypes.size());
  for (auto i = 0; i < argTypes.size(); i++)
  {
    auto type = this->argTypes[i]->ToValueType(*GetModule());
    argTypes[i] = type->GetLlvmType();
  }
  auto retType =
      (this->returnType != nullptr ?
          this->returnType->ToValueType(*GetModule()) :
          GetModule()->GetValueTypeByName(""))->GetLlvmType();

  // Creates the LLVM function.
  auto funcType = llvm::FunctionType::get(retType, argTypes, this->isVarArgs);
  this->llvmFunction = llvm::Function::Create(funcType,
      llvm::Function::ExternalLinkage, this->name, GetModule()->GetLlvmModule());

  // TODO: Not sure whether these are required, but I just copied them from the code
  // that generates printf in Module.cpp
  this->llvmFunction->addFnAttr(Attribute::NoCapture);
  this->llvmFunction->addFnAttr(Attribute::NoUnwind);
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage Function::PreGenerateCode()
{
  MODULE_CHECK;

  // Create the LLVM function early because it is referenced in other pre-code
  // generation places, e.g. Block.
  // TODO: Should we add a function map to Block class as well so that we can
  // define functions inside blocks?

  // Is the function already defined?
  if (GetModule()->GetFunction(this->name, this->argTypes) != nullptr) {
    THROW_EXCEPTION(RedefinitionException,
        "Function already defined: " + this->name);
  }

  // Stores this function in the function store of the module.
  GetModule()->AddFunction(this);

  if (this->funcType == FunctionDefinitionSource::Internal)
    CreateFunction();
  else
    CreateLinkToExternalFunction();

  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

CodeGenerationResult Function::GenerateCode()
{
  MODULE_CHECK;

  // If this is a function with a body, generate the code of the body.
  if (this->funcType == FunctionDefinitionSource::Internal)
  {
    GetBody()->GenerateCode();

    // If this function is of type void, we need to make sure there is a
    // return statement at the end in case the user doesn't add one.
    // TODO: What if the function is non-void and the user forgets to return
    // a value?
    if (this->returnType == nullptr || this->returnType->IsVoid())
      GetBody()->GetIRBuilder()->CreateRetVoid();
  }

  // A function definition doesn't evaluate to a value.
  return CodeGenerationResult();
}

// TODO: Do we not need to define post code generation code?

//------------------------------------------------------------------------------

void Function::SetFunction(Function *function)
{
  this->function = function;
  for (auto expr : this->children)
    expr->SetFunction(this);
}

//------------------------------------------------------------------------------

std::string Function::ToString()
{
  // TODO: Implement External as well.
  if (this->funcType == FunctionDefinitionSource::External)
    return "";

  std::stringstream str;
  str << "def " << this->name << " : function" << std::endl;
  if (this->argDefs.size() > 0)
  {
    str << "(";
    for (auto i = 0; i < this->argDefs.size(); i++)
      str << this->argDefs[0].ToString();
    str << ")";
  }
  str << GetBody()->ToString();
  return str.str();
}
}
