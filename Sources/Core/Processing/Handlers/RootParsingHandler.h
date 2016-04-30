/**
 * @file Core/Processing/Handlers/RootParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::RootParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_ROOTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_ROOTPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

class RootParsingHandler : public ParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootParsingHandler, ParsingHandler, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: RootParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state);

  public: virtual void onLevelExit(Parser *parser, ParserState *state, SharedPtr<IdentifiableObject> const &data);

}; // class

} } } // namespace

#endif
