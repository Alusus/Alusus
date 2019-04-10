/**
 * @file Spp/Handlers/ForParsingHandler.h
 * Contains the header of class Spp::Handlers::ForParsingHandler
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_FORPARSINGHANDLER_H
#define SPP_HANDLERS_FORPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class ForParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ForParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Constructor

  public: ForParsingHandler()
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

    if (expr->getCount() != 3) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidForStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }

    auto head = ti_cast<Core::Data::Ast::List>(expr->getElement(1));
    if (head == 0 || head->getCount() != 3) {
      state->addNotice(std::make_shared<Spp::Notices::InvalidForStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }
    for (Int i = 1; i < head->getCount(); ++i) {
      if (head->get(i) == 0) {
        state->addNotice(std::make_shared<Spp::Notices::InvalidForStatementNotice>(exprMetadata->findSourceLocation()));
        state->setData(SharedPtr<TiObject>(0));
        return;
      }
    }

    auto newObj = std::make_shared<Spp::Ast::ForStatement>();
    newObj->setSourceLocation(exprMetadata->findSourceLocation());
    newObj->setProdId(exprMetadata->getProdId());
    newObj->setInitializer(head->get(0));
    newObj->setCondition(head->get(1));
    newObj->setUpdater(head->get(2));
    newObj->setBody(expr->get(2));

    state->setData(newObj);
  }

}; // class

} } // namespace

#endif
