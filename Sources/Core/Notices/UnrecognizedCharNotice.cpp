/**
 * @file Core/Notices/UnrecognizedCharNotice.cpp
 * Contains the implementation of class Core::Notices::UnrecognizedCharNotice.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Notices
{

//==============================================================================
// Member Functions

void UnrecognizedCharNotice::buildDescription(Str &str) const
{
  auto format = L18nDictionary::getSingleton()->get(
    this->getCode().c_str(), STR("Lexer Error [Unrecognized Character(s)]: %s")
  );
  str = formatString(format, this->getText().c_str());
}

} // namespace
