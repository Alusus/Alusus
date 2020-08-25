/**
 * @file Spp/Handlers/IfParsingHandler.h
 * Contains the header of class Spp::Handlers::IfParsingHandler
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_IFPARSINGHANDLER_H
#define SPP_HANDLERS_IFPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class IfParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: IfParsingHandler()
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

    if (expr->getCount() != 3 && expr->getCount() != 5) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidIfStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }

    auto newObj = newSrdObj<Spp::Ast::IfStatement>();
    newObj->setSourceLocation(exprMetadata->findSourceLocation());
    newObj->setProdId(exprMetadata->getProdId());
    newObj->setCondition(expr->get(1));
    newObj->setIfBody(expr->get(2));
    if (expr->getCount() == 5) newObj->setElseBody(expr->get(4));

    state->setData(newObj);
  }

}; // class

} } // namespace

#endif
