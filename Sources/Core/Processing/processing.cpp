/**
 * @file Core/Basic/processing.cpp
 * Contains the global implementations of Processing namespace's declarations.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

void computeNextCharPosition(WChar ch, Int &line, Int &column)
{
  if (ch == WC('\r')) {
    column = 1;
  } else if (ch == WC('\n')) {
    column = 1;
    line++;
  } else {
    column++;
  }
}


void assignTokenSourceLocationRecord(Ast::Token *token, Ast::SourceLocationRecord const &sourceLocation)
{
  auto existingSourceLocation = token->getSourceLocation().ti_cast_get<Ast::SourceLocationRecord>();
  if (existingSourceLocation == 0) {
    token->setSourceLocation(newSrdObj<Ast::SourceLocationRecord>(sourceLocation));
  } else {
    existingSourceLocation->filename = sourceLocation.filename;
    existingSourceLocation->line = sourceLocation.line;
    existingSourceLocation->column = sourceLocation.column;
  }
}

} } // namespace
