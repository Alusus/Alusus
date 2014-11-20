/**
 * @file Core/Processing/RootParsingHandler.h
 * Contains the header of class Core::Processing::RootParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_ROOTPARSINGHANDLER_H
#define PROCESSING_ROOTPARSINGHANDLER_H

namespace Core { namespace Processing
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

} } // namespace

#endif
