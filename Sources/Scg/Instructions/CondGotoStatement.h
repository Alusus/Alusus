/**
 * @file Scg/Instructions/CondGotoStatement.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CONDGOTOSTATEMENT_H
#define SCG_CONDGOTOSTATEMENT_H

#include "core.h"
#include <typedefs.h>
#include <exceptions.h>
#include <Instructions/Instruction.h>
#include <llvm_fwd.h>

namespace Scg
{

class Block;
class CodeGenUnit;

/**
 * Represent a return statement.
 */
class CondGotoStatement : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(CondGotoStatement, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! A pointer to the LLVM CondGotoStatement instruction representing this return statement.
  private: llvm::BranchInst *branchInst;
  //! The condition that decides where the goto statement is jumping.
  private: SharedPtr<AstNode> cond;
  //! A pointer to the block this statement is jumping to if the condition is true.
  private: Block *trueBlock;
  //! A pointer to the block this statement is jumping to if the condition is false.
  private: Block *falseBlock;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a return statement that returns the value of the given expression.
   *
   * @param[in] expression  A pointer to the expression to be returned. Notice that
   *                        this expression will automatically get deleted, so it
   *                        should be allocated in the heap and not deleted.
   */
  public: CondGotoStatement(SharedPtr<AstNode> const &cond, Block *trueBlock, Block *falseBlock)
    : branchInst(0)
    , cond(cond)
    , trueBlock(trueBlock)
    , falseBlock(falseBlock)
  {
    if (this->cond == 0)
      throw EXCEPTION(ArgumentOutOfRangeException, "Conditional goto statement "
                      "expects a condition!");

    if (this->trueBlock == 0 || this->falseBlock == 0)
      throw EXCEPTION(ArgumentOutOfRangeException, "Conditional goto statement "
                      "expects two blocks!");
    OWN_SHAREDPTR(this->cond);
  }

  public: virtual ~CondGotoStatement()
  {
    DISOWN_SHAREDPTR(this->cond);
  }


  //============================================================================
  // Member Functions

  /**
   * Get the expression of this conditional goto.
   *
   * @return A pointer to the expression.
   */
  public: SharedPtr<AstNode> const& getCondition() const
  {
    return this->cond;
  }

  /**
   * Get the block this statement is jumping to if the condition is true.
   *
   * @return A pointer to the block.
   */
  public: Block* getTrueBlock() const
  {
    return this->trueBlock;
  }

  /**
   * Get the block this statement is jumping to if the condition is false.
   *
   * @return A pointer to the block.
   */
  public: Block* getFalseBlock() const
  {
    return this->falseBlock;
  }

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);


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
    return this->cond.get();
  }

}; // class

} // namespace

#endif
