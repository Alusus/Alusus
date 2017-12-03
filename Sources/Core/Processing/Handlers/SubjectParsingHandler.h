/**
 * @file Core/Processing/Handlers/SubjectParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::SubjectParsingHandler
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_SUBJECTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_SUBJECTPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

class SubjectParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(SubjectParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: SubjectParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onAlternateRouteDecision(
    Processing::Parser *parser, Processing::ParserState *state, Int route, Data::Token const *token)
  {
    if (state->isAProdRoot(-1)) {
      if (token->getText() == STR("(")) {
        auto bracket = Data::Ast::Bracket::create({
          { "sourceLocation", std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation()) }
        });
        bracket->setType(Data::Ast::BracketType::ROUND);
        state->setData(bracket);
        return;
      } else if (token->getText() == STR("[")) {
        auto bracket = Data::Ast::Bracket::create({
          { "sourceLocation", std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation()) }
        });
        bracket->setType(Data::Ast::BracketType::SQUARE);
        state->setData(bracket);
        return;
      }
    }
    GenericParsingHandler::onAlternateRouteDecision(parser, state, route, token);
  }

}; // class

} } } // namespace

#endif
