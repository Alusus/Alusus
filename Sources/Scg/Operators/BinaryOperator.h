/**
 * @file Scg/Operators/BinaryOperator.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __BinaryOperator_h__
#define __BinaryOperator_h__

// Scg header files
#include <typedefs.h>
#include <exceptions.h>
#include <Expression.h>

// LLVM forward declarations
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{
/**
 * Represents a binary operator.
 */
class BinaryOperator : public Expression
{
public:
  enum Operator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVISION,
    GREATERTHAN,
    GREATERTHAN_EQUAL,
    LESSTHAN,
    LESSTHAN_EQUAL,
    EQUAL,
    NOTEQUAL
  };

private:
  //! The operator represented by the instance ('+', '-', etc.)
  Operator opType;
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;

public:
  /**
   * Construct a binary operation.
   *
   * @param[in] op  The operation; can be '+', '-', '*', or '='.
   * @param[in] lhs The left-hand side of the operator. Must be a variable if the operation is '='.
   * @param[in] rhs The operation; can be '+', '-', '*', or '='.
   */
  BinaryOperator(Operator op, Expression *lhs, Expression *rhs)
    : opType(op)
  {
    if (opType < ADD || opType > NOTEQUAL)
      throw EXCEPTION(ArgumentOutOfRangeException, "Invalid binary operator.");

    children.push_back(lhs);
    children.push_back(rhs);
  }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const ExpressionArray::value_type getLHS() const
  {
    return children[0];
  }
  ExpressionArray::value_type getLHS()
  {
    return children[0];
  }

  /**
   * Get the expression representing the right-hand side of the binary operator.
   *
   * @return A pointer to the right-hand side expression.
   */
  const ExpressionArray::value_type getRHS() const
  {
    return children[1];
  }
  ExpressionArray::value_type getRHS()
  {
    return children[1];
  }

  //! @copydoc Expression::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override;

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __BinaryOperator_h__
