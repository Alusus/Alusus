/**
 * @file Spp/CodeGen/GlobalItemRepo.cpp
 * Contains the implementation of class Spp::CodeGen::GlobalItemRepo.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Destructor

GlobalItemRepo::~GlobalItemRepo()
{
  for (Int i = 0; i < this->entries.size(); ++i) {
    if (this->entries[i].size != 0) free(this->entries[i].ptr);
  }
}


//==============================================================================
// Member Function

void GlobalItemRepo::addItem(Char const *name, Word size, void *ptr)
{
  Int i = this->index.find(Str(name));
  if (i == -1) {
    this->entries.push_back(Entry());
    this->entries.back().name = name;
    this->entries.back().size = size;
    this->entries.back().ptr = malloc(size);
    if (ptr == 0) {
      memset(this->entries.back().ptr, 0, size);
    } else {
      memcpy(this->entries.back().ptr, ptr, size);
    }
    this->index.add();
  } else {
    if (this->entries[i].size != size) {
      throw EXCEPTION(GenericException, S("An existing variable is found with a different size."));
    }
  }
}


void GlobalItemRepo::addItem(Char const *name, void *ptr)
{
  Int i = this->index.find(Str(name));
  if (i == -1) {
    this->entries.push_back(Entry());
    this->entries.back().name = name;
    this->entries.back().size = 0;
    this->entries.back().ptr = ptr;
    this->index.add();
  } else {
    throw EXCEPTION(GenericException, S("An existing variable is found with a different size."));
  }
}


Str const& GlobalItemRepo::getItemName(Int i) const
{
  if (i < 0 || i >= this->entries.size()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->entries[i].name;
}


Word GlobalItemRepo::getItemSize(Int i) const
{
  if (i < 0 || i >= this->entries.size()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->entries[i].size;
}


void* GlobalItemRepo::getItemPtr(Int i) const
{
  if (i < 0 || i >= this->entries.size()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->entries[i].ptr;
}

} // namespace
