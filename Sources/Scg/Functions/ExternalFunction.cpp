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
#include <Functions/ExternalFunction.h>

using namespace llvm;

namespace Scg
{
ExternalFunction::ExternalFunction(const std::string &name, ValueTypeSpec *returnTypeSpec,
    const ValueTypeSpecArray &argTypeSpecs, bool isVarArgs) :
        name(name),
        returnTypeSpec(returnTypeSpec),
        isVarArgs(isVarArgs)
{
  this->argTypeSpecs = argTypeSpecs;
}

ExternalFunction::~ExternalFunction()
{
  // We don't delete this->returnType, etc., because they should be deleted by
  // the defining instruction.
}

llvm::Value *ExternalFunction::CreateLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
  return irb->CreateCall(this->llvmFunction, args);
}

void ExternalFunction::CreateLinkToExternalFunction()
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->GetArgumentTypeSpecs().size());
  for (auto i = 0; i < GetArgumentTypeSpecs().size(); i++)
  {
    auto type = this->GetArgumentTypeSpecs()[i]->ToValueType(*GetModule());
    argTypes[i] = type->GetLlvmType();
  }
  auto retType =
      (this->returnTypeSpec != nullptr ?
          this->returnTypeSpec->ToValueType(*GetModule()) :
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

Expression::CodeGenerationStage ExternalFunction::PreGenerateCode()
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

  CreateLinkToExternalFunction();

  return CodeGenerationStage::CodeGeneration;
}

// TODO: Do we not need to define post code generation code?

std::string ExternalFunction::ToString()
{
  // TODO: Implement.
  return "";
}
}
