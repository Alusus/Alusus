/**
 * @file Scg/ParsingHandlers/BuildParsingHandler.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __BuildParsingHandler_h__
#define __BuildParsingHandler_h__

#include <core.h>

namespace Scg
{

class BuildParsingHandler : public Core::Standard::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildParsingHandler, Core::Standard::GenericParsingHandler,
            "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: BuildParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Parser::StateMachine *machine, Core::Parser::State *state);

}; // class

} // namespace

#endif
