/**
 * @file Scg/Instructions/Return.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_RETURN_H
#define SCG_RETURN_H

#include "core.h"
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent a return statement.
 */
class Return : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(Return, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The expression to return
  private: SharedPtr<AstNode> retExpr;
  //! A pointer to the LLVM Return instruction representing this return statement.
  private: llvm::ReturnInst *retInst;


  //============================================================================
  // Constructors & Destructor

  // TODO: This expression should be a constant, because we only need to generate
  // IR code out of it. This means that the generateCode() method should be
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
  public: Return(SharedPtr<AstNode> const &expression) : retExpr(expression), retInst(0)
  {
    OWN_SHAREDPTR(this->retExpr);
  }

  public: virtual ~Return()
  {
    DISOWN_SHAREDPTR(this->retExpr);
  }


  //============================================================================
  // Member Functions

  /**
   * Get the expression this statement should return.
   *
   * @return A pointer to the expression.
   */
  public: SharedPtr<AstNode> const& getExpression() const
  {
    return this->retExpr;
  }

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return getExpression()->getValueTypeSpec();
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
    return 1;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->retExpr.get();
  }

}; // class

} // namespace

#endif
