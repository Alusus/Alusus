/**
 * @file Core/Processing/Handlers/CustomParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::CustomParsingHandler
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_CUSTOMPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_CUSTOMPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

class CustomParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(CustomParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Types

  public: typedef std::function<void(Parser *parser, ParserState *state)> ProdEndHandler;
  public: typedef std::function<Bool(
    Parser *parser, ParserState *state, TioSharedPtr const &modifierData, Bool prodProcessingComplete
  )> IncomingModifierHandler;

  //============================================================================
  // Member Variables

  private: ProdEndHandler prodEndHandler;
  private: IncomingModifierHandler incomingModifierHandler;
  private: Bool callingParentEnabled;


  //============================================================================
  // Constructor

  public: CustomParsingHandler(ProdEndHandler const &h, Bool parentEnabled=true) :
    prodEndHandler(h), callingParentEnabled(parentEnabled)
  {
  }

  public: CustomParsingHandler(ProdEndHandler const &peh, Bool parentEnabled, IncomingModifierHandler const &imh) :
    prodEndHandler(peh), callingParentEnabled(parentEnabled), incomingModifierHandler(imh)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Parser *parser, ParserState *state)
  {
    if (this->callingParentEnabled) {
      GenericParsingHandler::onProdEnd(parser, state);
    }
    this->prodEndHandler(parser, state);
  }

  public: virtual Bool onIncomingModifier(
    Parser *parser, ParserState *state, TioSharedPtr const &modifierData, Bool prodProcessingComplete
  ) {
    if (this->incomingModifierHandler == nullptr) return false;
    return this->incomingModifierHandler(parser, state, modifierData, prodProcessingComplete);
  }

}; // class

} } } // namespace

#endif
