/**
 * @file Core/Standard/StringLiteralTokenizingHandler.h
 * Contains the header of class Core::Standard::StringLiteralTokenizingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLES_STRINGLITERALTOKENIZINGHANDLER_H
#define CORE_PROCESSING_HANDLES_STRINGLITERALTOKENIZINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

class StringLiteralTokenizingHandler : public TokenizingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(StringLiteralTokenizingHandler, TokenizingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: StringLiteralTokenizingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void prepareToken(
    Data::Token *token, Word id, WChar const *tokenText, Word tokenTextLength,
    Data::SourceLocationRecord const &sourceLocation
  );

}; // class

} // namespace

#endif
