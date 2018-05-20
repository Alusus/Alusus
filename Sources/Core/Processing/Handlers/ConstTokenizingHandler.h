/**
 * @file Core/Processing/ConstTokenizingHandler.h
 * Contains the header of class Core::Processing::ConstTokenizingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_CONSTTOKENIZINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_CONSTTOKENIZINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

/**
 * @brief Used to override the ID of const tokens.
 * @ingroup core_processing_handlers
 *
 * This tokenizing handler is used to override the id of automatically generated
 * constant tokens to a specific one. This is useful if we need all const tokens
 * to have the same id so it can be easily matched in token terms.
 */
class ConstTokenizingHandler : public TokenizingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ConstTokenizingHandler, TokenizingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


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

  public: virtual void prepareToken(Data::Token *token, Word id, WChar const *tokenText, Word tokenTextLength,
                                    Data::SourceLocationRecord const &sourceLocation)
  {
    token->setText(tokenText, tokenTextLength);
    token->setId(this->id);
    token->setSourceLocation(sourceLocation);
  }

}; // class

} // namespace

#endif
