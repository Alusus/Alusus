/**
 * @file Scg/Operators/CastToFloat.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CastToFloat_h__
#define __CastToFloat_h__

// Scg header files
#include <Expression.h>
#include <Operators/CastingOperator.h>
#include <exceptions.h>
#include <typedefs.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
/**
 * Represents a binary operator.
 */
class CastToFloat : public CastingOperator
{
private:
  //! Storing the LLVM cast inst so that it can be freed after code generation.
  llvm::CastInst *llvmCastInst = nullptr;

public:
  /**
   * Construct an integer-to-float casting operator.
   * @param[in] operand The operand of the operator.
   */
  CastToFloat(Expression *operand)
  {
    children.push_back(operand);
  }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const ExpressionArray::value_type GetOperand() const { return children[0]; }
  ExpressionArray::value_type GetOperand() { return children[0]; }

  //! @copydoc Expression::GetValueType()
  virtual const ValueType *GetValueType() const;

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationResult GenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __CastToFloat_h__
