/**
 * @file Core/Processing/Handlers/ListParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ListParsingHandler
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_LISTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_LISTPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class TYPE> class ListParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(ListParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     TYPE);


  //============================================================================
  // Member Variables

  private: Int startIndex;


  //============================================================================
  // Constructor

  public: ListParsingHandler(Int sIndex = 0) : startIndex(sIndex)
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      TiObject *currentData = state->getData(levelIndex).get();
      if (currentData != 0) {
        // There is three possible situations at this point: Either the list was enforced, or
        // a child data was set into this level, or this level was visited more than once causing
        // a list to be created.
        TYPE *typedCurrentData = tio_cast<TYPE>(currentData);
        if (typedCurrentData != 0 && typedCurrentData->getProdId() == UNKNOWN_ID) {
          // This level already has a list that belongs to this production, so we can just add the new data
          // to this list.
          this->prepareToModifyData(state, levelIndex);
          typedCurrentData = state->getData(levelIndex).tio_cast_get<TYPE>();
          ASSERT(typedCurrentData != 0);
          auto posId = state->refTermLevel(levelIndex).getPosId();
          typedCurrentData->set(this->startIndex + posId - 1, data);
        } else {
          // At this point, posId must be 1 since the list is not enforced and the current data is
          // not null, meaning we've already set data at this level.
          TioSharedPtr list = this->createListNode(state, levelIndex);
          auto metadata = tii_cast<Data::Ast::MetadataHolder>(currentData);
          if (metadata != 0) {
            list.s_cast_get<TYPE>()->setSourceLocation(metadata->getSourceLocation());
          }
          auto newContainer = list.tii_cast_get<Data::Container>();
          newContainer->set(this->startIndex + 0, currentData);
          newContainer->set(this->startIndex + 1, data.get());
          state->setData(list, levelIndex);
        }
        return;
      }
    }
    GenericParsingHandler::addData(data, state, levelIndex);
  }

  protected: virtual SharedPtr<TiObject> createListNode(ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) {
      return std::make_shared<TYPE>();
    } else {
      return GenericParsingHandler::createListNode(state, levelIndex);
    }
  }

}; // class

} } } // namespace

#endif
