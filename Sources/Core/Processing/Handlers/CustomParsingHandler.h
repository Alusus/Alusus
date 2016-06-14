/**
 * @file Core/Processing/Handlers/CustomParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::CustomParsingHandler
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TYPE_INFO(CustomParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Types

  public: typedef std::function<void(Parser *parser, ParserState *state)> ProdEndHandler;


  //============================================================================
  // Member Variables

  private: ProdEndHandler prodEndHandler;


  //============================================================================
  // Constructor

  public: CustomParsingHandler(ProdEndHandler const &h) : prodEndHandler(h)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Parser *parser, ParserState *state)
  {
    GenericParsingHandler::onProdEnd(parser, state);
    this->prodEndHandler(parser, state);
  }

}; // class

} } } // namespace

#endif
