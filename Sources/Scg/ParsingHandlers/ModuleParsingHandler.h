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

class ModuleParsingHandler : public Core::Standard::GenericParsingHandler
{
    ////////////////////////////////////////////////////////////////////////////
    // Type Info

    TYPE_INFO(ModuleParsingHandler, Core::Standard::GenericParsingHandler,
              "Scg", "Scg", "alusus.net");


    ////////////////////////////////////////////////////////////////////////////
    // Member Variables

    Core::Standard::RootManager *rootManager;


    ////////////////////////////////////////////////////////////////////////////
    // Constructor

    public: ModuleParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
    {
    }


    ////////////////////////////////////////////////////////////////////////////
    // Member Functions

    public: virtual void onProdEnd(Core::Parser::StateMachine *machine, Core::Parser::State *state);

    private: void add_definition_to_module(const SharedPtr<Core::Standard::ParsedItem> &def, Core::Data::Module *module);

    private: void add_link_to_module(const SharedPtr<Core::Standard::ParsedItem> &link, Core::Data::Module *module);

    private: const Char* get_link_name(const SharedPtr<Core::Standard::ParsedItem> &link);

}; // class

} // namespace

#endif
