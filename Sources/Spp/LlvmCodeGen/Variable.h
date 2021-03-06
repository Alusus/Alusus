/**
 * @file Spp/LlvmCodeGen/Variable.h
 * Contains the header of class Spp::LlvmCodeGen::Variable.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_VARIABLE_H
#define SPP_LLVMCODEGEN_VARIABLE_H

namespace Spp::LlvmCodeGen
{

class Variable : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Variable, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str name;
  private: Type *type;
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

  public: void setName(Char const *n)
  {
    this->name = n;
  }

  public: Str const& getName() const
  {
    return this->name;
  }

  public: void setType(Type *t)
  {
    this->type = t;
  }

  public: Type* getType() const
  {
    return this->type;
  }

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

} // namespace

#endif
