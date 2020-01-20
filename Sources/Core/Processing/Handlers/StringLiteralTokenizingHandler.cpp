/**
 * @file Core/Processing/Handlers/StringLiteralTokenizingHandler.cpp
 * Contains the implementation of class
 * Core::Processing::Handlers::StringLiteralTokenizingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  static WChar backSlashChar = getWideCharFromUtf8(S("\\"));
  static WChar newLineChar = getWideCharFromUtf8(S("\n"));
  static WChar carriageReturnChar = getWideCharFromUtf8(S("\r"));
  static WChar formFeedChar = getWideCharFromUtf8(S("\f"));
  static WChar tabChar = getWideCharFromUtf8(S("\t"));
  static WChar nLetterChar = getWideCharFromUtf8(S("n"));
  static WChar rLetterChar = getWideCharFromUtf8(S("r"));
  static WChar tLetterChar = getWideCharFromUtf8(S("t"));
  static WChar fLetterChar = getWideCharFromUtf8(S("f"));
  static WChar jeemLetterChar = getWideCharFromUtf8(S("ج"));
  static WChar raaLetterChar = getWideCharFromUtf8(S("ر"));
  static WChar taaLetterChar = getWideCharFromUtf8(S("ت"));
  static WChar doubleQuoteChar = getWideCharFromUtf8(S("\""));
  static WChar singleQuoteChar = getWideCharFromUtf8(S("'"));
  static WChar xLetterChar = getWideCharFromUtf8(S("x"));
  static WChar hLetterChar = getWideCharFromUtf8(S("h"));
  static WChar uLetterChar = getWideCharFromUtf8(S("u"));
  static WChar bigULetterChar = getWideCharFromUtf8(S("U"));

  WChar outerQuoteChar, innerQuoteChar;
  if (outerQuoteType == OuterQuoteType::DOUBLE) {
    outerQuoteChar = doubleQuoteChar;
    innerQuoteChar = singleQuoteChar;
  } else {
    outerQuoteChar = singleQuoteChar;
    innerQuoteChar = doubleQuoteChar;
  }

  // Set the token text after parsing control sequences and removing quotes.
  Word i = 0;
  Bool inStr = false;
  WChar *buffer = reinterpret_cast<WChar*>(SALLOC(tokenTextLength*sizeof(WChar)));
  Word bufferLength = 0;
  while (i < tokenTextLength) {
    if (inStr) {
      if (tokenText[i] == outerQuoteChar) inStr = false;
      else if (tokenText[i] == backSlashChar) {
        ++i;
        if (tokenText[i] == outerQuoteChar) {
          buffer[bufferLength] = outerQuoteChar;
          ++bufferLength;
        } else if (tokenText[i] == innerQuoteChar) {
            buffer[bufferLength] = innerQuoteChar;
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
        } else if (tokenText[i] == fLetterChar) {
            buffer[bufferLength] = formFeedChar;
            ++bufferLength;
        } else if (tokenText[i] == xLetterChar || tokenText[i] == hLetterChar) {
          ++i;
          WChar val = (WChar)parseHexDigits(tokenText + i, 2);
          ++i;
          buffer[bufferLength] = val;
          ++bufferLength;
        } else if (tokenText[i] == uLetterChar) {
          ++i;
          WChar val = (WChar)parseHexDigits(tokenText + i, 4);
          i += 3;
          buffer[bufferLength] = val;
          ++bufferLength;
        } else if (tokenText[i] == bigULetterChar) {
          ++i;
          WChar val = (WChar)parseHexDigits(tokenText + i, 8);
          i += 7;
          buffer[bufferLength] = val;
          ++bufferLength;
        }
        // TODO: Parse other escape sequences.
      } else {
        buffer[bufferLength] = tokenText[i];
        ++bufferLength;
      }
    } else {
      if (tokenText[i] == outerQuoteChar) inStr = true;
    }
    ++i;
  }
  buffer[bufferLength] = WC('\0');
  token->setText(buffer, bufferLength);
  SFREE(buffer);
  // Set other token info.
  token->setId(id);
  token->setAsKeyword(false);
  token->setSourceLocation(sourceLocation);
}

} // namespace
