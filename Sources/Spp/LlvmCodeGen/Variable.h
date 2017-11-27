/**
 * @file Spp/LlvmCodeGen/Variable.h
 * Contains the header of class Spp::LlvmCodeGen::Variable.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_VARIABLE_H
#define SPP_LLVMCODEGEN_VARIABLE_H

namespace Spp { namespace LlvmCodeGen
{

class Variable : public Core::Basic::TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Variable, Core::Basic::TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: Ast::Type *astType = 0;
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

  public: void setAstType(Ast::Type *t)
  {
    this->astType = t;
  }

  public: Ast::Type* getAstType() const
  {
    return this->astType;
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

} } // namespace

#endif
