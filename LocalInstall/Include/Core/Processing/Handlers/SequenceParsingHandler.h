/**
 * @file Core/Processing/Handlers/SequenceParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::SequenceParsingHandler
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_SEQUENCEPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_SEQUENCEPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

template <class TYPE> class SequenceParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SequenceParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (TYPE));


  //============================================================================
  // Member Variables

  private: Int startIndex;
  private: Bool rootOnly;


  //============================================================================
  // Constructor

  public: SequenceParsingHandler(Int sIndex = -1, Bool ro = true) : startIndex(sIndex), rootOnly(ro)
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (this->isEnabled(state, levelIndex) && this->isListTerm(state, levelIndex) && this->startIndex != -1) {
      TiObject *currentData = state->getData(levelIndex).get();
      if (currentData != 0) {
        // There is three possible situations at this point: Either the list was enforced, or
        // a child data was set into this level, or this level was visited more than once causing
        // a list to be created.
        TYPE *typedCurrentData = ti_cast<TYPE>(currentData);
        if (typedCurrentData != 0 && typedCurrentData->getProdId() == UNKNOWN_ID) {
          // This level already has a list that belongs to this production, so we can just add the new data
          // to this list.
          this->prepareToModifyData(state, levelIndex);
          typedCurrentData = state->getData(levelIndex).ti_cast_get<TYPE>();
          ASSERT(typedCurrentData != 0);
          auto posId = state->refTermLevel(levelIndex).getPosId();
          typedCurrentData->setElement(this->startIndex + posId - 1, data.get());
        } else {
          // At this point, posId must be 1 since the list is not enforced and the current data is
          // not null, meaning we've already set data at this level.
          TioSharedPtr list = this->createListNode(state, levelIndex);
          auto metadata = ti_cast<Data::Ast::MetaHaving>(currentData);
          if (metadata != 0) {
            list.s_cast_get<TYPE>()->setSourceLocation(metadata->findSourceLocation());
          }
          auto newContainer = list.ti_cast_get<Containing<TiObject>>();
          newContainer->setElement(this->startIndex + 0, currentData);
          newContainer->setElement(this->startIndex + 1, data.get());
          state->setData(list, levelIndex);
        }
        return;
      } else if (this->isListItemEnforced(state, levelIndex) && state->refTermLevel(levelIndex).getPosId() > 1) {
        TioSharedPtr list = this->createListNode(state, levelIndex);
        auto metadata = data.ti_cast_get<Data::Ast::MetaHaving>();
        if (metadata != 0) {
          list.s_cast_get<TYPE>()->setSourceLocation(metadata->findSourceLocation());
        }
        auto newContainer = list.ti_cast_get<Containing<TiObject>>();
        newContainer->setElement(this->startIndex + 0, currentData);
        newContainer->setElement(this->startIndex + 1, data.get());
        state->setData(list, levelIndex);
        return;
      }
    }
    GenericParsingHandler::addData(data, state, levelIndex);
  }

  protected: virtual SharedPtr<TiObject> createListNode(ParserState *state, Int levelIndex)
  {
    if (this->isEnabled(state, levelIndex)) {
      return std::make_shared<TYPE>();
    } else {
      return GenericParsingHandler::createListNode(state, levelIndex);
    }
  }

  protected: Bool isEnabled(ParserState *state, Int levelIndex)
  {
    return state->isAProdRoot(levelIndex) || !this->rootOnly;
  }

}; // class

} // namespace

#endif
