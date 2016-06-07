/**
 * @file Core/Processing/Handlers/ChainOpParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::ChainOpParsingHandler
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_CHAINOPPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_CHAINOPPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

class ChainOpParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ChainOpParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: ChainOpParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      if (state->refTermLevel(levelIndex).getPosId() > 1) {
        auto myData = this->prepareToModifyData(data, state, levelIndex);
        auto container = myData.tii_cast_get<Data::Container>();
        if (container == 0) {
          throw EXCEPTION(InvalidArgumentException, STR("data"),
                          STR("Invalid object type received from chain op production"),
                          data->getMyTypeInfo()->getUniqueName());
        }
        if (container->get(0) != 0) {
          throw EXCEPTION(InvalidArgumentException, STR("data"),
                          STR("Data is found at index 0."));
        }
        container->set(0, state->getData(levelIndex).get());
        state->setData(myData, levelIndex);
        return;
      }
    }
    GenericParsingHandler::addData(data, state, levelIndex);
  }

  protected: virtual Bool isListObjEnforced(ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) return false;
    else return GenericParsingHandler::isListObjEnforced(state, levelIndex);
  }

  private: TioSharedPtr prepareToModifyData(TioSharedPtr const &data, Processing::ParserState *state, Int levelIndex)
  {
    // There is an edge case in which this is not accurate. If the state is shared at this level
    // but not at the level where data is originated then we'll be unnecessarily cloning data.
    // This is ok since it's an edge case and will only result in performance hit; the data will
    // still be valid.
    if (state->isDataShared(levelIndex)) {
      // Duplicate the data, if possible.
      auto clonable = data.tii_cast_get<Data::Clonable>();
      if (clonable != 0) return clonable->clone();
    }
    return data;
  }

}; // class

} } } // namespace

#endif
