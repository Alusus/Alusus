/**
 * @file Core/Processing/Handlers/RootParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::RootParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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
  SharedPtr<Data::Module> module;
  if (parser->getDefinitionsRepository() != 0 &&
      parser->getDefinitionsRepository()->getLevelCount() > 0 &&
      parser->getDefinitionsRepository()->getLevelData(0) != 0) {
    module = parser->getDefinitionsRepository()->getLevelData(0).tio_cast<Data::Module>();
  }

  if (module != 0) {
    // Use the global root module.
    state->setData(STR("root"), module);
  } else {
    // Create a new module.
    state->setData(STR("root"), Data::Module::create({}));
  }
}


void RootParsingHandler::onLevelExit(Parser *parser, ParserState *state, SharedPtr<TiObject> const &data)
{
  // Store unhandled data into the definitions module.
  if (state->isAtProdRoot()) {
    Data::Module *module = state->getData().s_cast_get<Data::Module>();
    ASSERT(module != 0);
    module->set(STR("_ORPHAN_"), data);
  } else {
    state->setData(data);
  }
}

} } } // namespace
