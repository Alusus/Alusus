/**
 * @file Scg/Operators/AssignmentOperator.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_ASSIGNMENTOPERATOR_H
#define SCG_ASSIGNMENTOPERATOR_H

#include "core.h"
#include <typedefs.h>
#include <exceptions.h>
#include <AstNode.h>
#include <llvm_fwd.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represents a binary operator.
 */
class AssignmentOperator : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(AssignmentOperator, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: SharedPtr<AstNode> leftExpr;
  private: SharedPtr<AstNode> rightExpr;
  //! Storing the LLVM store instruction so that it can be freed after code generation.
  private: llvm::StoreInst *llvmStoreInst = nullptr;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a binary operation.
   * @param[in] lhs The left-hand side of the operator. Must be a variable if the operation is '='.
   * @param[in] rhs The operation; can be '+', '-', '*', or '='.
   */
  public: AssignmentOperator(SharedPtr<AstNode> const &lhs, SharedPtr<AstNode> const &rhs)
    : leftExpr(lhs), rightExpr(rhs)
  {
    OWN_SHAREDPTR(this->leftExpr);
    OWN_SHAREDPTR(this->rightExpr);
  }

  public: virtual ~AssignmentOperator()
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

  //! @copydoc AstNode::callGenerateCode()
  public: virtual CodeGenerationStage callGenerateCode(CodeGenUnit *codeGenUnit)
  {
    // We want to manually call the generateCode() member function of children
    // so we override the default behaviour of callGenerateCode().
    return generateCode(codeGenUnit);
  }

  //! @copydoc Value::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return getRHS()->getValueTypeSpec();
  }

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
