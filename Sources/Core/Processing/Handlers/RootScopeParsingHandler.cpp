/**
 * @file Core/Processing/Handlers/RootScopeParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::RootScopeParsingHandler.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

void RootScopeParsingHandler::onProdStart(Parser *parser, ParserState *state, Data::Token const *token)
{
  state->setData(this->rootScopeHandler->getRootScope());
}


void RootScopeParsingHandler::addData(
  SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex
) {
  if (state->isAProdRoot(levelIndex)) {
    this->rootScopeHandler->addNewElement(data, parser, state);
  } else {
    GenericParsingHandler::addData(data, parser, state, levelIndex);
  }
}

} // namespace
