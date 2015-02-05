/**
 * @file Core/Processing/TokenizingHandler.h
 * Contains the header of class Core::Processing::TokenizingHandler.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_TOKENIZINGHANDLER_H
#define PROCESSING_TOKENIZINGHANDLER_H

namespace Core { namespace Processing
{

class Parser;

/**
 * @brief The root class of all tokenizing handlers.
 * @ingroup processing
 *
 * Tokenizing handlers are associated with token definitions to receive lexer
 * events that can be used for custom operations on the generated tokens.
 * Assigning tokenizing handlers to token definitions is optional.
 */
class TokenizingHandler : public Data::OperationHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(TokenizingHandler, Data::OperationHandler, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Protected Constructor

  protected: TokenizingHandler()
  {
  }


  //============================================================================
  // Virtual Functions

  /**
   * @brief Called when a new token is found.
   *
   * This event is raised by the lexer when a token is found. The handler should
   * set the properties of the provided Token object which will then be sent to
   * the parser.
   *
   * @param token The token whose properties should be set by the handler.
   * @param id The id of the token definition associated with the found token.
   * @param line The line number at which the token is found.
   * @param column The column number at which the token is found.
   * @param tokenText The full text of the token.
   * @param tokenTextLength The length of the token text.
   */
  public: virtual void prepareToken(Data::Token *token, Word id, Int line, Int column,
                                    WChar const *tokenText, Word tokenTextLength)
  {
  }

}; // class

} } // namespace

#endif
