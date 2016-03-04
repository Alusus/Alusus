/**
 * @file Scg/Operators/BinaryOperator.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_BINARYOPERATOR_H
#define SCG_BINARYOPERATOR_H

#include "core.h"
#include <typedefs.h>
#include <exceptions.h>
#include <AstNode.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represents a binary operator.
 */
class BinaryOperator : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(BinaryOperator, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Data Types

  public: enum Operator {
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


  //============================================================================
  // Member Variables

  private: SharedPtr<AstNode> leftExpr;
  private: SharedPtr<AstNode> rightExpr;
  //! The operator represented by the instance ('+', '-', etc.)
  private: Operator opType;
  //! Storing the binary operator so that it can be freed after code generation.
  private: llvm::Value *llvmValue = nullptr;


  //============================================================================
  // Cosntructors & Destructor

  /**
   * Construct a binary operation.
   *
   * @param[in] op  The operation; can be '+', '-', '*', or '='.
   * @param[in] lhs The left-hand side of the operator. Must be a variable if the operation is '='.
   * @param[in] rhs The operation; can be '+', '-', '*', or '='.
   */
  public: BinaryOperator(Operator op, SharedPtr<AstNode> const &lhs, SharedPtr<AstNode> const &rhs)
    : opType(op), leftExpr(lhs), rightExpr(rhs)
  {
    if (opType < ADD || opType > NOTEQUAL)
      throw EXCEPTION(ArgumentOutOfRangeException, "Invalid binary operator.");

    OWN_SHAREDPTR(this->leftExpr);
    OWN_SHAREDPTR(this->rightExpr);
  }

  public: virtual ~BinaryOperator()
  {
    DISOWN_SHAREDPTR(this->leftExpr);
    DISOWN_SHAREDPTR(this->rightExpr);
  }


  //============================================================================
  // Member Functions

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  public: SharedPtr<AstNode> const& getLHS() const
  {
    return this->leftExpr;
  }

  /**
   * Get the expression representing the right-hand side of the binary operator.
   *
   * @return A pointer to the right-hand side expression.
   */
  public: SharedPtr<AstNode> const& getRHS() const
  {
    return this->rightExpr;
  }

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, IdentifiableObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 2;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    switch (index) {
      case 0: return this->leftExpr.get();
      case 1: return this->rightExpr.get();
      default: return 0;
    }
  }

}; // class

} // namespace

#endif
