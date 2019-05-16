/**
 * @file Core/Processing/Handlers/ScopeParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ScopeParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  // Constructor

  public: ScopeParsingHandler()
  {
  }

  public: static SharedPtr<ScopeParsingHandler<TYPE>> create()
  {
    return std::make_shared<ScopeParsingHandler<TYPE>>();
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state, Data::Token const *token)
  {
    state->setData(std::make_shared<TYPE>());
  }

  public: virtual void onLevelExit(Parser *parser, ParserState *state, SharedPtr<TiObject> const &data)
  {
    // TODO: Merge StatementLists into the scope.
    GenericParsingHandler::onLevelExit(parser, state, data);
  }

  protected: virtual void addData(SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) {
      auto listContainer = state->getData(levelIndex).ti_cast_get<DynamicContaining<TiObject>>();
      ASSERT(listContainer);
      Core::Data::Ast::addPossiblyMergeableElement(data.get(), listContainer, state->getNoticeStore());
    } else {
      GenericParsingHandler::addData(data, parser, state, levelIndex);
    }
  }

}; // class

} // namespace

#endif
