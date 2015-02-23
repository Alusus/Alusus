/**
 * @file Scg/BuiltInFunctions/AddFloats.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __AddFloats_h__
#define __AddFloats_h__

// SCG header files
#include <exceptions.h>
#include <BuiltInFunctions/Function.h>

namespace Scg
{
/**
 * A built-in function to add two integer values.
 */
class AddFloats : public Function
{
private:
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;
  //! Storing the name of the function.
  std::string name { "__op_add" };

public:
  AddFloats();
  ~AddFloats();

  //! @copydoc Function::GetName()
  virtual const std::string &GetName() const { return name; }

  //! @copydoc Function::GetArgumentType()
  virtual const ValueType *GetArgumentType(int n) const;

  //! @copydoc Function::GetArgumentCount()
  ExpressionArray::size_type GetArgumentCount() const { return 2; }

  //! @copydoc Function::CreateLLVMInstruction()
  virtual llvm::Value *CreateLLVMInstruction(llvm::IRBuilder<> *irb,
      const std::vector<llvm::Value*> &args) const override;

  //! @copydoc Expression::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override;
};
}

#endif // __AddFloats_h__
