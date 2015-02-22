/**
 * @file Scg/CodeGeneration/FunctionAstBlock.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __FunctionAstBlock_h__
#define __FunctionAstBlock_h__

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
  class DefineFunction;
  class ListExpression;
  class StatementList;
  class Block;

  // TODO: The name Function can easily have conflict with other
  // parts of the code, e.g. if we add Function class to the IR
  // language just like Value, Variable, etc.
  /**
   * Parses an AST block representing a function definition.
   *
   * An example of an AST block is shown below, which is generated
   * from the following line of code:
   *
   * def add : function(i:float, j:float ::> float) { return i+j; };
   *
   * Function -- [LIST]:
   *  Subject.Subject1 -- [ROUTE]: 1
   *   Expression.Exp -- [LIST]:
   *    Expression.LowestLinkExp -- [LIST]:
   *     Expression.ListExp -- [LIST]:
   *      Expression.LowerLinkExp -- [LIST]:
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("i")
   *       [TOKEN]: CONSTANT_59 (":")
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")
   *      Expression.LowerLinkExp -- [LIST]:
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("j")
   *       [TOKEN]: CONSTANT_59 (":")
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")
   *     [TOKEN]: CONSTANT_63 ("::>")                                (Optional)
   *     Subject.Subject1 -- [ROUTE]: 0                              (Optional)
   *      Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("float")   (Optional)
   *  Subject.Subject1 -- [ROUTE]: 0
   *   StatementList -- [LIST]:
   *    Return -- [LIST]:
   *     Expression.Exp -- [LIST]:
   *      Expression.AddExp -- [LIST]:
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("i")
   *       [TOKEN]: CONSTANT_21 ("+")
   *       Subject.Subject1 -- [ROUTE]: 0
   *        Subject.Parameter -- [TOKEN]: IDENTIFIER_TOKEN ("j")
   *
   * If the function doesn't have a return value, then the optional lines won't
   * be generated.
   */
  class FunctionAstBlock
  {
  private:
    VariableDefinitionArray arguments;
    ValueTypeSpec *returnType = nullptr;
    Block *body;
    bool autoDelete;
    int lineInCode;
    int columnInCode;

  public:
    /**
     * Constructs a Function AST block from the given generator and
     * AST block root.
     * @param[in] gen     A pointer to the code generator.
     * @param[in] astRoot A pointer to the AST block root.
     */
    FunctionAstBlock(CodeGenerator *gen,
        const std::shared_ptr<Core::Data::ParsedList> &astRoot);

    //! Class destructor.
    ~FunctionAstBlock();

    /**
     * Creates a DefineFunction instruction from this AST block.
     * @param[in] name  A string containing the name of the function
     * @return A pointer to the DefineFunction instruction. The caller has to
     * delete the memory allocated for this object.
     */
    DefineFunction *ToDefineFunction(Char const *name);
  };
}

#endif // __FunctionAstBlock_h__
