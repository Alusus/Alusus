/**
 * @file Spp/Handlers/WhileParsingHandler.h
 * Contains the header of class Spp::Handlers::WhileParsingHandler
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_WHILEPARSINGHANDLER_H
#define SPP_HANDLERS_WHILEPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class WhileParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Constructor

  public: WhileParsingHandler()
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
      state->addNotice(std::make_shared<Spp::Notices::InvalidWhileStatementNotice>(exprMetadata->findSourceLocation()));
      state->setData(SharedPtr<TiObject>(0));
      return;
    }

    auto newObj = std::make_shared<Spp::Ast::WhileStatement>();
    newObj->setSourceLocation(exprMetadata->findSourceLocation());
    newObj->setProdId(exprMetadata->getProdId());
    for (Int i = 0; i < expr->getCount() - 1; ++i) {
      newObj->setElement(i, expr->getElement(i + 1));
    }

    state->setData(newObj);
  }

}; // class

} } // namespace

#endif
