/**
 * @file Spp/CodeGen/GlobalItemRepo.cpp
 * Contains the implementation of class Spp::CodeGen::GlobalItemRepo.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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
  for (Int i = 0; i < this->map.getLength(); ++i) {
    if (this->map.valAt(i).size != 0) free(this->map.valAt(i).ptr);
  }
}


//==============================================================================
// Member Function

void GlobalItemRepo::addItem(Char const *name, Word size, void *ptr)
{
  Int i = this->map.findPos(Str(true, name));
  if (i == -1) {
    void *ptrCopy = malloc(size);
    if (ptr == 0) {
      memset(ptrCopy, 0, size);
    } else {
      memcpy(ptrCopy, ptr, size);
    }
    this->map(name) = Entry(size, ptrCopy);
  } else {
    if (this->map.valAt(i).size != size) {
      throw EXCEPTION(GenericException, S("An existing variable is found with a different size."));
    }
  }
}


void GlobalItemRepo::addItem(Char const *name, void *ptr)
{
  Int i = this->map.findPos(Str(true, name));
  if (i == -1) {
    this->map(name) = Entry(0, ptr);
  } else {
    throw EXCEPTION(GenericException, S("An existing variable is found with a different size."));
  }
}


Str const& GlobalItemRepo::getItemName(Int i) const
{
  if (i < 0 || i >= this->map.getLength()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->map.keyAt(i);
}


Word GlobalItemRepo::getItemSize(Int i) const
{
  if (i < 0 || i >= this->map.getLength()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->map.valAt(i).size;
}


void* GlobalItemRepo::getItemPtr(Int i) const
{
  if (i < 0 || i >= this->map.getLength()) {
    throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
  }
  return this->map.valAt(i).ptr;
}

} // namespace
