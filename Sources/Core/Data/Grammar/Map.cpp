/**
 * @file Core/Data/Grammar/Map.cpp
 * Contains the implementation of class Core::Data::Grammar::Map.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

void Map::initialize(TiObject *context)
{
  if (this->baseRef != 0) {
    auto grammarContext = ti_cast<Context>(context);
    if (grammarContext == 0) {
      throw EXCEPTION(InvalidArgumentException, S("context"), S("Must be of type Core::Data::Grammar::Context."));
    }
    TiObject *p = grammarContext->traceValue(this->baseRef.get());
    if (p == 0) {
      throw EXCEPTION(GenericException, S("Parent reference points to missing definition."));
    }
    Map *pm = ti_cast<Map>(p);
    if (pm == 0) {
      throw EXCEPTION(GenericException, S("Parent reference points to an object of an invalid type."));
    }
    this->setBase(pm);
  }
}

} // namespace
