/**
 * @file Core/Processing/Handlers/ChainOpParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::ChainOpParsingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void ChainOpParsingHandler::addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
{
  if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
    if (state->refTermLevel(levelIndex).getPosId() > 1) {
      auto myData = this->prepareToModifyData(data, state, levelIndex);
      auto container = myData.ti_cast_get<Basic::Containing<TiObject>>();
      if (container == 0) {
        throw EXCEPTION(InvalidArgumentException, STR("data"),
                        STR("Invalid object type received from chain op production"),
                        data->getMyTypeInfo()->getUniqueName());
      }
      if (container->getElement(0) != 0) {
        throw EXCEPTION(InvalidArgumentException, STR("data"),
                        STR("Data is found at index 0."));
      }
      container->setElement(0, state->getData(levelIndex).get());

      auto myMetadata = myData.ti_cast_get<Data::Ast::Metadata>();
      auto srcMetadata = ti_cast<Data::Ast::Metadata>(container->getElement(0));
      if (myMetadata != 0 && srcMetadata != 0) {
        myMetadata->setSourceLocation(srcMetadata->findSourceLocation());
      }

      state->setData(myData, levelIndex);
      return;
    }
  }
  GenericParsingHandler::addData(data, state, levelIndex);
}


TioSharedPtr ChainOpParsingHandler::prepareToModifyData(
  TioSharedPtr const &data, Processing::ParserState *state, Int levelIndex
) {
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

} } } // namespace
