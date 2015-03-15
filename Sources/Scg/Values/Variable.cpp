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
Expression::CodeGenerationStage Variable::PreGenerateCode()
{
  MODULE_OR_BLOCK_CHECK;

  // TODO: Should storing the variable in the block's variable map be done
  // here or in DefineVariable instruction?
  // TODO: This code fails if the DefineVariable statement creating this Variable
  // is placed at the module level rather than within a function. The reason should
  // be obvious.

  // Is the variable already defined?
  if (GetBlock() != nullptr) {
    // This is a local variable.
    if (GetBlock()->GetVariableMap().find(this->name) != GetBlock()->GetVariableMap().end())
      throw EXCEPTION(RedefinitionException, ("Local variable already defined: " + this->name).c_str());

    // Store this variable in the variable map of the container block.
    GetBlock()->GetVariableMap()[this->name] = this;
  } else {
    // This is a global variable.
    if (GetModule()->GetVariableMap().find(this->name) != GetModule()->GetVariableMap().end())
      throw EXCEPTION(RedefinitionException, ("Global variable already defined: " + this->name).c_str());

    // Store this variable in the variable map of the container block.
    GetModule()->GetVariableMap()[this->name] = this;
  }

  return CodeGenerationStage::CodeGeneration;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Variable::GenerateCode()
{
  MODULE_OR_BLOCK_CHECK;

  if (GetBlock() != nullptr) {
    // This is a local variable, use the block's IRBuilder.
    this->llvmAllocaInst = GetBlock()->GetIRBuilder()->CreateAlloca(
        const_cast<llvm::Type *>(this->variableType->GetLlvmType()), 0, this->name.c_str());

    // If this variable represents a function argument, create the necessary
    // store instruction that sets the value of the variable passed by the
    // function caller.
    if (this->llvmArgument != nullptr)
      this->llvmStoreInst = GetBlock()->GetIRBuilder()->CreateStore(this->llvmArgument,
          this->llvmAllocaInst);
  } else {
    // This is a global variable.
    this->llvmGlobalVariable = new llvm::GlobalVariable(
        *GetModule()->GetLlvmModule(),
        this->variableType->GetLlvmType(),
        false, /* isConstant */
        llvm::GlobalVariable::ExternalLinkage,
        this->variableType->GetDefaultLLVMValue() /* Initializer */,
        this->name.c_str());
  }

  // Defining a variable doesn't evaluate to a value.
  return Expression::GenerateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Variable::PostGenerateCode()
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

std::string Variable::ToString()
{
  return this->name;
}
}
