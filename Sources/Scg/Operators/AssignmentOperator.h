/**
 * @file Scg/Operators/AssignmentOperator.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __AssignmentOperator_h__
#define __AssignmentOperator_h__

// Scg header files
#include <typedefs.h>
#include <exceptions.h>
#include <Expression.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
/**
 * Represents a binary operator.
 */
class AssignmentOperator : public Expression
{
private:
  //! Storing the LLVM store instruction so that it can be freed after code generation.
  llvm::StoreInst *llvmStoreInst = nullptr;

public:
  /**
   * Construct a binary operation.
   * @param[in] lhs The left-hand side of the operator. Must be a variable if the operation is '='.
   * @param[in] rhs The operation; can be '+', '-', '*', or '='.
   */
  AssignmentOperator(Expression *lhs, Expression *rhs)
  {
    children.push_back(lhs);
    children.push_back(rhs);
  }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const ExpressionArray::value_type GetLHS() const { return children[0]; }
  ExpressionArray::value_type GetLHS() { return children[0]; }

  /**
   * Get the expression representing the right-hand side of the binary operator.
   *
   * @return A pointer to the right-hand side expression.
   */
  const ExpressionArray::value_type GetRHS() const { return children[1]; }
  ExpressionArray::value_type GetRHS() { return children[1]; }

  //! @copydoc Expression::CallGenerateCode()
  virtual CodeGenerationStage CallGenerateCode()
  {
  	// We want to manually call the GenerateCode() member function of children
  	// so we override the default behaviour of CallGenerateCode().
  	return GenerateCode();
  }

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationStage GenerateCode();

  CodeGenerationResult GenerateCodeForAssignment();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __AssignmentOperator_h__
