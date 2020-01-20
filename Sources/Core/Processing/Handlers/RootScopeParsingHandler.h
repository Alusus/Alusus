/**
 * @file Core/Processing/Handlers/RootScopeParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::RootScopeParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_ROOTSCOPEPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_ROOTSCOPEPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

class RootScopeParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootScopeParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Core::Main::RootScopeHandler *rootScopeHandler;


  //============================================================================
  // Constructor

  public: RootScopeParsingHandler(Core::Main::RootScopeHandler *rsh) : rootScopeHandler(rsh)
  {
  }

  public: static SharedPtr<RootScopeParsingHandler> create(Core::Main::RootScopeHandler *rsh)
  {
    return std::make_shared<RootScopeParsingHandler>(rsh);
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state, Data::Token const *token);

  public: virtual void onProdEnd(Parser *parser, ParserState *state)
  {
  }

  protected: virtual void addData(SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex);

}; // class

} // namespace

#endif
