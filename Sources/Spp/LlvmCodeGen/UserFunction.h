/**
 * @file Spp/LlvmCodeGen/UserFunction.h
 * Contains the header of class Spp::LlvmCodeGen::UserFunction.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_USERFUNCTION_H
#define SPP_LLVMCODEGEN_USERFUNCTION_H

namespace Spp { namespace LlvmCodeGen
{

class UserFunction : public Function
{
  //============================================================================
  // Type Info

  TYPE_INFO(UserFunction, Function, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Types

  public: struct Arg
  {
    llvm::Type *type;
    llvm::AllocaInst *allocaInst;
    llvm::StoreInst *storeInst;
    Arg(llvm::Type *t, llvm::AllocaInst *a, llvm::StoreInst *s) : type(t), allocaInst(a), storeInst(s) {}
  };


  //============================================================================
  // Member Variables

  private: llvm::Function *llvmFunction;

  private: std::vector<Arg> args;


  //============================================================================
  // Constructor & Destructor

  public: UserFunction() : llvmFunction(0)
  {
  }

  public: UserFunction(llvm::Function *llvmF) : llvmFunction(llvmF)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Value* createCallInstruction(llvm::IRBuilder<> *irb, std::vector<llvm::Value*> const &args)
  {
    return irb->CreateCall(this->llvmFunction, args);
  }

  public: void setLlvmFunction(llvm::Function *llvmF)
  {
    this->llvmFunction = llvmF;
  }

  public: llvm::Function* getLlvmFunction() const
  {
    return this->llvmFunction;
  }

  public: void addArg(llvm::Type *t, llvm::AllocaInst *alloca = 0, llvm::StoreInst *store = 0)
  {
    this->args.push_back(Arg(t, alloca, store));
  }

  public: void setArgAllocaInst(Int i, llvm::AllocaInst *alloca)
  {
    this->args[i].allocaInst = alloca;
  }

  public: void setArgStoreInst(Int i, llvm::StoreInst *store)
  {
    this->args[i].storeInst = store;
  }

  public: Word getArgCount() const
  {
    return this->args.size();
  }

  public: Arg getArg(Int i) const
  {
    return this->args[i];
  }

}; // class

} } // namespace

#endif
