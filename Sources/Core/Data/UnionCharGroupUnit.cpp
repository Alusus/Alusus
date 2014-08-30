/**
 * @file Core/Data/UnionCharGroupUnit.cpp
 * Contains the implementation of class Core::Data::UnionCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Constructors

UnionCharGroupUnit::UnionCharGroupUnit(std::initializer_list<SharedPtr<CharGroupUnit>> const &args)
{
  this->charGroupUnits.reserve(args.size());
  for (auto arg : args) this->charGroupUnits.push_back(arg);
}

} } // namespace
