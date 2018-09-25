/**
 * @file Core/Data/Ast/Definition.cpp
 * Contains the implementation of class Core::Data::Ast::Definition.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data { namespace Ast
{

//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Definition::clone() const
{
  SharedPtr<Definition> newDefinition = std::make_shared<Definition>();
  newDefinition->setProdId(this->getProdId());
  newDefinition->setSourceLocation(this->getSourceLocation());
  newDefinition->setTarget(this->target);
  newDefinition->setName(this->name.get());
  newDefinition->setToMerge(this->isToMerge());
  return newDefinition;
}


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
  stream << S(": ");
  dumpData(stream, this->target.get(), indents);
}

} } } // namespace
