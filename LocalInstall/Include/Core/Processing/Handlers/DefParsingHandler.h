/**
 * @file Core/Processing/Handlers/DefParsingHandler.h
 * Contains the header of Core::Processing::Handlers::DefParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_DEFPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_DEFPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

class DefParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(DefParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: DefParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Parser *parser, ParserState *state);

  public: virtual Bool onIncomingModifier(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    TioSharedPtr const &modifierData, Bool prodProcessingComplete
  );

}; // class

} } } // namespace

#endif
