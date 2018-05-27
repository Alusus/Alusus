/**
 * @file Core/Processing/Handlers/ScopeParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ScopeParsingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_SCOPEPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_SCOPEPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

template <class TYPE> class ScopeParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(ScopeParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (TYPE));


  //============================================================================
  // Member Variables

  private: Int rootScopeIndex;


  //============================================================================
  // Constructor

  public: ScopeParsingHandler(Int rootIndex) : rootScopeIndex(rootIndex)
  {
  }

  public: static SharedPtr<ScopeParsingHandler<TYPE>> create(Int rootIndex)
  {
    return std::make_shared<ScopeParsingHandler<TYPE>>(rootIndex);
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdStart(Parser *parser, ParserState *state, Data::Token const *token)
  {
    if (this->rootScopeIndex == state->getProdLevelCount() - 1) {
      auto scope = parser->getRootScope();
      if (scope == 0) {
        throw EXCEPTION(GenericException, STR("Root scope is null."));
      }
      state->setData(scope);
    } else {
      state->setData(std::make_shared<TYPE>());
    }
  }

  public: virtual void onProdEnd(Parser *parser, ParserState *state)
  {
    if (this->rootScopeIndex == state->getProdLevelCount() - 1) return;
    GenericParsingHandler::onProdEnd(parser, state);
  }

  public: virtual void onLevelExit(Parser *parser, ParserState *state, SharedPtr<TiObject> const &data)
  {
    // TODO: Merge definitions.
    // TODO: Merge StatementLists into the scope.
    GenericParsingHandler::onLevelExit(parser, state, data);
  }

  protected: void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) {
      auto listContainer = state->getData(levelIndex).ti_cast_get<ListContaining<TiObject>>();
      ASSERT(listContainer);
      listContainer->addElement(data.get());
    } else {
      GenericParsingHandler::addData(data, state, levelIndex);
    }
  }

  protected: virtual void prepareToModifyData(ParserState *state, Int levelIndex)
  {
    if (this->rootScopeIndex == state->getProdLevelCount() - 1) return;
    GenericParsingHandler::prepareToModifyData(state, levelIndex);
  }

}; // class

} } } // namespace

#endif
