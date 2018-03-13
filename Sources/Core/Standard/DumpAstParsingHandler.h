/**
 * @file Core/Standard/DumpAstParsingHandler.h
 * Contains the header of Core::Standard::DumpAstParsingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_DUMPASTPARSINGHANDLER_H
#define CORE_STANDARD_DUMPASTPARSINGHANDLER_H

namespace Core { namespace Standard
{

class DumpAstParsingHandler : public Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DumpAstParsingHandler, Processing::Handlers::GenericParsingHandler, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  RootManager *rootManager;


  //============================================================================
  // Constructor

  public: DumpAstParsingHandler(RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Processing::Parser *parser, Processing::ParserState *state);

}; // class

} } // namespace

#endif
