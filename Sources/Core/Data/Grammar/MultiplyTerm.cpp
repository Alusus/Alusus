/**
 * @file Core/Data/Grammar/MultiplyTerm.cpp
 * Contains the implementation of class Core::Data::Grammar::MultiplyTerm.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Member Functions

/**
 * Set the minimum number of occurances the child path is allowed to have.
 * This must be >= 0. If null is specified, the default value of 0 will be
 * considered. This value must be either TiInt or Reference.
 */
void MultiplyTerm::setMin(SharedPtr<TiObject> const &m)
{
  if (min != 0 && !min->isA<TiInt>() && !min->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, S("min"),
                    S("Must be of type TiInt or Reference."),
                    min->getMyTypeInfo()->getUniqueName());
  }
  UPDATE_OWNED_SHAREDPTR(this->min, m);
}


/**
 * Set the maximum number of occurances. This must be >= 0. If null is specified
 * the number of occurances will be unlimited. This value must be null, TiInt
 * or Reference.
 */
void MultiplyTerm::setMax(SharedPtr<TiObject> const &m)
{
  if (max != 0 && !max->isA<TiInt>() && !max->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, S("max"),
                    S("Must be of type TiInt or Reference."),
                    max->getMyTypeInfo()->getUniqueName());
  }
  UPDATE_OWNED_SHAREDPTR(this->max, m);
}


//==============================================================================
// CacheHaving Implementation

void MultiplyTerm::clearCache()
{
  this->innerTextBasedDecisionCache.clear();
  this->innerIdBasedDecisionCache.clear();
}

} // namespace
