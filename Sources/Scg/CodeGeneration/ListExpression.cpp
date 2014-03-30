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
  using namespace Core::Standard;
  using namespace Core::Basic;

  ListExpression::ListExpression(CodeGenerator *gen,
      const SharedPtr<ParsedItem> &item)
  {
    // TODO: Why is the statement list considered a list expression? It is
    // currently used by Function class to parse the body of the function, but I don't
    // think this is correct. Instead, we should have a separate class to
    // parse a list of statements.
    if (item->getProdId() == gen->GetListExpId() ||
        item->getProdId() == gen->GetStatementListId())
    {
      auto listExp = item.s_cast<Core::Standard::ParsedList>();
      for (auto i = 0; i < listExp->getElementCount(); i++)
        this->items.push_back(listExp->getElement(i).s_cast<ParsedItem>());
    }
    else
      this->items.push_back(item);
  }

  //----------------------------------------------------------------------------

  StringArray ListExpression::ParseTokenList() const
  {
    static ParsedDataBrowser tokenBrowser(STR(
        "Subject.Subject1>0:Subject.Parameter"));
    StringArray tokens;
    for (auto i = 0; i < GetItemCount(); i++)
    {
      auto item = GetItem(i);
      auto token = tokenBrowser.getValue<ParsedToken>(item);
      if (token == 0)
        // TODO: Add the index of the non-token to the exception message.
        THROW_EXCEPTION(InvalidArgumentException,
            "This list expression has one or more non-token item(s).");
      tokens.push_back(token->getText());
    }
    return tokens;
  }
}
