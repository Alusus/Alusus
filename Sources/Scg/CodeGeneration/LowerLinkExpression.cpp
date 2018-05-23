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

// Core header files
#include <Basic/SharedPtr.h>

// SCG header files
#include <CodeGeneration/CodeGenerator.h>
#include <CodeGeneration/LowerLinkExpression.h>

namespace Scg
{
using namespace Core::Basic;
using namespace Core::Data;

LowerLinkExpression::LowerLinkExpression(CodeGenerator *gen,
    const SharedPtr<TiObject> &item)
{
  auto metadata = item.ti_cast_get<Ast::MetadataHolder>();

  if (metadata == nullptr || metadata->getProdId() != gen->getLowerLinkExpId())
    throw EXCEPTION(InvalidArgumentException,
                    "Lower link expressions can be constructed from "
                    "Expression.LowerLinkExp only.");

  auto list = item.s_cast<Ast::List>();
  this->lhs = list->getShared(0);
  this->separator = static_cast<Ast::Token*>(list->get(1))->getText();
  this->rhs = list->getShared(2);
}
}
