/**
 * @file Scg/Instructions/GotoStatement.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_GOTOSTATEMENT_H
#define SCG_GOTOSTATEMENT_H

#include "core.h"
#include <exceptions.h>
#include <typedefs.h>
#include <AstNode.h>
#include <llvm_fwd.h>

namespace Scg
{

class Block;
class CodeGenUnit;

/**
 * Represent a return statement.
 */
class GotoStatement : public AstNode
{
  //============================================================================
  // Type Info

  TYPE_INFO(GotoStatement, AstNode, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! A pointer to the LLVM GotoStatement instruction representing this return statement.
  private: llvm::BranchInst *branchInst;
  //! A pointer to the block this statement is jumping to.
  private: Block *block;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a return statement that returns the value of the given expression.
   *
   * @param[in] expression  A pointer to the expression to be returned. Notice that
   *                        this expression will automatically get deleted, so it
   *                        should be allocated in the heap and not deleted.
   */
  public: GotoStatement(Block *block) : branchInst(0), block(block)
  {
    if (block == 0)
      throw EXCEPTION(ArgumentOutOfRangeException, "Goto statement needs a "
                      "target block to jump to.");
  }


  //============================================================================
  // Member Functions

  /**
   * Get the block this statement is jumping to.
   *
   * @return A pointer to the block.
   */
  public: Block const* getTargetBlock() const
  {
    return this->block;
  }
  public: Block* getTargetBlock()
  {
    return this->block;
  }

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

}; // class

} // namespace

#endif
