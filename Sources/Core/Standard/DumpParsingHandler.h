/**
 * @file Core/Standard/DumpParsingHandler.h
 * Contains the header of Core::Standard::DumpParsingHandler.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_DUMPPARSINGHANDLER_H
#define CORE_STANDARD_DUMPPARSINGHANDLER_H

namespace Core { namespace Standard
{

class DumpParsingHandler : public Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DumpParsingHandler, Processing::Handlers::GenericParsingHandler, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  RootManager *rootManager;


  //============================================================================
  // Constructor

  public: DumpParsingHandler(RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Processing::Parser *parser, Processing::ParserState *state);

}; // class

} } // namespace

#endif
