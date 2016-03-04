/**
 * @file Scg/Functions/ExternalFunction.cpp
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

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Functions/ExternalFunction.h>

using namespace llvm;

namespace Scg
{

//==============================================================================
// Constructors & Destructor

ExternalFunction::ExternalFunction(const std::string &name, SharedPtr<ValueTypeSpec> const &returnTypeSpec,
                                   const ValueTypeSpecArray &argTypeSpecs, bool varArgs) :
  name(name),
  returnTypeSpec(returnTypeSpec),
  varArgs(varArgs)
{
  this->argTypeSpecs = argTypeSpecs;
}


//==============================================================================
// Member Functions

llvm::Value *ExternalFunction::createLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
  return irb->CreateCall(this->llvmFunction, args);
}


void ExternalFunction::createLinkToExternalFunction(CodeGenUnit *codeGenUnit)
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->getArgumentTypeSpecs().size());

  auto module = this->findOwner<Module>();

  for (auto i = 0; i < getArgumentTypeSpecs().size(); i++) {
    auto type = this->getArgumentTypeSpecs()[i]->toValueType(*module);
    argTypes[i] = type->getLlvmType();
  }

  auto retType =
    (this->returnTypeSpec != nullptr ?
     this->returnTypeSpec->toValueType(*module) :
     module->getValueTypeByName(""))->getLlvmType();

  // Creates the LLVM function.
  auto funcType = llvm::FunctionType::get(retType, argTypes, this->varArgs);
  this->llvmFunction = llvm::Function::Create(funcType,
                       llvm::Function::ExternalLinkage, this->name, codeGenUnit->getLlvmModule());

  // TODO: Not sure whether these are required, but I just copied them from the code
  // that generates printf in Module.cpp
  this->llvmFunction->addFnAttr(Attribute::NoCapture);
  this->llvmFunction->addFnAttr(Attribute::NoUnwind);
}


AstNode::CodeGenerationStage ExternalFunction::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;

  // Create the LLVM function early because it is referenced in other pre-code
  // generation places, e.g. Block.
  // TODO: Should we add a function map to Block class as well so that we can
  // define functions inside blocks?

  // Is the function already defined?
  Module *module = this->findOwner<Module>();
  if (module->getFunction(this->name, this->getArgumentTypeSpecs()) != nullptr) {
    throw EXCEPTION(RedefinitionException,
                    ("Function already defined: " + this->name).c_str());
  }

  // Stores this function in the function store of the module.
  module->addFunction(s_cast<Function>(this->getSharedThis()));

  this->createLinkToExternalFunction(codeGenUnit);

  return CodeGenerationStage::CodeGeneration;
}


// TODO: Do we not need to define post code generation code?

std::string ExternalFunction::toString()
{
  // TODO: Implement.
  return "";
}

} // namespace
