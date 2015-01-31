/**
 * @file Scg/Instructions/ForStatement.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ForStatement_h__
#define __ForStatement_h__

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
  class ForStatement : public Expression
  {
  private:
    Expression *init;
    Expression *cond;
    Expression *loop;
    Block *initBlock;
    Block *condBlock;
    Block *loopBlock;
    Block *exitBlock;
    llvm::Instruction *brInst;

  public:
    /**
     * Construct a function with the given name, arguments, and body.
     *
     * @param[in] init  The initialization expression of the for statement.
     * @param[in] cond  The condition expression of the for statement.
     * @param[in] loop  The loop expression of the for statement.
     * @param[in] body  The body of the for loop.
     */
    ForStatement(Expression *init, Expression *cond, Expression *loop, Block *body);

    /**
     * Get the initialization expression of the for loop.
     *
     * @return A pointer to the initialization expression of the for loop.
     */
    const Expression *GetInitExpr() const { return init; }
    Expression *GetInitExpr() { return init; }

    /**
     * Get the condition expression of the for loop.
     *
     * @return A pointer to the condition expression of the for loop.
     */
    const Expression *GetCondExpr() const { return cond; }
    Expression *GetCondExpr() { return cond; }

    /**
     * Get the loop expression of the for loop.
     *
     * @return A pointer to the loop expression of the for loop.
     */
    const Expression *GetLoopExpr() const { return loop; }
    Expression *GetLoopExpr() { return loop; }

    /**
     * Get the body of the for loop.
     *
     * @return A pointer to the body of the for loop.
     */
    const Block *GetBody() const { return loopBlock; }
    Block *GetBody() { return loopBlock; }

    //! @copydoc Expression::CallGenerateCode()
    virtual CodeGenerationStage CallGenerateCode()
    {
    	// We want to manually call the GenerateCode() member function of children
    	// so we override the default behaviour of CallGenerateCode();
    	return GenerateCode();
    }

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __ForStatement_h__
