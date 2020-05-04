/**
 * @file Spp/CodeGen/StringLiteralRepo.cpp
 * Contains the implementation of class Spp::CodeGen::StringLiteralRepo.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Destructor

StringLiteralRepo::~StringLiteralRepo()
{
  for (Int i = 0; i < this->entries.size(); ++i) {
    free(this->entries[i].str.c_str());
  }
}


//==============================================================================
// Member Function

Int StringLiteralRepo::addString(Char const *s)
{
  Int i = this->index.find(SBSTR(s));
  if (i == -1) {
    Int len = getStrLen(s);
    Char *str = static_cast<Char*>(malloc(len + 1));
    copyStr(s, str);
    i = this->entries.size();
    this->entries.push_back(Entry(str));
    this->index.add();
  }
  return i;
}


Char const* StringLiteralRepo::getString(Int i) const
{
  if (i < 0 || i >= this->entries.size()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->entries[i].str.c_str();
}

} // namespace
