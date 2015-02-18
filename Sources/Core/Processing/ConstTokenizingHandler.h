/**
 * @file Core/Processing/ConstTokenizingHandler.h
 * Contains the header of class Core::Processing::ConstTokenizingHandler.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_CONSTTOKENIZINGHANDLER_H
#define PROCESSING_CONSTTOKENIZINGHANDLER_H

namespace Core { namespace Processing
{

// TODO: DOC

/**
 * @brief Used to override the ID of const tokens.
 * @ingroup processing_lexer
 *
 * This tokenizing handler is used to override the id of automatically generated
 * constant tokens to a specific one. This is useful if we need all const tokens
 * to have the same id so it can be easily matched in token terms.
 */
class ConstTokenizingHandler : public TokenizingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ConstTokenizingHandler, TokenizingHandler, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Word id;


  //============================================================================
  // Constructor

  public: ConstTokenizingHandler(Word id) : id(id)
  {
  }


  //============================================================================
  // Member Functions

  public: Word getId() const
  {
    return this->id;
  }

  public: virtual void prepareToken(Data::Token *token, Word id, Int line, Int column,
                                    WChar const *tokenText, Word tokenTextLength)
  {
    token->setText(tokenText, tokenTextLength);
    token->setId(this->id);
    token->setLine(line);
    token->setColumn(column);
  }

}; // class

} } // namespace

#endif
