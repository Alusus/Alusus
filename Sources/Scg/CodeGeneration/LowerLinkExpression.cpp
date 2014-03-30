/**
 * @file Scg/CodeGeneration/LowerLinkExpression.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/LowerLinkExpression.h>

namespace Scg
{
  using namespace Core::Standard;
  using namespace Core::Basic;

  LowerLinkExpression::LowerLinkExpression(CodeGenerator *gen,
      const SharedPtr<ParsedItem> &item)
  {
    if (item->getProdId() != gen->GetLowerLinkExpId())
      THROW_EXCEPTION(InvalidArgumentException,
          "Lower link expressions can be constructed from "
          "Expression.LowerLinkExp only.");

    auto list = item.s_cast<ParsedList>();
    this->lhs = list->getElement(0).s_cast<ParsedItem>();
    this->separator = list->getElement(1).s_cast<ParsedToken>()->getText();
    this->rhs = list->getElement(2).s_cast<ParsedItem>();
  }
}
