/**
 * @file Core/Processing/Handlers/ListExpParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ListExpParsingHandler
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_LISTEXPPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_LISTEXPPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

template <class TYPE> class ListExpParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(ListExpParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (TYPE));


  //============================================================================
  // Constructor

  public: ListExpParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (this->isListTerm(state, levelIndex) && state->isAProdRoot(levelIndex - 1)) {
      TiObject *currentData = state->getData(levelIndex).get();
      if (state->refTermLevel(levelIndex).getPosId() == 1) {
        ASSERT(currentData == 0);
        if (state->refTermLevel(levelIndex + 1).getPosId() == 1) {
          state->setData(data, levelIndex);
        } else {
          state->setData(this->createListWithItems(TioSharedPtr::null, data), levelIndex);
        }
      } else {
        auto list = ti_cast<TYPE>(currentData);
        if (list != 0 && list->getProdId() == UNKNOWN_ID) {
          this->prepareToModifyData(state, levelIndex);
          list = state->getData(levelIndex).ti_cast_get<TYPE>();
          list->add(data);
        } else {
          state->setData(this->createListWithItems(getSharedPtr(currentData), data), levelIndex);
        }
      }
      return;
    }
    GenericParsingHandler::addData(data, state, levelIndex);
  }

  protected: virtual SharedPtr<TiObject> createListNode(ParserState *state, Int levelIndex)
  {
    return std::make_shared<TYPE>();
  }

  protected: SharedPtr<TYPE> createListWithItems(TioSharedPtr const &i1, TioSharedPtr const &i2)
  {
    auto list = std::make_shared<TYPE>();
    list->add(i1);
    list->add(i2);
    auto metadata = i1.ti_cast_get<Data::Ast::MetaHaving>();
    if (metadata != 0) {
      list->setSourceLocation(metadata->findSourceLocation());
    }
    return list;
  }
}; // class

} // namespace

#endif
