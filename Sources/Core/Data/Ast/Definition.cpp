/**
 * @file Core/Data/Ast/Definition.cpp
 * Contains the implementation of class Core::Data::Ast::Definition.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Ast
{

//==============================================================================
// Printable Implementation

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
  if (this->modifiers != 0 && this->modifiers->getCount() > 0) {
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("modifiers:\n");
    for (Int i = 0; i < this->modifiers->getCount(); ++i) {
      auto modifier = this->modifiers->get(i).ti_cast_get<Printable>();
      if (modifier != 0) {
        printIndents(stream, indents+2);
        modifier->print(stream, indents+2);
      }
    }
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("target: ");
    dumpData(stream, this->target.get(), indents+2);
  } else {
    stream << S(": ");
    dumpData(stream, this->target.get(), indents);
  }
}

} // namespace
