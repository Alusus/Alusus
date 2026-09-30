/**
 * @file Core/Processing/Handlers/StatementParsingHandler.h
 * Contains the header of Core::Processing::Handlers::StatementParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_STATEMENTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_STATEMENTPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

/**
 * @brief The parsing handler used for the top level statement production.
 * @ingroup core_processing_handlers
 *
 * Inner productions (Def, Function, Type, ...) each get a chance to claim an incoming modifier through
 * their own Grammar::SymbolDefinition::modifierActions and onIncomingModifier overrides. Any modifier
 * that none of them claims bubbles all the way up to the Statement production, which is where this
 * handler is used. Rather than reporting such orphan modifiers as unexpected, this handler simply
 * accepts them and stores them as modifiers on the statement's resulting Node, to be dealt with later
 * (e.g. by AstProcessor).
 */
class StatementParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(StatementParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Constructor

  public: StatementParsingHandler()
  {
  }

  public: static SharedPtr<StatementParsingHandler> create()
  {
    return newSrdObj<StatementParsingHandler>();
  }


  //============================================================================
  // Member Functions

  public: virtual Bool onIncomingModifier(
    Parser *parser, ParserState *state, SharedPtr<Ast::Node> const &modifierData, Bool prodProcessingComplete
  );

}; // class

} // namespace

#endif
