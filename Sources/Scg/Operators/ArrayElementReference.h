/**
 * @file Scg/Operators/ArrayElementReference.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_ARRAYELEMENTREFERENCE_H
#define SCG_ARRAYELEMENTREFERENCE_H

#include "core.h"
#include <typedefs.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{

class Block;
class Value;
class CodeGenUnit;

/**
 * Represents a reference to a variable by name.
 */
class ArrayElementReference : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArrayElementReference, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  /*! A pointer to the expression evaluating to an array containing the element
    this pointer instance is pointing to. */
  protected: SharedPtr<AstNode> expression;
  //! The index of the element to point to.
  protected: SharedPtr<AstNode> elementIndexExpr;
  //! A pointer to the LLVM pointer object pointing to the array element.
  protected: llvm::Value *llvmPointer = nullptr;
  //! Storing the value type to avoid fetching it frequently.
  protected: mutable ValueType *valueType = nullptr;


  //============================================================================
  // Constructors & Destructor

  // TODO: Change elementIndex to Expression to allow run-time integral
  // expressions to be used to access array elements.
  /**
   * Constructs pointer to an element within an array.
   * @param[in] pointer       A pointer to an array to get a pointer to one
   *                          one of its elements.
   * @param[in] elementIndex  The index of the element to create a pointer to.
   */
  public: ArrayElementReference(SharedPtr<AstNode> const &expression, SharedPtr<AstNode> const &elementIndexExpr) :
    expression(expression), elementIndexExpr(elementIndexExpr)
  {
    OWN_SHAREDPTR(this->expression);
    OWN_SHAREDPTR(this->elementIndexExpr);
  }

  public: virtual ~ArrayElementReference()
  {
    DISOWN_SHAREDPTR(this->expression);
    DISOWN_SHAREDPTR(this->elementIndexExpr);
  }


  //============================================================================
  // Member Functions

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, TiObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 2;
  }

  public: virtual TiObject* get(Int index) const
  {
    switch (index) {
      case 0: return this->expression.get();
      case 1: return this->elementIndexExpr.get();
      default: return 0;
    }
  }

}; // class

} // namespace

#endif
