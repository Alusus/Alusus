/**
 * @file Core/Processing/Handlers/ChainOpParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::ChainOpParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

//==============================================================================
// Overloaded Abstract Functions

void ChainOpParsingHandler::addData(SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex)
{
  if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
    if (state->refTermLevel(levelIndex).getPosId() > 1) {
      auto container = data.ti_cast_get<Containing<TiObject>>();
      if (container == 0) {
        throw EXCEPTION(InvalidArgumentException, S("data"),
                        S("Invalid object type received from chain op production"),
                        data->getMyTypeInfo()->getUniqueName());
      }
      if (container->getElement(0) != 0) {
        throw EXCEPTION(InvalidArgumentException, S("data"),
                        S("Data is found at index 0."));
      }
      container->setElement(0, state->getData(levelIndex).get());

      auto myMetadata = data.ti_cast_get<Data::Ast::MetaHaving>();
      auto srcMetadata = ti_cast<Data::Ast::MetaHaving>(container->getElement(0));
      if (myMetadata != 0 && srcMetadata != 0) {
        myMetadata->setSourceLocation(srcMetadata->findSourceLocation());
      }

      state->setData(data, levelIndex);
      return;
    }
  }
  GenericParsingHandler::addData(data, parser, state, levelIndex);
}

} } } // namespace
