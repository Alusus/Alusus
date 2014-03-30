/**
 * @file Scg/CodeGeneration/ParamPassExp.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/ListExpression.h>
#include <CodeGeneration/ParamPassExp.h>
#include <exceptions.h>

namespace Scg
{
using namespace Core::Standard;
using namespace Core::Basic;

ParamPassExp::ParamPassExp(CodeGenerator *gen,
    const SharedPtr<ParsedRoute> &astBlockRoot) : gen(gen)
{
  if (astBlockRoot->getProdId() != gen->GetParamPassId())
    THROW_EXCEPTION(InvalidArgumentException,
        "Parameter pass expressions can be constructed from Expression.ParamPassExp only.");
  this->route = astBlockRoot->getRoute();

  static ParsedDataBrowser singleParamBrowser(STR("Expression.Exp>0"));
  static ParsedDataBrowser multiParamBrowser(
      STR("Expression.Exp>0:Expression.ListExp"));

  SharedPtr<ParsedItem> exp;
  SharedPtr<ParsedList> listExprItem;
  if ((listExprItem = multiParamBrowser.getChildValue<ParsedList>(astBlockRoot))
      != 0)
  {
    auto listExpr = ListExpression(gen, listExprItem);
    for (auto i = 0; i < listExpr.GetItemCount(); i++)
      this->params.push_back(listExpr.GetItem(i));
  }
  else if ((exp = singleParamBrowser.getChildValue<ParsedItem>(astBlockRoot)) != 0)
  {
    this->params.push_back(exp);
  }
  else {
    // No parameter is passed, we don't need to do anything.
  }
}

//------------------------------------------------------------------------------

ExpressionArray ParamPassExp::ParseExpressionList() const
{
  ExpressionArray exprs;
  for (auto i = 0; i < GetParamCount(); i++)
    exprs.push_back(this->gen->GenerateExpression(GetParam(i)));
  return exprs;
}

//------------------------------------------------------------------------------

StringArray ParamPassExp::ParseTokenList() const
{
  StringArray tokens;
  for (auto i = 0; i < GetParamCount(); i++)
    tokens.push_back(this->gen->ParseToken(GetParam(i)));
  return tokens;
}

//------------------------------------------------------------------------------

ValueTypeSpecArray ParamPassExp::ParseValueTypes() const
{
  ValueTypeSpecArray tokens;
  for (auto i = 0; i < GetParamCount(); i++)
    tokens.push_back(this->gen->ParseVariableType(GetParam(i)));
  return tokens;
}
}
