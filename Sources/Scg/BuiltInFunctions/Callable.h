/**
 * @file Scg/BuiltInFunctions/Callable.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Callable_h__
#define __Callable_h__

// SCG header files
#include <Containers/List.h>
#include <Values/Value.h>

// LLVM header files
#pragma warning(disable: 4146 4800 4355 4996)
#include <llvm/IR/IRBuilder.h>
#pragma warning(default: 4146 4800 4355 4996)
//#include <llvm_fwd.h>

namespace Scg
{
/**
 * Base class for all expressions that can be called and return a value.
 * Such expressions include built-in functions and user-defined functions.
 */
class Callable : public Value
{
public:
  /**
   * Retrieves the name of this callable.
   * @return The name of the callable.
   */
  virtual const std::string &GetName() const = 0;

	//@{
  /**
   * Retrieves the type of the nth argument of this callable.
   * @return The type of the nth argument.
   */
  virtual const ValueType *GetArgumentType(int n) const = 0;
	//@}

  /**
   * Retrieves the number of arguments passed to this callable.
   * @return The number of arguments passed to this callable.
   */
  virtual ExpressionArray::size_type GetArgumentCount() const = 0;

  // TODO: Can we convert the return type to llvm::Instruction.
  /**
   * Creates an LLVM instruction that reflects a call to this callable.
   * @param[in] args A list containing the arguments to pass this callable.
   * @return A pointer to the LLVM instruction. This has to be freed by the
   * caller.
   */
  virtual llvm::Value *CreateLLVMInstruction(llvm::IRBuilder<> *irb,
  		const List &args) const = 0;
};
}

#endif // __Callable_h__
