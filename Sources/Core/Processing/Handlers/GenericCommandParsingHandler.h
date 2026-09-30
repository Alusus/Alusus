/**
 * @file Core/Processing/Handlers/GenericCommandParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::GenericCommandParsingHandler
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_GENERICCOMMANDPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_GENERICCOMMANDPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

class GenericCommandParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(GenericCommandParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiStr type;


  //============================================================================
  // Constructor

  public: GenericCommandParsingHandler(Char const *t) : type(t)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state, Ast::Token const *token);

  protected: virtual void addData(SharedPtr<Ast::Node> const &data, Parser *parser, ParserState *state, Int levelIndex);

}; // class

} } } // namespace

#endif
