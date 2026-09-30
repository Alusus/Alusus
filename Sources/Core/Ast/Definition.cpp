/**
 * @file Core/Ast/Definition.cpp
 * Contains the implementation of class Core::Ast::Definition.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Ast
{

//==============================================================================
// Printing Functions

void Definition::print(OutStream &stream, Int indents) const
{
  stream << S("Definition");
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
  }
  stream << S(" ") << this->name;
  if (this->isToMerge()) {
    stream << S(" <to merge>");
  }
  Node::printModifiers(stream, indents);
  Node::printMetadata(stream, indents);
  if ((this->modifiers != 0 && this->modifiers->getCount() > 0) ||
      (this->metadata != 0 && this->metadata->getCount() > 0)) {
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("target: ");
    Ast::dumpAst(stream, this->target.get(), indents+2);
  } else {
    stream << S(": ");
    Ast::dumpAst(stream, this->target.get(), indents);
  }
}

} // namespace
