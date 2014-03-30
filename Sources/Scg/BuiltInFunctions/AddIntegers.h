/**
 * @file Scg/BuiltInFunctions/AddIntegers.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __AddIntegers_h__
#define __AddIntegers_h__

// Scg header files
#include <typedefs.h>
#include <exceptions.h>
#include <Expression.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
/**
 * A built-in function to add two integer values.
 */
class AddIntegers : public Expression
{
private:
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;

public:
  /**
   * Construct a binary operation.
   * @param[in] lhs The left-hand side of the addition.
   * @param[in] rhs The right-hand side of the addition.
   */
  AddIntegers(Expression *lhs, Expression *rhs)
  {
    this->children.push_back(lhs);
    this->children.push_back(rhs);
  }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const ExpressionArray::value_type GetLHS() const { return this->children[0]; }
  ExpressionArray::value_type GetLHS() { return this->children[0]; }

  /**
   * Get the expression representing the right-hand side of the binary operator.
   *
   * @return A pointer to the right-hand side expression.
   */
  const ExpressionArray::value_type GetRHS() const { return this->children[1]; }
  ExpressionArray::value_type GetRHS() { return this->children[1]; }

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationResult GenerateCode();

  CodeGenerationResult GenerateCodeForAssignment();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __AddIntegers_h__
