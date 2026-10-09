/**
 * @file Core/Processing/Handlers/StatementParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::StatementParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

Bool StatementParsingHandler::onIncomingModifier(
  Parser *parser, ParserState *state, SharedPtr<Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  // This is the outermost statement-level handler, so any modifier that reached us wasn't claimed by any
  // inner production. Rather than reporting it as unexpected, accept it as an orphan modifier of the
  // statement's resulting node.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto node = state->getData(levelOffset).get();
  if (node == 0) return false;

  node->addModifier(modifierData);
  return true;
}

} // namespace
