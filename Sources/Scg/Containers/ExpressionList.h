/**
 * @file Scg/Containers/ExpressionList.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ExpressionList_h__
#define __ExpressionList_h__

// Scg header files
#include <typedefs.h>
#include <Expression.h>

namespace Scg
{
  /**
   * Group a list of expressions under a single expression.
   */
  class ExpressionList : public Expression
  {
  public:
    /**
     * Constructs an empty expression list.
     */
    ExpressionList() {}

    /**
     * Constructs an expression list with the given expressions.
     */
    ExpressionList(Expression *firstExpr, ...);

    /**
     * Constructs an expression list with the given expressions.
     */
    ExpressionList(const ExpressionArray &body);

    /**
     * Prepends the given expression at the beginning of the expression list.
     *
     * @param[in] expr  A pointer to the expression.
     */
    void PrependExpression(Expression *expr)
    {
      children.insert(children.begin(), expr);
    }

    /**
     * Append the given expression to the end of the expression list.
     *
     * @param[in] expr  A pointer to the expression.
     */
    void AppendExpression(Expression *expr)
    {
      children.push_back(expr);
    }

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __ExpressionList_h__
