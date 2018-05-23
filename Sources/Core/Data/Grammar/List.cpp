/**
 * @file Core/Data/Grammar/List.cpp
 * Contains the implementation of class Core::Data::Grammar::List.
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

void List::initialize(TiObject *context)
{
  if (this->baseRef != 0) {
    auto grammarContext = ti_cast<Context>(context);
    if (grammarContext == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("context"), STR("Must be of type Core::Data::Grammar::Context."));
    }
    TiObject *p = grammarContext->traceValue(this->baseRef.get());
    if (p == 0) {
      throw EXCEPTION(GenericException, STR("Parent reference points to missing definition."));
    }
    List *pm = tio_cast<List>(p);
    if (pm == 0) {
      throw EXCEPTION(GenericException, STR("Parent reference points to an object of an invalid type."));
    }
    this->setBase(pm);
  }
}

} // namespace
