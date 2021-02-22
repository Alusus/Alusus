/**
 * @file Core/Processing/Handlers/SubjectParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::SubjectParsingHandler
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_SUBJECTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_SUBJECTPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

class SubjectParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(SubjectParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Word constTokenId;


  //============================================================================
  // Constructor

  public: SubjectParsingHandler(Word cti): constTokenId(cti)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onAlternateRouteDecision(
    Parser *parser, ParserState *state, Int route, Data::Token const *token)
  {
    if (state->isAProdRoot(-1) && token->getId() == this->constTokenId) {
      if (token->getText() == S("(")) {
        auto bracket = Data::Ast::Bracket::create({
          { "sourceLocation", newSrdObj<Data::SourceLocationRecord>(token->getSourceLocation()) }
        });
        bracket->setType(Data::Ast::BracketType::ROUND);
        state->setData(bracket);
        return;
      } else if (token->getText() == S("[")) {
        auto bracket = Data::Ast::Bracket::create({
          { "sourceLocation", newSrdObj<Data::SourceLocationRecord>(token->getSourceLocation()) }
        });
        bracket->setType(Data::Ast::BracketType::SQUARE);
        state->setData(bracket);
        return;
      }
    }
    GenericParsingHandler::onAlternateRouteDecision(parser, state, route, token);
  }

}; // class

} // namespace

#endif
