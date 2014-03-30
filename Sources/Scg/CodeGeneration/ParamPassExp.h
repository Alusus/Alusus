/**
 * @file Scg/CodeGeneration/ParamPassExp.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ParamPassExp_h__
#define __ParamPassExp_h__

// STL header files
#include <vector>

// Alusus Core header files
#include <core.h>

// Alusus SCG header files
#include <CodeGeneration/typedefs.h>
#include <exceptions.h>
#include <typedefs.h>

namespace Scg
{
class ParamPassExp
{
public:
  /**
   * The type of the parameter pass expression. Could be either parentheses or
   * square brackets for now.
   */
  enum BracketType
  {
    Parenthesis = 0,
    Square = 1
  };

private:
  CodeGenerator *gen;
  ParsedItemArray params;
  int route;

public:
  ParamPassExp(CodeGenerator *gen,
      const Core::Basic::SharedPtr<Core::Standard::ParsedRoute> &item);

  /**
   * Retrieves the number of parameters in this list expression.
   * @return The number of parameters in this list expression.
   */
  ParsedItemArray::size_type GetParamCount() const
  {
    return params.size();
  }

  /**
   * Retrieves the parameter at the given index.
   * @param[in] index The index of the parameter
   * @return A pointer to the parameter.
   */
  ParsedItemArray::value_type GetParam(ParsedItemArray::size_type index) const
  {
    return params[index];
  }

  /**
   * Retrieves the type of the brackets of this parameter pass expression.
   * @return The type of the bracket.
   */
  BracketType GetRoute() const { return static_cast<BracketType>(route); }

  // TODO: We need to have a generic function that parses a list of expressions,
  // regardless of their types. Then probably we will not even the two function
  // below.

  /**
   * Parses the expressions inside this parameter pass expression and return
   * them as an array.
   * @return An array of the expressions inside this parameter pass expression.
   */
  ExpressionArray ParseExpressionList() const;

  /**
   * If the parameter pass expression is a list of tokens, e.g. variable names,
   * this function return them.
   * @return An array of strings.
   */
  StringArray ParseTokenList() const;

  /**
   * If the parameter pass expression is a list of types, e.g. the
   * variable types in a link statement, this function return them.
   * @return An array of value type IDs.
   */
  ValueTypeSpecArray ParseValueTypes() const;
};
}

#endif // __ParamPassExp_h__
