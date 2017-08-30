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

} } // namespace
