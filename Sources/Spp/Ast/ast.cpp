/**
 * @file Spp/Ast/ast.cpp
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

Char const* findOperationModifier(Core::Data::Ast::Definition const *def)
{
  auto modifiers = def->getModifiers().get();
  if (modifiers != 0) {
    for (Int i = 0; i < modifiers->getElementCount(); ++i) {
      auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(modifiers->getElement(i));
      if (paramPass != 0) {
        auto identifier = paramPass->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
        if (identifier != 0 && identifier->getValue() == S("operation")) {
          auto stringLiteral = paramPass->getParam().ti_cast_get<Core::Data::Ast::StringLiteral>();
          if (stringLiteral != 0) return stringLiteral->getValue().get();
        }
      }
    }
  }
  return 0;
}


Function* getDummyBuiltInOpFunction()
{
  static Function func;
  return &func;
}

} // namespace
