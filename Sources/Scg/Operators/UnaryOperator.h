/**
 * @file Scg/Operators/UnaryOperator.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_UNARYOPERATOR_H
#define SCG_UNARYOPERATOR_H

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
class UnaryOperator : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnaryOperator, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Data Types

  public: enum Operator {
    INCREMENT,
    DECREMENT,
    NEGATIVE,
  };


  //============================================================================
  // Member Variables

  //! The operator represented by the instance ('++' or '--')
  private: Operator operatorType;
  private: SharedPtr<AstNode> operand;
  //! Storing the binary operator so that it can be freed after code generation.
  private: llvm::Value *llvmValue = nullptr;
  //! Storing the LLVM Load instruction so that it can be freed after code generation.
  //llvm::LoadInst *llvmLoadInst = nullptr;
  //! Storing the LLVM Store instruction so that it can be freed after code generation.
  private: llvm::StoreInst *llvmStoreInst = nullptr;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a binary operation.
   *
   * @param[in] op        The operation.
   * @param[in] operand   The operand to apply the operator on.
   */
  public: UnaryOperator(Operator op, SharedPtr<AstNode> const &operand) : operatorType(op), operand(operand)
  {
    if (operatorType < INCREMENT || operatorType > NEGATIVE)
      throw EXCEPTION(ArgumentOutOfRangeException, "Invalid unary operator.");

    OWN_SHAREDPTR(this->operand);
  }

  public: virtual ~UnaryOperator()
  {
    DISOWN_SHAREDPTR(this->operand);
  }


  //============================================================================
  // Member Functions

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  public: SharedPtr<AstNode> const& getOperand() const
  {
    return this->operand;
  }

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

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
    return 1;
  }

  public: virtual TiObject* get(Int index) const
  {
    if (index == 0) return this->operand.get();
    else return 0;
  }

}; // class

} // namespace

#endif
