/**
 * @file Spp/LlvmCodeGen/Variable.h
 * Contains the header of class Spp::LlvmCodeGen::Variable.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_VARIABLE_H
#define SPP_LLVMCODEGEN_VARIABLE_H

namespace Spp { namespace LlvmCodeGen
{

class Variable : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Variable, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Int llvmStructIndex = -1;
  private: llvm::AllocaInst *llvmAllocaInst = 0;
  private: llvm::GlobalVariable *llvmGlobalVariable = 0;


  //============================================================================
  // Constructor & Destructor

  public: Variable()
  {
  }


  //============================================================================
  // Member Functions

  public: void setLlvmStructIndex(Int i)
  {
    this->llvmStructIndex = i;
  }

  public: Int getLlvmStructIndex() const
  {
    return this->llvmStructIndex;
  }

  public: void setLlvmAllocaInst(llvm::AllocaInst *inst)
  {
    this->llvmAllocaInst = inst;
  }

  public: llvm::AllocaInst* getLlvmAllocaInst() const
  {
    return this->llvmAllocaInst;
  }

  public: void setLlvmGlobalVariable(llvm::GlobalVariable *var)
  {
    this->llvmGlobalVariable = var;
  }

  public: llvm::GlobalVariable* getLlvmGlobalVariable() const
  {
    return this->llvmGlobalVariable;
  }

}; // class

} } // namespace

#endif
