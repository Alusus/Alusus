/**
 * @file Spp/Handlers/UseInParsingHandler.h
 * Contains the header of class Spp::Handlers::UseInParsingHandler
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_USEINPARSINGHANDLER_H
#define SPP_HANDLERS_USEINPARSINGHANDLER_H

namespace Spp::Handlers
{

class UseInParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(UseInParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: UseInParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state)
  {
    GenericParsingHandler::onProdEnd(parser, state);

    auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
    ASSERT(expr != 0);
    auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(expr);
    ASSERT(exprMetadata != 0);

    auto newObj = newSrdObj<Spp::Ast::UseInOp>();
    newObj->setSourceLocation(exprMetadata->findSourceLocation());
    newObj->setProdId(exprMetadata->getProdId());
    newObj->setOperandName(S("this"));
    newObj->setSkipInjection(false);

    if (expr->getCount() >= 3) {
      auto operandName = expr->get(1).ti_cast_get<Core::Data::Ast::Bracket>();
      if (operandName == 0) {
        newObj->setOperandName(S("this"));
        newObj->setSkipInjection(true);
      } else {
        auto identifier = operandName->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
        if (identifier == 0) {
          throw EXCEPTION(GenericException, S("Unexpected use_in operand name."));
        }
        newObj->setOperandName(identifier->getValue());
        if (expr->getCount() == 4) {
          newObj->setSkipInjection(true);
        }
      }
    }

    // Set the body.
    auto body = expr->get(expr->getCount() - 1).ti_cast<Spp::Ast::Block>();
    if (body == 0) {
      throw EXCEPTION(GenericException, S("Invalid use_in operator body."));
    }
    newObj->setBody(body);

    state->setData(newObj);
  }

}; // class

} // namespace

#endif
