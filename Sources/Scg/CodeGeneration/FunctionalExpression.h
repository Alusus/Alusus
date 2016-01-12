/**
 * @file Scg/CodeGeneration/FunctionalExpression.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __FunctionalExpression_h__
#define __FunctionalExpression_h__

// STL header files
#include <vector>

// Alusus Core header files
#include <core.h>

// Alusus SCG header files
#include <CodeGeneration/typedefs.h>
#include <CodeGeneration/ParamPassExp.h>
#include <exceptions.h>

namespace Scg
{
class CodeGenerator;
class DeclareExtFunction;
class ParamPassExp;
class Expression;

/**
 * Represents and help parsing a functional expressions AST block. Functional
 * expressions are the the result of a variety of expressions. For example,
 * the Alusus expression "myarray[3]" generates the following AST block:
 *
 * Expression.FunctionalExp -- [LIST]:
 *  Subject.Subject1 -- [ROUTE]: 0
 *   Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("myarray")
 *  Expression.ParamPassExp -- [ROUTE]: 1
 *   Expression.Exp -- [LIST]:
 *    Subject.Subject1 -- [ROUTE]: 0
 *     Subject.Literal -- [TOKEN]: INT_LITERAL_TOKEN ("3")
 *
 * Other Alusus expressions which result in functional expressions include
 * ~ptr, ~cnt, accessing a structure field, calling a function, linking to an
 * external function, and many . Concatenating any of the previous expression
 * types also result in a functional expression.
 */
class FunctionalExpression
{
private:
  CodeGenerator *gen;
  //! The list of expressions making up this functional expression.
  ParsedItemArray subExprs;
  //std::string name;
  //ParamPassExp *params;

public:
  /**
   * Constructs a functional expression block from the given code generator
   * and AST block root.
   */
  FunctionalExpression(CodeGenerator *gen,
                       const Core::Basic::SharedPtr<Core::Data::PrtList> &astBlockRoot);

  /**
   * If this functional expression is the result of a link expression, this
   * function creates the DeclareExtFunction object that creates the function
   * link.
   *
   * Below are two examples of the AST blocks which are generated from link
   * keywords. The first has a return value and the second doesn't. The part
   * of these blocks that this function parses are the ones having the root
   * Expression.FunctionalExp. Thus, the parsed return value has to be sen
   * to this function.
   *
   * link pow(double, double) : double;
   *
   * pow: Main.Link -- [LIST]:
   *  Expression.Exp -- [LIST]:
   *   Expression.LowerLinkExp -- [LIST]:
   *    Expression.FunctionalExp -- [LIST]:
   *     Subject.Subject1 -- [ROUTE]: 0
   *      Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("pow")
   *     Expression.ParamPassExp -- [ROUTE]: 0
   *      Expression.Exp -- [LIST]:
   *       Expression.ListExp -- [LIST]:
   *        Subject.Subject1 -- [ROUTE]: 0
   *         Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("double")
   *        Subject.Subject1 -- [ROUTE]: 0
   *         Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("double")
   *    [TOKEN]: CONSTANT_59 (":")
   *    Subject.Subject1 -- [ROUTE]: 0
   *     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("double")
   *
   * link cos(double);
   *
   * cos: Main.Link -- [LIST]:
   *  Expression.Exp -- [LIST]:
   *   Expression.FunctionalExp -- [LIST]:
   *    Subject.Subject1 -- [ROUTE]: 0
   *     Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("cos")
   *    Expression.ParamPassExp -- [ROUTE]: 0
   *     Expression.Exp -- [LIST]:
   *      Subject.Subject1 -- [ROUTE]: 0
   *       Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("double")
   *
   * @param[in] retType The return type of the function, which is
   *
   * @return A pointer to the DeclareExtFunction instance. This is allocated
   * in the heap, and thus should be deleted by the caller.
   */
  DeclareExtFunction *toDeclareExtFunction(ValueTypeSpec *retType);

  /**
   * If this functional expression is the result of an expression that includes
   * dots (to access members), array subscripts, post-fix tildes, etc., this
   * function parses it and returns the expression representing it.
   *
   * @return A pointer to the Expression instance. This is allocated in the heap,
   * and thus should be deleted by the caller.
   */
  Expression *toExpression();

private:
  Char const *parseFieldName(
    const Core::Basic::SharedPtr<Core::Data::PrtList> &astBlockRoot);
  Expression *parseElementIndex(
    const Core::Basic::SharedPtr<Core::Data::PrtRoute> &astBlockRoot);
};
}

#endif // __FunctionalExpression_h__
