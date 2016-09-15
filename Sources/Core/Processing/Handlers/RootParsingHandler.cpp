/**
 * @file Core/Processing/Handlers/RootParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::RootParsingHandler.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void RootParsingHandler::onProdStart(Parser *parser, ParserState *state)
{
  // Find the global definition module, if any.
  SharedPtr<Data::Ast::Scope> scope;
  if (parser->getDefinitionsRepository() != 0 &&
      parser->getDefinitionsRepository()->getLevelCount() > 0 &&
      parser->getDefinitionsRepository()->getLevelData(0) != 0) {
    scope = parser->getDefinitionsRepository()->getLevelData(0).tio_cast<Data::Ast::Scope>();
  }

  if (scope != 0) {
    // Use the global root module.
    state->setData(STR("root"), scope);
  } else {
    // Create a new module.
    state->setData(STR("root"), Data::Ast::Scope::create({}));
  }
}


void RootParsingHandler::onLevelExit(Parser *parser, ParserState *state, SharedPtr<TiObject> const &data)
{
  // Store unhandled data into the definitions module.
  if (state->isAtProdRoot()) {
    Data::Ast::Scope *scope = state->getData().s_cast_get<Data::Ast::Scope>();
    ASSERT(scope != 0);
    scope->set(STR("_ORPHAN_"), data);
  } else {
    state->setData(data);
  }
}

} } } // namespace
