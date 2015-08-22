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
  const Expression *getCondExpr() const
  {
    return cond;
  }
  Expression *getCondExpr()
  {
    return cond;
  }

  /**
   * Get the body of the for loop.
   *
   * @return A pointer to the body of the for loop.
   */
  const Block *getBody() const
  {
    return loopBlock;
  }
  Block *getBody()
  {
    return loopBlock;
  }

  //! @copydoc Expression::callGenerateCode()
  virtual CodeGenerationStage callGenerateCode()
  {
    // We want to manually call the generateCode() member function of children
    // so we override the default behaviour of callGenerateCode();
    return generateCode();
  }

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __WhileStatement_h__
