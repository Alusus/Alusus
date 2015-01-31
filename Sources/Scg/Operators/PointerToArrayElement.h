/**
 * @file Scg/Operators/PointerToArrayElement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __PointerToArrayElement_h__
#define __PointerToArrayElement_h__

// Alusus header files
#include <typedefs.h>
#include <llvm_fwd.h>

namespace Scg
{
class Block;
class Value;

/**
 * Represents a reference to a variable by name.
 */
class PointerToArrayElement : public Expression
{
protected:
  /*! A pointer to the expression evaluating to an array containing the element
      this pointer instance is pointing to. */
  Expression *expression;
  //! The index of the element to point to.
  Expression *elementIndexExpr;
  //! A pointer to the LLVM pointer object pointing to the array element.
  llvm::Value *llvmPointer = nullptr;
  //! Storing the value type to avoid fetching it frequently.
  mutable ValueType *valueType = nullptr;

public:
  // TODO: Change elementIndex to Expression to allow run-time integral
  // expressions to be used to access array elements.
  /**
   * Constructs pointer to an element within an array.
   * @param[in] pointer       A pointer to an array to get a pointer to one
   *                          one of its elements.
   * @param[in] elementIndex  The index of the element to create a pointer to.
   */
  PointerToArrayElement(Expression *expression, Expression *elementIndexExpr) :
    expression(expression), elementIndexExpr(elementIndexExpr)
  {
    children.push_back(expression);
    children.push_back(elementIndexExpr);
  }

  //! Class destructor.
  ~PointerToArrayElement();

  //! @copydoc Expression::GetValueType()
  virtual const ValueType *GetValueType() const;

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationStage GenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __PointerToArrayElement_h__
