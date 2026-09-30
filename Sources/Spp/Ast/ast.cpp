/**
 * @file Spp/Ast/ast.cpp
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

Char const* findOperationModifier(Core::Ast::Definition const *def)
{
  auto stringLiteral = def->getMetadata(S("operation")).ti_cast_get<Core::Ast::StringLiteral>();
  if (stringLiteral != 0) return stringLiteral->getValue().get();
  return 0;
}


Bool isInjection(Core::Ast::Definition *def)
{
  return def->getMetadata(S("injection")) != 0;
}


Function* getDummyBuiltInOpFunction()
{
  static Function func;
  return &func;
}

} // namespace
