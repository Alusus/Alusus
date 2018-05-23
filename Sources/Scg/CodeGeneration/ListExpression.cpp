/**
 * @file Scg/CodeGeneration/LinkExpression.cpp
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
#include <exceptions.h>

namespace Scg
{
using namespace Core::Basic;
using namespace Core::Data;

ListExpression::ListExpression(CodeGenerator *gen,
                               const SharedPtr<TiObject> &item)
{
  // TODO: Why is the statement list considered a list expression? It is
  // currently used by Function class to parse the body of the function, but I don't
  // think this is correct. Instead, we should have a separate class to
  // parse a list of statements.
  auto metadata = item.ti_cast_get<Ast::MetadataHolder>();

  if (metadata != nullptr && (metadata->getProdId() == gen->getListExpId() ||
                              metadata->getProdId() == gen->getStatementListId())) {
    auto listExp = item.s_cast<Core::Data::Ast::List>();

    for (auto i = 0; i < listExp->getCount(); i++)
      this->items.push_back(listExp->getShared(i));
  } else
    this->items.push_back(item);
}

//----------------------------------------------------------------------------

StringArray ListExpression::parseTokenList() const
{
  static ReferenceSeeker seeker;
  static SharedPtr<Reference> tokenReference = REF_PARSER->parseQualifier(
        STR("self~where(prodId=Subject.Subject1).0~where(prodId=Subject.Parameter)"),
        ReferenceUsageCriteria::MULTI_DATA);
  StringArray tokens;

  for (auto i = 0; i < getItemCount(); i++) {
    auto item = getItem(i);
    auto token = ti_cast<Ast::Token>(seeker.tryGet(tokenReference.get(), item.get()));

    if (token == 0)
      // TODO: Add the index of the non-token to the exception message.
      throw EXCEPTION(InvalidArgumentException,
                      "This list expression has one or more non-token item(s).");

    tokens.push_back(token->getText());
  }

  return tokens;
}
}
