/**
 * @file Core/Processing/Handlers/ScopeParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ScopeParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_SCOPEPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_SCOPEPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

template <class TYPE> class ScopeParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(ScopeParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org",
                     (TYPE));


  //============================================================================
  // Member Variables

  private: Ast::Seeker *seeker;


  //============================================================================
  // Constructor

  public: ScopeParsingHandler(Ast::Seeker *seeker) : seeker(seeker)
  {
  }

  public: static SharedPtr<ScopeParsingHandler<TYPE>> create(Ast::Seeker *seeker)
  {
    return newSrdObj<ScopeParsingHandler<TYPE>>(seeker);
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state, Ast::Token const *token)
  {
    state->setData(newSrdObj<TYPE>());
  }

  public: virtual void onLevelExit(Parser *parser, ParserState *state, SharedPtr<Ast::Node> const &data)
  {
    // TODO: Merge StatementLists into the scope.
    GenericParsingHandler::onLevelExit(parser, state, data);
  }

  protected: virtual void addData(SharedPtr<Ast::Node> const &data, Parser *parser, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) {
      auto listContainer = state->getData(levelIndex).ti_cast_get<DynamicContaining<Ast::Node>>();
      ASSERT(listContainer);
      Core::Ast::addPossiblyMergeableElement(data.get(), listContainer, this->seeker, state->getNoticeStore());
    } else {
      GenericParsingHandler::addData(data, parser, state, levelIndex);
    }
  }

}; // class

} // namespace

#endif
