/**
 * @file Scg/BuiltInFunctions/AddDoubles.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __AddDoubles_h__
#define __AddDoubles_h__

// SCG header files
#include <exceptions.h>
#include <BuiltInFunctions/Callable.h>

namespace Scg
{
/**
 * A built-in function to add two integer values.
 */
class AddDoubles : public Callable
{
private:
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;
  //! Storing the name of the function.
  std::string name { "__op_add" };

public:
  //! @copydoc Callable::GetName()
  virtual const std::string &GetName() const { return name; }

  //! @copydoc Callable::GetArgumentType()
  virtual const ValueType *GetArgumentType(int n) const;

  //! @copydoc Callable::GetArgumentCount()
  virtual ExpressionArray::size_type GetArgumentCount() const { return 2; }

  //! @copydoc Callable::CreateLLVMInstruction()
  virtual llvm::Value *CreateLLVMInstruction(llvm::IRBuilder<> *irb,
  		const List &args) const;

  //! @copydoc Expression::GetValueType()
  virtual const ValueType *GetValueType() const;
};
}

#endif // __AddDoubles_h__
