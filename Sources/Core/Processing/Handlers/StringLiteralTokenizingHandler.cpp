/**
 * @file Core/Processing/Handlers/StringLiteralTokenizingHandler.cpp
 * Contains the implementation of class
 * Core::Processing::Handlers::StringLiteralTokenizingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void StringLiteralTokenizingHandler::prepareToken(
  Data::Token *token, Word id, WChar const *tokenText, Word tokenTextLength,
  Data::SourceLocationRecord const &sourceLocation
) {
  // Precomputed wide characters values.
  static WChar backSlashChar = getWideCharFromUtf8(STR("\\"));
  static WChar newLineChar = getWideCharFromUtf8(STR("\n"));
  static WChar carriageReturnChar = getWideCharFromUtf8(STR("\r"));
  static WChar tabChar = getWideCharFromUtf8(STR("\t"));
  static WChar nLetterChar = getWideCharFromUtf8(STR("n"));
  static WChar rLetterChar = getWideCharFromUtf8(STR("r"));
  static WChar tLetterChar = getWideCharFromUtf8(STR("t"));
  static WChar jeemLetterChar = getWideCharFromUtf8(STR("ج"));
  static WChar raaLetterChar = getWideCharFromUtf8(STR("ر"));
  static WChar taaLetterChar = getWideCharFromUtf8(STR("ت"));
  static WChar doubleQuoteChar = getWideCharFromUtf8(STR("\""));
  static WChar singleQuoteChar = getWideCharFromUtf8(STR("'"));
  // Set the token text after parsing control sequences and removing quotes.
  Word i = 0;
  Bool inStr = false;
  WChar *buffer = reinterpret_cast<WChar*>(SALLOC(tokenTextLength*sizeof(WChar)));
  Word bufferLength = 0;
  while (i < tokenTextLength) {
    if (inStr) {
      if (tokenText[i] == doubleQuoteChar) inStr = false;
      else if (tokenText[i] == backSlashChar) {
        ++i;
        if (tokenText[i] == doubleQuoteChar) {
          buffer[bufferLength] = doubleQuoteChar;
          ++bufferLength;
        } else if (tokenText[i] == singleQuoteChar) {
            buffer[bufferLength] = singleQuoteChar;
            ++bufferLength;
        } else if (tokenText[i] == backSlashChar) {
            buffer[bufferLength] = backSlashChar;
            ++bufferLength;
        } else if (tokenText[i] == nLetterChar || tokenText[i] == jeemLetterChar) {
          buffer[bufferLength] = newLineChar;
          ++bufferLength;
        } else if (tokenText[i] == rLetterChar || tokenText[i] == raaLetterChar) {
          buffer[bufferLength] = carriageReturnChar;
          ++bufferLength;
        } else if (tokenText[i] == tLetterChar || tokenText[i] == taaLetterChar) {
          buffer[bufferLength] = tabChar;
          ++bufferLength;
        }
        // TODO: Parse other escape sequences.
      } else {
        buffer[bufferLength] = tokenText[i];
        ++bufferLength;
      }
    } else {
      if (tokenText[i] == doubleQuoteChar) inStr = true;
    }
    ++i;
  }
  token->setText(buffer, bufferLength);
  SFREE(buffer);
  // Set other token info.
  token->setId(id);
  token->setSourceLocation(sourceLocation);
}

} // namespace
