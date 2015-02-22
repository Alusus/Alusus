/**
 * @file Scg/BuiltInFunctions/Function.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Function_h__
#define __Function_h__

// STL header files
#include <vector>

// SCG header files
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
protected:
  // Defining as protected to allow inheritors to specify them. See for example UserDefinedFunction.
  //! The types of the arguments of this function.
  // mutable ValueTypeArray argTypes;
  //! The type specifications of the arguments of this function. This has to
  //! be set by the overriding function.
  mutable ValueTypeSpecArray argTypeSpecs;
private:
  //! The signature of this function.
  mutable FunctionSignature *sig = nullptr;

public:
  /**
  * Class destructor.
  */
  virtual ~Function()
  {
    if (sig != nullptr) {
      delete sig;
    }
  }

  /**
   * Retrieves the name of this callable.
   * @return The name of the callable.
   */
  virtual const std::string &GetName() const = 0;

  /**
  * Retrieves the type specifications of the arguments of this function.
  * @return An array containing the type specifications of the arguments.
  */
  virtual const ValueTypeSpecArray &GetArgumentTypeSpecs() const
  {
    return argTypeSpecs;
  }

  /**
  * Returns a value determining whether this function has a variable number of
  * arguments.
  * @return True or false.
  */
  virtual bool IsVarArgs() const
  {
    // Most functions have a fixed number of arguments, so we return false by
    // default.
    return false;
  }

  //@{
  /**
  * Retrieves the signature of this function.
  * @return The signature of the function.
  */
  virtual const FunctionSignature &GetSignature() const
  {
    if (sig == nullptr) {
      sig = new FunctionSignature(GetName(), GetArgumentTypeSpecs(), IsVarArgs());
    }
    return *sig;
  }
  virtual FunctionSignature &GetSignature()
  {
    return const_cast<FunctionSignature&>(
        static_cast<const Function*>(this)->GetSignature());
  }
  //@}

  // TODO: Can we convert the return type to llvm::Instruction.
  /**
   * Creates an LLVM instruction that reflects a call to this callable.
   * @param[in] args A list containing the arguments to pass this callable.
   * @return A pointer to the LLVM instruction. This has to be freed by the
   * caller.
   */
  virtual llvm::Value *CreateLLVMInstruction(llvm::IRBuilder<> *irb,
  		const std::vector<llvm::Value*> &args) const = 0;
};
}

#endif // __Function_h__
