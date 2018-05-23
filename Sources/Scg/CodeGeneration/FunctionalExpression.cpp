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
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/FunctionalExpression.h>
#include <CodeGeneration/ParamPassExp.h>
#include <Containers/List.h>
#include <Instructions/CallFunction.h>
#include <Instructions/Cast.h>
#include <Instructions/DeclareExtFunction.h>
#include <Operators/Content.h>
#include <Operators/Size.h>
#include <Operators/ArrayElementReference.h>
#include <Operators/MemberFieldReference.h>
#include <Operators/IdentifierReference.h>

namespace Scg
{

using namespace Core::Basic;
using namespace Core::Data;

FunctionalExpression::FunctionalExpression(CodeGenerator *gen, const SharedPtr<Ast::List> &item) : gen(gen)
{
  if (item->getProdId() != gen->getFunctionalExpId())
    throw EXCEPTION(InvalidArgumentException,
                    "Functional expressions can be constructed from Expression.FunctionalExp only.");

  for (auto i = 0; i < item->getCount(); i++)
    this->subExprs.push_back(item->getShared(i));
}


SharedPtr<DeclareExtFunction> FunctionalExpression::toDeclareExtFunction(SharedPtr<ValueTypeSpec> const &retType)
{
  if (this->subExprs.size() != 2)
    throw EXCEPTION(SystemException, "Invalid function link.");

  auto name = this->gen->parseToken(this->subExprs[0]);
  auto args = ParamPassExp(
                this->gen, this->subExprs[1].s_cast<Ast::Route>()).parseValueTypes();

  // Determine whether the function being linked to has a variable number of
  // arguments or not, e.g. printf.
  // TODO: Remove dynamic_cast.
  auto varArgs = false;

  if (args.size() > 0) {
    auto typeByName = dynamic_cast<ValueTypeSpecByName*>(args[args.size() - 1].get());

    if (typeByName != nullptr && typeByName->getName().compare("args") == 0) {
      varArgs = true;
      args.pop_back();
    }
  }

  auto declFunc = std::make_shared<DeclareExtFunction>(name, retType, args, varArgs);
  auto metadata = this->subExprs[0]->getInterface<Ast::MetadataHolder>();

  if (metadata != 0) {
    declFunc->setSourceLocation(metadata->getSourceLocation());
  }

  return declFunc;
}


SharedPtr<AstNode> FunctionalExpression::toExpression()
{
  auto metadata = this->subExprs[0]->getInterface<Ast::MetadataHolder>();

  if (metadata == nullptr) {
    throw EXCEPTION(SyntaxErrorException, "Invalid expression. Expressions "
                    "should start with a token representing a variable or a function.");
  }

  if (metadata->getProdId() != this->gen->getSubjectId()) {
    this->gen->getBuildMsgStore()->add(std::make_shared<Processing::CustomBuildMsg>(
                                         STR("Invalid expression. Expressions should start with "
                                             "a token representing a variable or a function."),
                                         STR("SCG1023"), 1, metadata->getSourceLocation()));
  }

  SharedPtr<AstNode> expr;
  auto cancelNextContentOp = false;

  for (auto i = 0; i < this->subExprs.size(); i++) {
    auto thisExprAst = this->subExprs[i];
    auto thisExprAstMeta = thisExprAst->getInterface<Ast::MetadataHolder>();

    if (i < (this->subExprs.size() - 1)) {
      auto nextExprAst = this->subExprs[i + 1];
      auto nextExprAstMeta = nextExprAst->getInterface<Ast::MetadataHolder>();

      if (thisExprAstMeta->getProdId() == this->gen->getSubjectId() &&
          nextExprAstMeta->getProdId() == this->gen->getParamPassId() &&
          nextExprAst.s_cast<Ast::Route>()->getRoute() == ParamPassExp::Parenthesis) {
        // A token followed by parentheses. This is a function call.
        ParamPassExp params(this->gen, nextExprAst.s_cast<Ast::Route>());
        expr = std::make_shared<CallFunction>(this->gen->parseToken(thisExprAst),
                                              std::make_shared<List>(params.parseExpressionList()));
        expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
        i++;
        cancelNextContentOp = true;
        continue;
      }
    }

    if (thisExprAstMeta->getProdId() == this->gen->getSubjectId()) {
      try {
        auto varName = this->gen->parseToken(thisExprAst);
        expr = std::make_shared<IdentifierReference>(varName);
      } catch (InvalidArgumentException) {
        expr = this->gen->generateExpression(thisExprAst);
      }

      expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
    } else if (thisExprAstMeta->getProdId() == this->gen->getParamPassId() &&
               thisExprAst.s_cast<Ast::Route>()->getRoute() == ParamPassExp::Square) {
      // Square brackets. This is array element access.
      auto index = parseElementIndex(thisExprAst.s_cast<Ast::Route>());
      expr = std::make_shared<ArrayElementReference>(expr, index);
      expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
    } else if (thisExprAstMeta->getProdId() == this->gen->getLinkExpId()) {
      // Dot followed by a token. This is a field access.
      auto fieldName = parseFieldName(thisExprAst.s_cast<Ast::List>());
      expr = std::make_shared<MemberFieldReference>(expr, fieldName);
      expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
    } else if (thisExprAstMeta->getProdId() == this->gen->getPostfixTildeExpId()) {
      auto child = ti_cast<Ast::MetadataHolder>(thisExprAst.s_cast<Ast::List>()->get(0));

      if (child->getProdId() == this->gen->getContentTildeId()) {
        // ~cnt
        if (cancelNextContentOp) {
          cancelNextContentOp = false;
        } else {
          expr = std::make_shared<Content>(expr);
          expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
        }
      } else if (child->getProdId() == this->gen->getPointerTildeId()) {
        // ~ptr
        cancelNextContentOp = true;
      } else if (child->getProdId() == this->gen->getSizeTildeId()) {
        // ~size
        expr = std::make_shared<Size>(expr);
        expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
        cancelNextContentOp = true;
      } else if (child->getProdId() == this->gen->getCastTildeId()) {
        // ~cast
        if (i >= (this->subExprs.size() - 1)) {
          this->gen->getBuildMsgStore()->add(std::make_shared<Processing::CustomBuildMsg>(
                                               STR("Invalid postfix tilde expression."),
                                               STR("SCG1024"), 1, metadata->getSourceLocation()));
        } else {
          auto nextExprAst = this->subExprs[i + 1];
          auto nextExprAstMeta = nextExprAst->getInterface<Ast::MetadataHolder>();

          if (nextExprAstMeta->getProdId() == this->gen->getParamPassId() &&
              nextExprAst.s_cast<Ast::Route>()->getRoute() == ParamPassExp::Square) {
            ParamPassExp params(this->gen, nextExprAst.s_cast<Ast::Route>());
            auto type = this->gen->parseVariableType(params.getParam(0));

            // A token followed by parentheses. This is a function call.
            if (!cancelNextContentOp) {
              expr = std::make_shared<Content>(expr);
              expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
            }

            expr = std::make_shared<Cast>(expr, type);
            expr->setSourceLocation(thisExprAstMeta->getSourceLocation());
            i++;
            // FIXME: Workaround to avoid taking the content of a cast instruction
            // which is not possible.
            cancelNextContentOp = true;
            continue;
          }
        }
      } else {
        this->gen->getBuildMsgStore()->add(std::make_shared<Processing::CustomBuildMsg>(
                                             STR("Invalid postfix tilde expression."),
                                             STR("SCG1024"), 1, metadata->getSourceLocation()));
      }
    }
  }

  if (cancelNextContentOp)
    return expr;
  else {
    auto loc = expr->getSourceLocation();
    expr = std::make_shared<Content>(expr);
    expr->setSourceLocation(loc);
    return expr;
  }
}


Char const* FunctionalExpression::parseFieldName(
  const SharedPtr<Ast::List> &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->getLinkExpId())
    throw EXCEPTION(SystemException,
                    "Unexpected error while trying to parse a field name.");

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> fieldNameReference = REF_PARSER->parseQualifier(
        STR("1~where(prodId=Subject.Subject1)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto fieldName = getSharedPtr(seeker.tryGet(fieldNameReference.get(), astBlockRoot.get()));

  if (fieldName == nullptr)
    throw EXCEPTION(SystemException,
                    "Unexpected error while trying to parse a field name.");

  return this->gen->parseToken(fieldName);
}


SharedPtr<AstNode> FunctionalExpression::parseElementIndex(SharedPtr<Ast::Route> const &astBlockRoot)
{
  if (astBlockRoot->getProdId() != this->gen->getParamPassId())
    throw EXCEPTION(SystemException,
                    "Unexpected error while trying to parse an array index.");

  static ReferenceSeeker seeker;
  static SharedPtr<Reference> indexReference = REF_PARSER->parseQualifier(
        STR("0~where(prodId=Expression.Exp)"),
        ReferenceUsageCriteria::MULTI_DATA);
  auto index = getSharedPtr(seeker.tryGet(indexReference.get(), astBlockRoot.get()));

  if (index == nullptr)
    throw EXCEPTION(SystemException,
                    "Unexpected error while trying to parse an array index.");

  return this->gen->generateExpression(index);
}

} // namespace
