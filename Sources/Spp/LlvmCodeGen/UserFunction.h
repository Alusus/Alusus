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

  public: typedef std::pair<llvm::AllocaInst*, llvm::StoreInst*> Arg;


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

  public: virtual void createCallInstruction(llvm::IRBuilder<> *irb, std::vector<llvm::Value*> const &args)
  {
    throw EXCEPTION(GenericException, STR("Not implemented."));
  }

  public: void setLlvmFunction(llvm::Function *llvmF)
  {
    this->llvmFunction = llvmF;
  }

  public: llvm::Function* getLlvmFunction() const
  {
    return this->llvmFunction;
  }

  public: void addArg(llvm::AllocaInst *alloca, llvm::StoreInst *store)
  {
    this->args.push_back(Arg(alloca, store));
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
