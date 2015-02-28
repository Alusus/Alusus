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
using namespace Core::Basic;
using namespace Core::Data;

FunctionalExpression::FunctionalExpression(CodeGenerator *gen,
    const SharedPtr<ParsedList> &item) : gen(gen)
{
  if (item->getProdId() != gen->GetFunctionalExpId())
    throw EXCEPTION(InvalidArgumentException,
        "Functional expressions can be constructed from Expression.FunctionalExp only.");

  for (auto i = 0; i < item->getCount(); i++)
    this->subExprs.push_back(item->getShared(i));
}

//------------------------------------------------------------------------------

DeclareExtFunction *FunctionalExpression::ToDeclareExtFunction(
    ValueTypeSpec *retType)
{
  if (this->subExprs.size() != 2)
    throw EXCEPTION(SystemException, "Invalid function link.");
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
  auto metadata = this->subExprs[0]->getInterface<ParsingMetadataHolder>();
  if (metadata != 0) {
    declFunc->SetLineInCode(metadata->getLine());
    declFunc->SetColumnInCode(metadata->getColumn());
  }
  return declFunc;
}

//------------------------------------------------------------------------------

Expression *FunctionalExpression::ToExpression()
{
  auto metadata = this->subExprs[0]->getInterface<ParsingMetadataHolder>();
  if (metadata == nullptr || metadata->getProdId() != this->gen->GetSubjectId())
    throw EXCEPTION(SyntaxErrorException, "Invalid expression. Expressions "
        "should start with a token representing a variable or a function.");

  Expression *expr = nullptr;
  auto cancelNextContentOp = false;
  for (auto i = 0; i < this->subExprs.size(); i++)
  {
    auto thisExprAst = this->subExprs[i];
    auto thisExprAstMeta = thisExprAst->getInterface<ParsingMetadataHolder>();
    if (i < (this->subExprs.size() - 1))
    {
      auto nextExprAst = this->subExprs[i+1];
      auto nextExprAstMeta = nextExprAst->getInterface<ParsingMetadataHolder>();
      if (thisExprAstMeta->getProdId() == this->gen->GetSubjectId() &&
          nextExprAstMeta->getProdId() == this->gen->GetParamPassId() &&
          nextExprAst.s_cast<ParsedRoute>()->getRoute() == ParamPassExp::Parenthesis)
      {
        // A token followed by parentheses. This is a function call.
        ParamPassExp params(this->gen, nextExprAst.s_cast<ParsedRoute>());
        expr = new CallFunction(this->gen->ParseToken(thisExprAst),
            new List(params.ParseExpressionList()));
        expr->SetLineInCode(thisExprAstMeta->getLine());
        expr->SetColumnInCode(thisExprAstMeta->getColumn());
        i++;
        cancelNextContentOp = true;
        continue;
      }
    }
    if (thisExprAstMeta->getProdId() == this->gen->GetSubjectId())
    {
      auto varName = this->gen->ParseToken(thisExprAst);
      expr = new PointerToVariable(varName);
      expr->SetLineInCode(thisExprAstMeta->getLine());
      expr->SetColumnInCode(thisExprAstMeta->getColumn());
    }
    else if (thisExprAstMeta->getProdId() == this->gen->GetParamPassId() &&
        thisExprAst.s_cast<ParsedRoute>()->getRoute() == ParamPassExp::Square)
    {
      // Square brackets. This is array element access.
      auto index = ParseElementIndex(thisExprAst.s_cast<ParsedRoute>());
      expr = new PointerToArrayElement(expr, index);
      expr->SetLineInCode(thisExprAstMeta->getLine());
      expr->SetColumnInCode(thisExprAstMeta->getColumn());
    }
    else if (thisExprAstMeta->getProdId() == this->gen->GetLinkExpId())
    {
      // Dot followed by a token. This is a field access.
      auto fieldName = ParseFieldName(thisExprAst.s_cast<ParsedList>());
      expr = new PointerToMemberField(expr, fieldName);
      expr->SetLineInCode(thisExprAstMeta->getLine());
      expr->SetColumnInCode(thisExprAstMeta->getColumn());
    }
    else if (thisExprAstMeta->getProdId() == this->gen->GetPostfixTildeExpId())
    {
      auto child = ii_cast<ParsingMetadataHolder>(thisExprAst.s_cast<ParsedList>()->get(0));
      if (child->getProdId() == this->gen->GetContentTildeId())
      {
        // ~cnt
        if (cancelNextContentOp) {
          cancelNextContentOp = false;
        } else {
          expr = new Content(expr);
          expr->SetLineInCode(thisExprAstMeta->getLine());
          expr->SetColumnInCode(thisExprAstMeta->getColumn());
        }
      }
      else if (child->getProdId() == this->gen->GetPointerTildeId())
      {
        // ~ptr
        cancelNextContentOp = true;
      }
      else
      {
        throw EXCEPTION(SyntaxErrorException, "Invalid post-fix tilde"
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

Char const* FunctionalExpression::ParseFieldName(
    const SharedPtr<ParsedList> &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->GetLinkExpId())
    throw EXCEPTION(SystemException,
        "Unexpected error while trying to parse a field name.");

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> fieldNameReference = ReferenceParser::parseQualifier(
    STR("1~where(prodId=Subject.Subject1)"),
    ReferenceUsageCriteria::MULTI_DATA);
  auto fieldName = getSharedPtr(seeker.tryGet(fieldNameReference.get(), astBlockRoot.get()));
  if (fieldName == nullptr)
    throw EXCEPTION(SystemException,
        "Unexpected error while trying to parse a field name.");
  return this->gen->ParseToken(fieldName);
}

//------------------------------------------------------------------------------

Expression *FunctionalExpression::ParseElementIndex(
    const SharedPtr<ParsedRoute> &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->GetParamPassId())
    throw EXCEPTION(SystemException,
        "Unexpected error while trying to parse an array index.");

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> indexReference = ReferenceParser::parseQualifier(
    STR("0~where(prodId=Expression.Exp)"),
    ReferenceUsageCriteria::MULTI_DATA);
  auto index = getSharedPtr(seeker.tryGet(indexReference.get(), astBlockRoot.get()));
  if (index == nullptr)
    throw EXCEPTION(SystemException,
        "Unexpected error while trying to parse an array index.");
  return this->gen->GenerateExpression(index);
}

}
