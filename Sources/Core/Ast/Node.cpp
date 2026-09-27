/**
 * @file Core/Ast/Node.cpp
 * Contains the implementation of class Core::Ast::Node.
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

void Node::print(OutStream &stream, Int indents) const
{
  stream << this->getMyTypeInfo()->getUniqueName();
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
  }
  auto mapContainer = this->getInterface<MapContaining<Node> const>();
  if (mapContainer != 0) {
    for (Word i = 0; i < mapContainer->getElementCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+1);
      stream << mapContainer->getElementKey(i) << S(": ");
      dumpAst(stream, mapContainer->getElement(i), indents+1);
    }
    return;
  }
  auto container = this->getInterface<Containing<Node> const>();
  if (container != 0) {
    for (Word i = 0; i < container->getElementCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+1);
      dumpAst(stream, container->getElement(i), indents+1);
    }
  }
}

} // namespace
