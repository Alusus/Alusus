/**
 * @file Scg/Instructions/GotoStatement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __GotoStatement_h__
#define __GotoStatement_h__

// Scg header files
#include <exceptions.h>
#include <typedefs.h>
#include <Expression.h>
#include <llvm_fwd.h>

namespace Scg
{
/**
 * Represent a return statement.
 */
class GotoStatement : public Expression
{
  //! A pointer to the LLVM GotoStatement instruction representing this return statement.
  llvm::BranchInst *branchInst;
  //! A pointer to the block this statement is jumping to.
  Block *block;

public:
  /**
   * Construct a return statement that returns the value of the given expression.
   *
   * @param[in] expression  A pointer to the expression to be returned. Notice that
   *                        this expression will automatically get deleted, so it
   *                        should be allocated in the heap and not deleted.
   */
  GotoStatement(Block *block) : branchInst(0), block(block)
  {
    if (block == 0)
      throw EXCEPTION(ArgumentOutOfRangeException, "Goto statement needs a "
                      "target block to jump to.");
  }

  /**
   * Get the block this statement is jumping to.
   *
   * @return A pointer to the block.
   */
  const Block *getTargetBlock() const
  {
    return block;
  }
  Block *getTargetBlock()
  {
    return block;
  }

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();
};
}

#endif // __Return_h__
