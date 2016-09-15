/**
 * @file Spp/Handlers/BlockParsingHandler.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_BLOCKPARSINGHANDLER_H
#define SPP_HANDLERS_BLOCKPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

template <class TYPE> class BlockParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(BlockParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
                     "Spp.Handlers", "Spp", "alusus.net", TYPE);


  //============================================================================
  // Constructor

  public: BlockParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Core::Processing::Parser *parser, Core::Processing::ParserState *state)
  {
    state->setData(std::make_shared<TYPE>());
  }

  public: virtual void onLevelExit(Core::Processing::Parser *parser, Core::Processing::ParserState *state,
                                   SharedPtr<TiObject> const &data)
  {
    Int index = state->refTopProdLevel().getTermStackIndex();
    auto block = state->getData(index).s_cast_get<TYPE>();
    ASSERT(block != 0);
    auto statementList = data.ti_cast_get<Core::Data::Ast::StatementList>();
    if (statementList != 0 && statementList->getCount() > 0) {
      block->setStatements(data);
    }
  }

  public: virtual void onNewToken(Core::Processing::Parser *parser, Core::Processing::ParserState *state,
                                  Core::Data::Token const *token)
  {
    Int prodTermIndex = state->refTopProdLevel().getTermStackIndex();
    auto block = state->getData(prodTermIndex).ti_cast<TYPE>();
    if (block != 0 && block->getSourceLocation().line == 0) {
      block->setSourceLocation(token->getSourceLocation());
    }
  }

}; // class

} } // namespace

#endif
