/**
 * @file Spp/Handlers/DumpParsingHandler.h
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_DUMPPARSINGHANDLER_H
#define SPP_HANDLERS_DUMPPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class DumpParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DumpParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: DumpParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

}; // class

} } // namespace

#endif
