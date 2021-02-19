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


  //============================================================================
  // Member Variables

  private: ProdEndHandler prodEndHandler;
  private: Bool callingParentEnabled;


  //============================================================================
  // Constructor

  public: CustomParsingHandler(ProdEndHandler const &h, Bool parentEnabled=true) :
    prodEndHandler(h), callingParentEnabled(parentEnabled)
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

}; // class

} } } // namespace

#endif
