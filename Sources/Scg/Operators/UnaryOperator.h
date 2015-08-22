/**
 * @file Scg/Operators/UnaryOperator.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __UnaryOperator_h__
#define __UnaryOperator_h__

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
class UnaryOperator : public Expression
{
public:
  enum Operator {
    INCREMENT,
    DECREMENT,
    NEGATIVE,
  };

private:
  //! The operator represented by the instance ('++' or '--')
  Operator operatorType;
  //! Storing the binary operator so that it can be freed after code generation.
  llvm::Value *llvmValue = nullptr;
  //! Storing the LLVM Load instruction so that it can be freed after code generation.
  //llvm::LoadInst *llvmLoadInst = nullptr;
  //! Storing the LLVM Store instruction so that it can be freed after code generation.
  llvm::StoreInst *llvmStoreInst = nullptr;

public:
  /**
   * Construct a binary operation.
   *
   * @param[in] op        The operation.
   * @param[in] operand   The operand to apply the operator on.
   */
  UnaryOperator(Operator op, Expression *operand) : operatorType(op)
  {
    if (operatorType < INCREMENT || operatorType > NEGATIVE)
      throw EXCEPTION(ArgumentOutOfRangeException, "Invalid unary operator.");

    children.push_back(operand);
  }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const Expression *getOperand() const
  {
    return children[0];
  }
  Expression *getOperand()
  {
    return children[0];
  }

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __UnaryOperator_h__
