/**
 * @file Core/Processing/Handlers/ModifierParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::ModifierParsingHandler.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

void ModifierParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  if (this->leading) state->pushLeadingModifierLevel(state->getData());
  else state->pushTrailingModifierLevel(state->getData());
  state->setData(SharedPtr<TiObject>(0));
}

} } } // namespace
