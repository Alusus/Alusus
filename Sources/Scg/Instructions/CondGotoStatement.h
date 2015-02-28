/**
 * @file Scg/Instructions/CondGotoStatement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CondGotoStatement_h__
#define __CondGotoStatement_h__

// Scg header files
#include <typedefs.h>
#include <exceptions.h>
#include <Expression.h>
#include <llvm_fwd.h>

namespace Scg
{
  /**
   * Represent a return statement.
   */
  class CondGotoStatement : public Expression
  {
    //! A pointer to the LLVM CondGotoStatement instruction representing this return statement.
    llvm::BranchInst *branchInst;
    //! The condition that decides where the goto statement is jumping.
    Expression *cond;
    //! A pointer to the block this statement is jumping to if the condition is true.
    Block *trueBlock;
    //! A pointer to the block this statement is jumping to if the condition is false.
    Block *falseBlock;

  public:
    /**
     * Construct a return statement that returns the value of the given expression.
     *
     * @param[in] expression  A pointer to the expression to be returned. Notice that
     *                        this expression will automatically get deleted, so it
     *                        should be allocated in the heap and not deleted.
     */
    CondGotoStatement(Expression *cond, Block *trueBlock, Block *falseBlock)
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

      this->children.push_back(this->cond);
    }

    /**
     * Get the expression of this conditional goto.
     *
     * @return A pointer to the expression.
     */
    const Expression *GetCondition() const { return cond; }
    Expression *GetCondition() { return cond; }

    /**
     * Get the block this statement is jumping to if the condition is true.
     *
     * @return A pointer to the block.
     */
    const Block *GetTrueBlock() const { return trueBlock; }
    Block *GetTrueBlock() { return trueBlock; }

    /**
     * Get the block this statement is jumping to if the condition is false.
     *
     * @return A pointer to the block.
     */
    const Block *GetFalseBlock() const { return falseBlock; }
    Block *GetFalseBlock() { return falseBlock; }

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();
  };
}

#endif // __Return_h__
