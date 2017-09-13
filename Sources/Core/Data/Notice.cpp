/**
 * @file Core/Data/Notice.cpp
 * Contains the implementation of class Core::Data::Notice.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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
// Static Variables

Str Notice::tempBuf;


//==============================================================================
// Member Functions

/**
 * This function generates the description in a temporary buffer and returns
 * a pointer to that temporary buffer. The user should immediately copy the
 * contents of the buffer somewhere else, or use buildDescription instead.
 *
 * @sa buildDescription()
 */
Str const& Notice::getDescription() const
{
  this->buildDescription(Notice::tempBuf);
  return Notice::tempBuf;
}


void Notice::prependSourceLocationStack(std::vector<Data::SourceLocation> const *l)
{
  if (this->sourceLocationStack == 0) {
    this->setSourceLocationStack(*l);
  } else {
    this->sourceLocationStack->reserve(this->sourceLocationStack->size() + l->size());
    for (Int i = 0; i < l->size(); ++i) {
      this->sourceLocationStack->insert(this->sourceLocationStack->begin() + i, l->at(i));
    }
  }
}


void Notice::appendSourceLocationStack(std::vector<Data::SourceLocation> const *l)
{
  if (l == 0) return;
  if (this->sourceLocationStack == 0) {
    this->setSourceLocationStack(*l);
  } else {
    this->sourceLocationStack->reserve(this->sourceLocationStack->size() + l->size());
    for (Int i = 0; i < l->size(); ++i) {
      this->sourceLocationStack->push_back(l->at(i));
    }
  }
}

} } // namespace
