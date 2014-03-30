/**
 * @file Core/Data/Module.cpp
 * Contains the implementation of class Data::Module.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdarg.h>

namespace Core { namespace Data
{

//==============================================================================
// Constructors

Module::Module(const std::initializer_list<Argument<const Char*>> &args) : definitions(true)
{
  this->definitions.contentChangeNotifier.connect(this, &Module::onDefinitionsContentChanged);

  for (auto arg : args) {
    this->add(arg.id, arg.ioVal);
  }
}


//==============================================================================
// DataOwner Implementation

void Module::unsetIndexes(Int from, Int to)
{
  this->definitions.unsetIndexes(from, to);
}

} } // namespace
