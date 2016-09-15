/**
 * @file Spp/Ast/Type.cpp
 * Contains the implementation of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Printable Implementation

void Type::print(OutStream &stream, Int indents) const
{
  stream << STR("Type ") << this->getIdString();
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << ID_GENERATOR->getDesc(id) << STR("]");
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-defs:");
  for (Word i = 1; i < this->getCount(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+2);
    stream << this->getKey(i).c_str() << STR(": ");
    Core::Data::dumpData(stream, this->get(i), indents+2);
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-statements: ");
  Core::Data::dumpData(stream, this->getStatements().get(), indents+2);
}

} } // namespace
