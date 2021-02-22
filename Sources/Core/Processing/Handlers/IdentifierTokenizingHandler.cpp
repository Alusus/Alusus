/**
 * @file Core/Processing/Handlers/IdentifierTokenizingHandler.cpp
 * Contains the implementation of class
 * Core::Processing::Handlers::IdentifierTokenizingHandler.
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

//==============================================================================
// Overloaded Abstract Functions

void IdentifierTokenizingHandler::prepareToken(
  Data::Token *token, Word id, WChar const *tokenText, Word tokenTextLength,
  Data::SourceLocationRecord const &sourceLocation
) {
  token->setText(tokenText, tokenTextLength);
  token->setId(id);
  token->setSourceLocation(sourceLocation);
  auto count = this->keywords.count(token->getText());
  if (count == 1) token->setAsKeyword(true);
  else token->setAsKeyword(false);
}

} // namespace
