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
#include <Containers/Module.h>
#include <CodeGeneration/CodeGenerator.h>

namespace Scg
{

class ModuleParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ModuleParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;
  Core::Basic::SharedPtr<Core::Data::Reference> aliasDictionaryRef =
    REF_PARSER->parseQualifier(STR("root:_Aliases"),
        Core::Data::ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH);


  //============================================================================
  // Constructor

public:
  ModuleParsingHandler(Core::Standard::RootManager *rm) : rootManager(rm)
  {
  }


  //============================================================================
  // Member Functions

public:
  virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

private:
  void addDefinitionToModule(Core::Processing::ParserState *state, const SharedPtr<TiObject> &def,
                             Module *module, CodeGenerator *generator);

private:
  void addLinkToModule(Core::Processing::ParserState *state, const SharedPtr<TiObject> &link,
                       Module *module, CodeGenerator *generator);

private:
  Char const* getLinkName(TiObject *link);

}; // class

} // namespace

#endif
