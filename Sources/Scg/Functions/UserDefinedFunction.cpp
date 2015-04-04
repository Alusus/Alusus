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

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Functions/UserDefinedFunction.h>
#include <Values/Variable.h>

using namespace llvm;

namespace Scg
{
UserDefinedFunction::UserDefinedFunction(const std::string &name,
    ValueTypeSpec *returnTypeSpec, const VariableDefinitionArray &argDefs,
    Block *body)
    : name(name)
    , returnTypeSpec(returnTypeSpec)
    , argDefs(argDefs)
{
  this->children.push_back(body);
  for (auto def : this->argDefs) {
    // TODO: Is there anyway to avoid casting away the constness?
    this->argTypeSpecs.push_back(const_cast<ValueTypeSpec*>(def.GetTypeSpec()));
  }
}

UserDefinedFunction::~UserDefinedFunction()
{
  // We don't delete this->returnType, etc., because they should be deleted by
  // the defining instruction.
  // TODO: This goes against the convention I have been following that the
  // class should delete the pointers it receives, but I have to do this way
  // because otherwise the pointers will be deleted more than once if the
  // code is compiled twice, because DefineFunction defines a new UserDefinedFunction
  // instance each time its PreCodeGeneration() function called.
}

llvm::Value *UserDefinedFunction::CreateLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
  return irb->CreateCall(this->llvmFunction, args);
}

void UserDefinedFunction::CreateFunction()
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->argDefs.size());
  for (auto i = 0; i < argTypes.size(); i++)
  {
    auto type = this->argDefs[i].GetTypeSpec()->ToValueType(*GetModule());
    argTypes[i] = type->GetLlvmType();
  }
  auto retType =
      (this->returnTypeSpec != nullptr ?
          this->returnTypeSpec->ToValueType(*GetModule()) :
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

Expression::CodeGenerationStage UserDefinedFunction::PreGenerateCode()
{
  MODULE_CHECK;

  // Create the LLVM function early because it is referenced in other pre-code
  // generation places, e.g. Block.
  // TODO: Should we add a function map to Block class as well so that we can
  // define functions inside blocks?

  // Is the function already defined?
  if (GetModule()->GetFunction(this->name, this->GetArgumentTypeSpecs()) != nullptr) {
    throw EXCEPTION(RedefinitionException,
        ("Function already defined: " + this->name).c_str());
  }

  // Stores this function in the function store of the module.
  GetModule()->AddFunction(this);

  CreateFunction();

  return CodeGenerationStage::CodeGeneration;
}

Expression::CodeGenerationStage UserDefinedFunction::GenerateCode()
{
  MODULE_CHECK;

  GetBody()->GenerateCode();

  // If this function is of type void, we need to make sure there is a
  // return statement at the end in case the user doesn't add one.
  // TODO: What if the function is non-void and the user forgets to return
  // a value?
  if (this->returnTypeSpec == nullptr || this->returnTypeSpec->IsVoid())
    GetBody()->GetIRBuilder()->CreateRetVoid();

  // A function definition doesn't evaluate to a value.
  return Expression::GenerateCode();
}

// TODO: Do we not need to define post code generation code?

void UserDefinedFunction::SetFunction(UserDefinedFunction *function)
{
  this->function = function;
  for (auto expr : this->children)
    expr->SetFunction(this);
}

std::string UserDefinedFunction::ToString()
{
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
