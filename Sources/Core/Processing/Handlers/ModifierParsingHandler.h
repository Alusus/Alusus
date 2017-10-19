/**
 * @file Core/Processing/Handlers/ModifierParsingHandler.h
 * Contains the header of Core::Processing::Handlers::ModifierParsingHandler.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_MODIFIERPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_MODIFIERPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

class ModifierParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ModifierParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Bool leading;


  //============================================================================
  // Constructor

  public: ModifierParsingHandler(Bool leading) : leading(leading)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

}; // class

} } } // namespace

#endif
