/**
 * @file Scg/CodeGeneration/FunctionalExpression.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Boost header files
#include <boost/lexical_cast.hpp>

// Scg header files
#include <Containers/List.h>
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionalExpression.h>
#include <CodeGeneration/ParamPassExp.h>
#include <Instructions/CallFunction.h>
#include <Operators/Content.h>
#include <Operators/PointerToArrayElement.h>
#include <Operators/PointerToMemberField.h>
#include <Operators/PointerToVariable.h>
#include <Instructions/DeclareExtFunction.h>

namespace Scg
{
using namespace Core::Standard;
using namespace Core::Basic;

FunctionalExpression::FunctionalExpression(CodeGenerator *gen,
    const SharedPtr<ParsedList> &item) : gen(gen)
{
  if (item->getProdId() != gen->GetFunctionalExpId())
    THROW_EXCEPTION(InvalidArgumentException,
        "Functional expressions can be constructed from Expression.FunctionalExp only.");

  for (auto i = 0; i < item->getElementCount(); i++)
    this->subExprs.push_back(item->getElement(i).s_cast<ParsedItem>());
}

//------------------------------------------------------------------------------

DeclareExtFunction *FunctionalExpression::ToDeclareExtFunction(
    ValueTypeSpec *retType)
{
  if (this->subExprs.size() != 2)
    THROW_EXCEPTION(SystemException, "Invalid function link.");
  auto name = this->gen->ParseToken(this->subExprs[0]);
  auto args = ParamPassExp(
        this->gen, this->subExprs[1].s_cast<ParsedRoute>()).ParseValueTypes();

  // Determine whether the function being linked to has a variable number of
  // arguments or not, e.g. printf.
  // TODO: Remove dynamic_cast.
  auto isVarArgs = false;
  auto typeByName = dynamic_cast<ValueTypeSpecByName*>(args[args.size()-1]);
  if (typeByName != nullptr && typeByName->GetName().compare("args") == 0)
  {
    isVarArgs = true;
    args.pop_back();
  }

  auto declFunc = new DeclareExtFunction(name, retType, args, isVarArgs);
  declFunc->SetLineInCode(this->subExprs[0]->getLine());
  declFunc->SetColumnInCode(this->subExprs[0]->getColumn());
  return declFunc;
}

//------------------------------------------------------------------------------

Expression *FunctionalExpression::ToExpression()
{
  if (this->subExprs[0]->getProdId() != this->gen->GetSubjectId())
    THROW_EXCEPTION(SyntaxErrorException, "Invalid expression. Expressions "
        "should start with a token representing a variable or a function.");

  Expression *expr = nullptr;
  auto cancelNextContentOp = false;
  for (auto i = 0; i < this->subExprs.size(); i++)
  {
    auto thisExprAst = this->subExprs[i];
    if (i < (this->subExprs.size() - 1))
    {
      auto nextExprAst = this->subExprs[i+1];
      if (thisExprAst->getProdId() == this->gen->GetSubjectId() &&
          nextExprAst->getProdId() == this->gen->GetParamPassId() &&
          nextExprAst.s_cast<ParsedRoute>()->getRoute() == ParamPassExp::Parenthesis)
      {
        // A token followed by parentheses. This is a function call.
        ParamPassExp params(this->gen, nextExprAst.s_cast<ParsedRoute>());
        expr = new CallFunction(this->gen->ParseToken(thisExprAst),
            new List(params.ParseExpressionList()));
        expr->SetLineInCode(thisExprAst->getLine());
        expr->SetColumnInCode(thisExprAst->getColumn());
        i++;
        cancelNextContentOp = true;
        continue;
      }
    }
    if (thisExprAst->getProdId() == this->gen->GetSubjectId())
    {
      auto varName = this->gen->ParseToken(thisExprAst);
      expr = new PointerToVariable(varName);
      expr->SetLineInCode(thisExprAst->getLine());
      expr->SetColumnInCode(thisExprAst->getColumn());
    }
    else if (thisExprAst->getProdId() == this->gen->GetParamPassId() &&
        thisExprAst.s_cast<ParsedRoute>()->getRoute() == ParamPassExp::Square)
    {
      // Square brackets. This is array element access.
      auto index = ParseElementIndex(thisExprAst.s_cast<ParsedList>());
      expr = new PointerToArrayElement(expr, index);
      expr->SetLineInCode(thisExprAst->getLine());
      expr->SetColumnInCode(thisExprAst->getColumn());
    }
    else if (thisExprAst->getProdId() == this->gen->GetLinkExpId())
    {
      // Dot followed by a token. This is a field access.
      auto fieldName = ParseFieldName(thisExprAst.s_cast<ParsedList>());
      expr = new PointerToMemberField(expr, fieldName);
      expr->SetLineInCode(thisExprAst->getLine());
      expr->SetColumnInCode(thisExprAst->getColumn());
    }
    else if (thisExprAst->getProdId() == this->gen->GetPostfixTildeExpId())
    {
      auto child = thisExprAst.s_cast<ParsedList>()->getElement(0).s_cast<ParsedItem>();
      if (child->getProdId() == this->gen->GetContentTildeId())
      {
        // ~cnt
        if (cancelNextContentOp) {
          cancelNextContentOp = false;
        } else {
          expr = new Content(expr);
          expr->SetLineInCode(thisExprAst->getLine());
          expr->SetColumnInCode(thisExprAst->getColumn());
        }
      }
      else if (child->getProdId() == this->gen->GetPointerTildeId())
      {
        // ~ptr
        cancelNextContentOp = true;
      }
      else
      {
        THROW_EXCEPTION(SyntaxErrorException, "Invalid post-fix tilde"
            "an expression.");
      }
    }
  }

  if (cancelNextContentOp)
    return expr;
  else
    return new Content(expr);
}

//------------------------------------------------------------------------------

std::string FunctionalExpression::ParseFieldName(
    const SharedPtr<ParsedList> &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->GetLinkExpId())
    THROW_EXCEPTION(SystemException,
        "Unexpected error while trying to parse a field name.");

  static ParsedDataBrowser fieldNameBrowser(STR("1:Subject.Subject1"));
  auto fieldName = fieldNameBrowser.getChildValue<ParsedItem>(astBlockRoot);
  if (fieldName == nullptr)
    THROW_EXCEPTION(SystemException,
        "Unexpected error while trying to parse a field name.");
  return this->gen->ParseToken(fieldName);
}

//------------------------------------------------------------------------------

Expression *FunctionalExpression::ParseElementIndex(
    const SharedPtr<ParsedList> &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->GetParamPassId())
    THROW_EXCEPTION(SystemException,
        "Unexpected error while trying to parse an array index.");

  static ParsedDataBrowser indexBrowser(STR("0:Expression.Exp"));
  auto index = indexBrowser.getChildValue<ParsedItem>(astBlockRoot);
  if (index == nullptr)
    THROW_EXCEPTION(SystemException,
        "Unexpected error while trying to parse an array index.");
  return this->gen->GenerateExpression(index);
}
}
