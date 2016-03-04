/**
 * @file Scg/Functions/BasicBuiltInFunction.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_BASICBINARYBUILTINFUNCTION_H
#define SCG_BASICBINARYBUILTINFUNCTION_H

#include <functional>
#include "core.h"
#include <exceptions.h>
#include <Functions/Function.h>

namespace Scg
{

/**
 * A built-in function to add two integer values.
 */
class BasicBinaryBuiltInFunction : public Function
{
  //============================================================================
  // Type Info

  TYPE_INFO(BasicBinaryBuiltInFunction, Function, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Types

  public: using Callback = std::function<llvm::Value*(llvm::IRBuilder<> *irb, llvm::Value*, llvm::Value*)>;


  //============================================================================
  // Member Variables

  //! Storing the binary operator so that it can be freed after code generation.
  private: llvm::Value *llvmValue = nullptr;
  //! Storing the name of the function.
  private: std::string name;
  private: Callback function;
  private: SharedPtr<ValueTypeSpec> retTypeSpec;


  //============================================================================
  // Constructor & Destructor

  /**
  * @param[in] name     The name of the function.
  * @param[in] retType  The return type of the function, e.g. "double", "int".
  * @param[in] arg1Type The type of the function's first argument, e.g.
  *                     double", "int".
  * @param[in] arg2Type The type of the function's second argument, e.g.
  *                     "double", "int".
  * @param[in] function The function to be called to create the LLVM
  *                     instruction to execute this function.
  */
  public: BasicBinaryBuiltInFunction(const Core::Basic::Char *name, const Core::Basic::Char *retType,
                                     const Core::Basic::Char *arg1Type, const Core::Basic::Char *arg2Type,
                                     Callback function);


  //============================================================================
  // Member Functions

  //! @copydoc Function::getName()
  public: virtual const std::string &getName() const
  {
    return name;
  }

  //! @copydoc Function::createLLVMInstruction()
  public: virtual llvm::Value *createLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const override;

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->retTypeSpec;
  }

}; // class

} // namespace

#endif
