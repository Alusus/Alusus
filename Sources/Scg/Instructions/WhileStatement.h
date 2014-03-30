/**
 * @file Scg/Instructions/WhileStatement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __WhileStatement_h__
#define __WhileStatement_h__

// Scg header files
#include <Expression.h>

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
  class WhileStatement : public Expression
  {
  private:
    Expression *cond;
    Block *condBlock;
    Block *loopBlock;
    Block *exitBlock;
    llvm::Instruction *brInst;

  public:
    /**
     * Construct a function with the given name, arguments, and body.
     *
     * @param[in] cond  The condition expression of the for statement.
     * @param[in] body  The body of the for loop.
     */
    WhileStatement(Expression *cond, Block *body);

    /**
     * Get the condition expression of the for loop.
     *
     * @return A pointer to the condition expression of the for loop.
     */
    const Expression *GetCondExpr() const { return cond; }
    Expression *GetCondExpr() { return cond; }

    /**
     * Get the body of the for loop.
     *
     * @return A pointer to the body of the for loop.
     */
    const Block *GetBody() const { return loopBlock; }
    Block *GetBody() { return loopBlock; }

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationResult GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __WhileStatement_h__
