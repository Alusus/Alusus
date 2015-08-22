/**
 * @file Scg/Values/Variable.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg files
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Types/IntegerType.h>
#include <Values/Variable.h>

// LLVM header files
#include <llvm/IR/Argument.h>
#include <llvm/IR/GlobalVariable.h>

// STL header files

// Boost header files

namespace Scg
{
Expression::CodeGenerationStage Variable::preGenerateCode()
{
  MODULE_OR_BLOCK_CHECK;

  // TODO: Should storing the variable in the block's variable map be done
  // here or in DefineVariable instruction?
  // TODO: This code fails if the DefineVariable statement creating this Variable
  // is placed at the module level rather than within a function. The reason should
  // be obvious.

  // Is the variable already defined?
  if (getBlock() != nullptr) {
    // This is a local variable.
    if (getBlock()->getVariableMap().find(this->name) != getBlock()->getVariableMap().end())
      throw EXCEPTION(RedefinitionException, ("Local variable already defined: " + this->name).c_str());

    // Store this variable in the variable map of the container block.
    getBlock()->getVariableMap()[this->name] = this;
  } else {
    // This is a global variable.
    if (getModule()->getVariableMap().find(this->name) != getModule()->getVariableMap().end())
      throw EXCEPTION(RedefinitionException, ("Global variable already defined: " + this->name).c_str());

    // Store this variable in the variable map of the container block.
    getModule()->getVariableMap()[this->name] = this;
  }

  return CodeGenerationStage::CodeGeneration;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Variable::generateCode()
{
  MODULE_OR_BLOCK_CHECK;

  if (getBlock() != nullptr) {
    // This is a local variable, use the block's IRBuilder.
    this->llvmAllocaInst = getBlock()->getIRBuilder()->CreateAlloca(
                             const_cast<llvm::Type *>(this->variableType->getLlvmType()), 0, this->name.c_str());

    // If this variable represents a function argument, create the necessary
    // store instruction that sets the value of the variable passed by the
    // function caller.
    if (this->llvmArgument != nullptr)
      this->llvmStoreInst = getBlock()->getIRBuilder()->CreateStore(this->llvmArgument,
                            this->llvmAllocaInst);
  } else {
    // This is a global variable.
    this->llvmGlobalVariable = new llvm::GlobalVariable(
      *getModule()->getLlvmModule(),
      this->variableType->getLlvmType(),
      false, /* isConstant */
      llvm::GlobalVariable::ExternalLinkage,
      this->variableType->getDefaultLLVMValue() /* Initializer */,
      this->name.c_str());
  }

  // Defining a variable doesn't evaluate to a value.
  return Expression::generateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Variable::postGenerateCode()
{
  if (this->llvmAllocaInst && this->llvmAllocaInst->hasNUses(0)) {
    this->llvmAllocaInst->eraseFromParent();
    this->llvmAllocaInst = nullptr;
  }

  if (this->llvmGlobalVariable && this->llvmGlobalVariable->hasNUses(0)) {
    this->llvmGlobalVariable->eraseFromParent();
    this->llvmGlobalVariable = nullptr;
  }

  if (this->llvmStoreInst && this->llvmStoreInst->hasNUses(0)) {
    this->llvmStoreInst->eraseFromParent();
    this->llvmStoreInst = nullptr;
  }

  if (this->llvmAllocaInst != nullptr || this->llvmStoreInst != nullptr)
    return CodeGenerationStage::PostCodeGeneration;

  return CodeGenerationStage::None;
}

//----------------------------------------------------------------------------

std::string Variable::toString()
{
  return this->name;
}
}
