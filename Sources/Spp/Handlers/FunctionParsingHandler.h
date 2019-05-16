/**
 * @file Spp/Handlers/FunctionParsingHandler.h
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_FUNCTIONPARSINGHANDLER_H
#define SPP_HANDLERS_FUNCTIONPARSINGHANDLER_H

namespace Spp::Handlers
{

class FunctionParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: FunctionParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: Bool parseArgs(
    Core::Processing::ParserState *state, Core::Data::Ast::Bracket *bracket, SharedPtr<Core::Data::Ast::Map> &result
  );

  private: Bool parseArg(
    Core::Processing::ParserState *state, TioSharedPtr astNode, SharedPtr<Core::Data::Ast::Map> const &result
  );

  private: Bool parseNumber(
    Core::Processing::ParserState *state, TiObject *ast, TiWord &result, Core::Data::Ast::MetaHaving *parentMetadata
  );

  public: virtual Bool onIncomingModifier(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    TioSharedPtr const &modifierData, Bool prodProcessingComplete
  );

  private: Bool processExpnameModifier(
    Core::Processing::ParserState *state, TioSharedPtr const &modifierData
  );

  private: Bool processUnknownModifier(
    Core::Processing::ParserState *state, TioSharedPtr const &modifierData
  );

}; // class

} // namespace

#endif
