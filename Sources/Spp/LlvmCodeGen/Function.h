/**
 * @file Spp/LlvmCodeGen/Function.h
 * Contains the header of class Spp::LlvmCodeGen::Function.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_FUNCTION_H
#define SPP_LLVMCODEGEN_FUNCTION_H

namespace Spp::LlvmCodeGen
{

class Function : public TiObject
{
  friend class TargetGenerator;

  //============================================================================
  // Type Info

  TYPE_INFO(Function, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Str name;
  private: FunctionType *functionType;
  private: llvm::Function *llvmFunction;
  private: llvm::AllocaInst *llvmVaList = 0;
  private: std::unique_ptr<llvm::Module> llvmModule;


  //============================================================================
  // Constructor & Destructor

  public: Function() : functionType(0), llvmFunction(0)
  {
  }

  public: Function(Char const *name, FunctionType *functionType, llvm::Function *llvmFunction)
    : name(name), functionType(functionType), llvmFunction(llvmFunction)
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

  public: void setFunctionType(FunctionType *ft)
  {
    this->functionType = ft;
  }
  public: FunctionType* getFunctionType() const
  {
    return this->functionType;
  }

  public: void setLlvmFunction(llvm::Function *llvmFn)
  {
    this->llvmFunction = llvmFn;
  }
  public: llvm::Function* getLlvmFunction() const
  {
    return this->llvmFunction;
  }

}; // class

} // namespace

#endif
