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
                                   const ValueTypeSpecArray &argTypeSpecs, bool varArgs) :
  name(name),
  returnTypeSpec(returnTypeSpec),
  varArgs(varArgs)
{
  this->argTypeSpecs = argTypeSpecs;
}

ExternalFunction::~ExternalFunction()
{
  // We don't delete this->returnType, etc., because they should be deleted by
  // the defining instruction.
}

llvm::Value *ExternalFunction::createLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
  return irb->CreateCall(this->llvmFunction, args);
}

void ExternalFunction::createLinkToExternalFunction()
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->getArgumentTypeSpecs().size());

  for (auto i = 0; i < getArgumentTypeSpecs().size(); i++) {
    auto type = this->getArgumentTypeSpecs()[i]->toValueType(*getModule());
    argTypes[i] = type->getLlvmType();
  }

  auto retType =
    (this->returnTypeSpec != nullptr ?
     this->returnTypeSpec->toValueType(*getModule()) :
     getModule()->getValueTypeByName(""))->getLlvmType();

  // Creates the LLVM function.
  auto funcType = llvm::FunctionType::get(retType, argTypes, this->varArgs);
  this->llvmFunction = llvm::Function::Create(funcType,
                       llvm::Function::ExternalLinkage, this->name, getModule()->getLlvmModule());

  // TODO: Not sure whether these are required, but I just copied them from the code
  // that generates printf in Module.cpp
  this->llvmFunction->addFnAttr(Attribute::NoCapture);
  this->llvmFunction->addFnAttr(Attribute::NoUnwind);
}

Expression::CodeGenerationStage ExternalFunction::preGenerateCode()
{
  MODULE_CHECK;

  // Create the LLVM function early because it is referenced in other pre-code
  // generation places, e.g. Block.
  // TODO: Should we add a function map to Block class as well so that we can
  // define functions inside blocks?

  // Is the function already defined?
  if (getModule()->getFunction(this->name, this->getArgumentTypeSpecs()) != nullptr) {
    throw EXCEPTION(RedefinitionException,
                    ("Function already defined: " + this->name).c_str());
  }

  // Stores this function in the function store of the module.
  getModule()->addFunction(this);

  createLinkToExternalFunction();

  return CodeGenerationStage::CodeGeneration;
}

// TODO: Do we not need to define post code generation code?

std::string ExternalFunction::toString()
{
  // TODO: Implement.
  return "";
}
}
