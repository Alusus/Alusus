/**
 * @file Scg/Instructions/Return.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Return_h__
#define __Return_h__

// Scg header files
#include <typedefs.h>
#include <Expression.h>
#include <llvm_fwd.h>

namespace Scg
{
  /**
   * Represent a return statement.
   */
  class Return : public Expression
  {
    //! A pointer to the LLVM Return instruction representing this return statement.
    llvm::ReturnInst *retInst;

  public:
    // TODO: This expression should be a constant, because we only need to generate
    // IR code out of it. This means that the GenerateCode() method should be
    // converted to a constant method, and any variable that is changed inside the
    // method should be mutable. Also, do the same for any other places where an
    // expression is only needed for generating IR code.

    /**
     * Construct a return statement that returns the value of the given expression.
     *
     * @param[in] expression  A pointer to the expression to be returned. Notice that
     *                        this expression will automatically get deleted, so it
     *                        should be allocated in the heap and not deleted.
     */
    Return(Expression *expression) : retInst(0)
    {
      children.push_back(expression);
    }

    /**
     * Get the expression this statement should return.
     *
     * @return A pointer to the expression.
     */
    const ExpressionArray::value_type GetExpression() const { return children[0]; }
    ExpressionArray::value_type GetExpression() { return children[0]; }

    //! @copydoc Expression::GetValueType()
    virtual const ValueType *GetValueType() const
    {
      return GetExpression()->GetValueType();
    }

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __Return_h__
