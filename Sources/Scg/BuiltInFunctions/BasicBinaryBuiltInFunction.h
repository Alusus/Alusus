/**
 * @file Scg/BuiltInFunctions/BasicBuiltInFunction.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __BasicBuiltInFunction_h__
#define __BasicBuiltInFunction_h__

// STL header files
#include <functional>

// SCG header files
#include <exceptions.h>
#include <BuiltInFunctions/Function.h>

namespace Scg
{
/**
 * A built-in function to add two integer values.
 */
class BasicBinaryBuiltInFunction : public Function
{
public:
  using Callback = std::function<llvm::Value*(llvm::IRBuilder<> *irb, llvm::Value*, llvm::Value*)>;

private:
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;
  //! Storing the name of the function.
  std::string name;
  Callback function;
  ValueTypeSpec *retTypeSpec;

public:
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
  BasicBinaryBuiltInFunction(const Core::Basic::Char *name, const Core::Basic::Char *retType,
      const Core::Basic::Char *arg1Type, const Core::Basic::Char *arg2Type, Callback function);
  ~BasicBinaryBuiltInFunction();

  //! @copydoc Function::GetName()
  virtual const std::string &GetName() const { return name; }

  //! @copydoc Function::GetArgumentType()
  //virtual const ValueType *GetArgumentType(int n) const;

  //! @copydoc Function::GetArgumentCount()
  //virtual ExpressionArray::size_type GetArgumentCount() const { return 2; }

  //! @copydoc Function::CreateLLVMInstruction()
  virtual llvm::Value *CreateLLVMInstruction(llvm::IRBuilder<> *irb,
      const std::vector<llvm::Value*> &args) const override;

  //! @copydoc Expression::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override;
};
}

#endif // __BasicBuiltInFunction_h__
