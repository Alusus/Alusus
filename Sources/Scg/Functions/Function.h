/**
 * @file Scg/Functions/Function.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_FUNCTION_H
#define SCG_FUNCTION_H

#include <vector>

#include "core.h"
#include <Containers/List.h>
#include <Functions/FunctionSignature.h>
#include <Values/Value.h>

// LLVM header files
#pragma warning(disable: 4146 4800 4355 4996)
#include <llvm/IR/IRBuilder.h>
#include <Types/ValueType.h>
#pragma warning(default: 4146 4800 4355 4996)

namespace Scg
{

/**
 * Base class for all expressions that can be called and return a value.
 * Such expressions include built-in functions and user-defined functions.
 */
class Function : public Value
{
  //============================================================================
  // Type Info

  TYPE_INFO(Function, Value, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  // Defining as protected to allow inheritors to specify them. See for example UserDefinedFunction.
  //! The types of the arguments of this function.
  // protected: mutable ValueTypeArray argTypes;
  //! The type specifications of the arguments of this function. This has to
  //! be set by the overriding function.
  protected: mutable ValueTypeSpecArray argTypeSpecs;
  //! The signature of this function.
  private: mutable SharedPtr<FunctionSignature> sig;


  //============================================================================
  // Member Function

  /**
   * Retrieves the name of this callable.
   * @return The name of the callable.
   */
  public: virtual const std::string &getName() const = 0;

  /**
  * Retrieves the type specifications of the arguments of this function.
  * @return An array containing the type specifications of the arguments.
  */
  public: virtual const ValueTypeSpecArray &getArgumentTypeSpecs() const
  {
    return argTypeSpecs;
  }

  /**
  * Returns a value determining whether this function has a variable number of
  * arguments.
  * @return True or false.
  */
  public: virtual bool isVarArgs() const
  {
    // Most functions have a fixed number of arguments, so we return false by
    // default.
    return false;
  }

  /**
  * Retrieves the signature of this function.
  * @return The signature of the function.
  */
  public: virtual FunctionSignature const& getSignature() const
  {
    if (sig == 0) {
      sig = std::make_shared<FunctionSignature>(getName(), getArgumentTypeSpecs(), isVarArgs());
    }

    return *sig;
  }
  public: virtual FunctionSignature& getSignature()
  {
    return const_cast<FunctionSignature&>(static_cast<const Function*>(this)->getSignature());
  }

  // TODO: Can we convert the return type to llvm::Instruction.
  /**
   * Creates an LLVM instruction that reflects a call to this callable.
   * @param[in] args A list containing the arguments to pass this callable.
   * @return A pointer to the LLVM instruction. This has to be freed by the
   * caller.
   */
  public: virtual llvm::Value *createLLVMInstruction(llvm::IRBuilder<> *irb,
      const std::vector<llvm::Value*> &args) const = 0;

}; // class

} // namespace

#endif
