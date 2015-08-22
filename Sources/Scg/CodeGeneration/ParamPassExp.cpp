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

namespace Scg
{
using namespace Core::Basic;
using namespace Core::Data;

ParamPassExp::ParamPassExp(CodeGenerator *gen,
                           const SharedPtr<ParsedRoute> &astBlockRoot) : gen(gen)
{
  if (astBlockRoot->getProdId() != gen->getParamPassId())
    throw EXCEPTION(InvalidArgumentException,
                    "Parameter pass expressions can be constructed from Expression.ParamPassExp only.");

  this->route = astBlockRoot->getRoute();

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> singleParamReference = ReferenceParser::parseQualifier(
        STR("{find prodId=Expression.Exp, 0}.0"),
        ReferenceUsageCriteria::MULTI_DATA);
  static SharedPtr<Reference> multiParamReference = ReferenceParser::parseQualifier(
        STR("{find prodId=Expression.Exp, 0}.0~where(prodId=Expression.ListExp)"),
        ReferenceUsageCriteria::MULTI_DATA);

  SharedPtr<IdentifiableObject> exp;
  SharedPtr<ParsedList> listExprItem;

  if ((listExprItem = getSharedPtr(seeker.tryGet(multiParamReference.get(), astBlockRoot.get()))
                      .io_cast<ParsedList>()) != 0) {
    auto listExpr = ListExpression(gen, listExprItem);

    for (auto i = 0; i < listExpr.getItemCount(); i++)
      this->params.push_back(listExpr.getItem(i));
  } else if ((exp = getSharedPtr(seeker.tryGet(singleParamReference.get(), astBlockRoot.get()))) != 0) {
    this->params.push_back(exp);
  } else {
    // No parameter is passed, we don't need to do anything.
  }
}

//------------------------------------------------------------------------------

ExpressionArray ParamPassExp::parseExpressionList() const
{
  ExpressionArray exprs;

  for (auto i = 0; i < getParamCount(); i++)
    exprs.push_back(this->gen->generateExpression(getParam(i)));

  return exprs;
}

//------------------------------------------------------------------------------

StringArray ParamPassExp::parseTokenList() const
{
  StringArray tokens;

  for (auto i = 0; i < getParamCount(); i++)
    tokens.push_back(this->gen->parseToken(getParam(i)));

  return tokens;
}

//------------------------------------------------------------------------------

ValueTypeSpecArray ParamPassExp::parseValueTypes() const
{
  ValueTypeSpecArray tokens;

  for (auto i = 0; i < getParamCount(); i++)
    tokens.push_back(this->gen->parseVariableType(getParam(i)));

  return tokens;
}
}
