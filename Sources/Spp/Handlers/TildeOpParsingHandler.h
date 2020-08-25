/**
 * @file Spp/Handlers/TildeOpParsingHandler.h
 * Contains the header of class Spp::Handlers::TildeOpParsingHandler
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_TILDEOPPARSINGHANDLER_H
#define SPP_HANDLERS_TILDEOPPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

template<class TYPE> class TildeOpParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(
    TildeOpParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
    "Spp.Handlers", "Spp", "alusus.org", (TYPE)
  );


  //============================================================================
  // Constructor

  public: TildeOpParsingHandler()
  {
  }

  public: static SharedPtr<TildeOpParsingHandler<TYPE>> create()
  {
    return newSrdObj<TildeOpParsingHandler<TYPE>>();
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state, Core::Data::Token const *token
  ) {
    auto data = newSrdObj<TYPE>();
    data->setSourceLocation(newSrdObj<Data::SourceLocationRecord>(token->getSourceLocation()));
    state->setData(data);
  }

  public: virtual void onNewToken(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state, Core::Data::Token const *token
  ) {
  }

  protected: virtual void addData(
    SharedPtr<TiObject> const &data, Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    Int levelIndex
  ) {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      auto currentData = state->getData(levelIndex).ti_cast_get<TYPE>();
      if (currentData != 0) {
        TiObject *operand;
        // Remove the unneeded bracket.
        if (data->isDerivedFrom<Core::Data::Ast::Bracket>()) {
          operand = data.s_cast_get<Core::Data::Ast::Bracket>()->getOperand().get();
        } else {
          operand = data.get();
        }
        // The operand which is at index 0 will be set by the parent production, so we'll skip that.
        if (operand != 0) {
          if (currentData->getElementCount() >= 2) {
            currentData->setElement(1, operand);
          } else {
            state->addNotice(newSrdObj<Core::Notices::SyntaxErrorNotice>(
              Core::Data::Ast::findSourceLocation(operand)
            ));
          }
        }
        return;
      }
    }
    GenericParsingHandler::addData(data, parser, state, levelIndex);
  }

}; // class

} } // namespace

#endif
