/**
 * @file Spp/Notices/MultipleCalleeMatchNotice.cpp
 * Contains the implementation of class Spp::Notices::MultipleCalleeMatchNotice.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Spp::Notices
{

//==============================================================================
// Member Functions

void MultipleCalleeMatchNotice::buildDescription(Str &str) const
{
  auto fmt = Core::Notices::L18nDictionary::getSingleton()->get(
    this->getCode(),
    S("Multiple matches were found for the given callee at these locations:\n%s\n%s\nCalled from:")
  );
  auto loc1 = Core::Notices::getSourceLocationString(this->sourceLocation1.get());
  auto loc2 = Core::Notices::getSourceLocationString(this->sourceLocation2.get());
  str.alloc(getStrLen(fmt) + loc1.getLength() + loc2.getLength());
  sprintf((Char*)str.getBuf(), fmt, loc1.getBuf(), loc2.getBuf());
}

Bool MultipleCalleeMatchNotice::isEqual(Notice *notice) const
{
  auto mcmNotice = ti_cast<MultipleCalleeMatchNotice>(notice);
  if (mcmNotice == 0) return false;
  return Data::isEqual(this->getSourceLocation().get(), mcmNotice->getSourceLocation().get()) &&
    Data::isEqual(this->sourceLocation1.get(), mcmNotice->sourceLocation1.get()) &&
    Data::isEqual(this->sourceLocation2.get(), mcmNotice->sourceLocation2.get());
}

} // namespace
