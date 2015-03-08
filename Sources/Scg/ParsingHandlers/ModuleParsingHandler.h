/**
 * @file Scg/ParsingHandlers/ModuleParsingHandler.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ModuleParsingHandler_h__
#define __ModuleParsingHandler_h__

#include <core.h>

namespace Scg
{

class ModuleParsingHandler : public Core::Processing::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ModuleParsingHandler, Core::Processing::GenericParsingHandler,
            "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: ModuleParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: void addDefinitionToModule(Core::Processing::ParserState *state,const SharedPtr<IdentifiableObject> &def,
                                      Core::Data::Module *module);

  private: void addLinkToModule(Core::Processing::ParserState *state, const SharedPtr<IdentifiableObject> &link,
                                Core::Data::Module *module);

  private: Char const* getLinkName(IdentifiableObject *link);

}; // class

} // namespace

#endif
