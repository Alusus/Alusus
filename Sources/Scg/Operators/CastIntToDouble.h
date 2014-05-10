/**
 * @file Scg/Operators/CastIntToDouble.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CastIntToDouble_h__
#define __CastIntToDouble_h__

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
class CastIntToDouble : public Expression
{
private:
  //! Storing the LLVM cast inst so that it can be freed after code generation.
  llvm::CastInst *llvmCastInst = nullptr;

public:
  /**
   * Construct an integer-to-float casting operator.
   * @param[in] operand The operand of the operator.
   */
  CastIntToDouble(Expression *operand)
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

#endif // __CastIntToDouble_h__
