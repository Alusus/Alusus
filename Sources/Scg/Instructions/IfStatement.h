/**
 * @file Scg/Instructions/IfStatement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __IfStatement_h__
#define __IfStatement_h__

// Scg header files
#include <Expression.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
  class Block;
}

namespace Scg
{
  /**
   * Represents a function definition, i.e. a prototype and body.
   */
  class IfStatement : public Expression
  {
  private:
    Expression *condition;
    Block *thenBlock;
    Block *elseBlock;
    Block *mergeBlock;
    llvm::ICmpInst *cmpInst;
    llvm::BranchInst *ifBranch;
    llvm::BranchInst *thenBranch;
    llvm::BranchInst *elseBranch;

  public:
    /**
     * Construct a function with the given name, arguments, and body.
     *
     * @param[in] condition The condition of the if statement.
     * @param[in] thenBlock The block to be executed if the condition evaluates to true.
     * @param[in] elseBlock The block to be executed if the condition evaluates to false.
     */
    IfStatement(Expression *condition, Block *thenBlock, Block *elseBlock);

    /**
     * Class destructor
     */
    ~IfStatement();

    /**
     * Get the block to be executed if the condition evaluates to true.
     *
     * @return A pointer to the 'then' block.
     */
    const Expression *GetCondition() const { return condition; }
    Expression *GetCondition() { return condition; }

    /**
     * Get the block to be executed if the condition evaluates to true.
     *
     * @return A pointer to the 'then' block.
     */
    const Block *GetThenBlock() const { return thenBlock; }
    Block *GetThenBlock() { return thenBlock; }

    /**
     * Get the block to be executed if the condition evaluates to true.
     *
     * @return A pointer to the 'else' block.
     */
    const Block *GetElseBlock() const { return elseBlock; }
    Block *GetElseBlock() { return elseBlock; }

    const Block *GetMergeBlock() const { return mergeBlock; }
    Block *GetMergeBlock() { return mergeBlock; }

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationResult GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __IfStatement_h__
