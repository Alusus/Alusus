/**
 * @file Core/Processing/Handlers/DumpAstParsingHandler.h
 * Contains the header of Core::Processing::Handleres::DumpAstParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_DUMPASTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_DUMPASTPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

class DumpAstParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DumpAstParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  Main::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: DumpAstParsingHandler(Main::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Parser *parser, ParserState *state);

}; // class

} // namespace

#endif
