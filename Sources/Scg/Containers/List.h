/**
 * @file Scg/Containers/List.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __List_h__
#define __List_h__

#include <typedefs.h>
#include <Expression.h>
#include <llvm_fwd.h>

namespace Scg
{
// TODO: Why is List derived from Expression?
// TODO: Why do we need ExpressionArray if we already have this class?
/**
 * Represent a return statement.
 */
class List : public Expression
{
public:
  /**
   * Construct a return statement that returns the value of the given expression.
   *
   * @param[in] expression  A pointer to the expression to be returned. Notice that
   *                        this expression will automatically get deleted, so it
   *                        should be allocated in the heap and not deleted.
   */
  List(const ExpressionArray &elements)
  {
    this->children = elements;
  }

  /**
   * Get the nth element of this list.
   *
   * @return A pointer to the nth element of this list.
   */
  const ExpressionArray::value_type getElement(ExpressionArray::size_type n) const
  {
    return children[n];
  }
  ExpressionArray::value_type getElement(ExpressionArray::size_type n)
  {
    return children[n];
  }

  /**
   * Get the number of elements in the list.
   *
   * @return The number of elements in the list.
   */
  ExpressionArray::size_type getElementCount() const
  {
    return children.size();
  }

  //! @copydoc Expression::generateCode()
  virtual Expression::CodeGenerationStage generateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __Return_h__
