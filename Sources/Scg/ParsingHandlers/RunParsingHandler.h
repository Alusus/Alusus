/**
 * @file Scg/ParsingHandlers/RunParsingHandler.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __RunParsingHandler_h__
#define __RunParsingHandler_h__

#include <core.h>

namespace Scg
{

class RunParsingHandler : public Core::Processing::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(RunParsingHandler, Core::Processing::GenericParsingHandler,
            "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: RunParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

}; // class

} // namespace

#endif
