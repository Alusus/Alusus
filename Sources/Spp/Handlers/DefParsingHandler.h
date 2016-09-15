/**
 * @file Spp/Handlers/DefParsingHandler.h
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_DEFPARSINGHANDLER_H
#define SPP_HANDLERS_DEFPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class DefParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DefParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Constructor

  public: DefParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: void mergeDefinition(Char const *qualifier, TiObject *obj, Core::Processing::ParserState *state);

}; // class

} } // namespace

#endif
