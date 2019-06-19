/**
 * @file Core/Data/Grammar/MultiplyTerm.cpp
 * Contains the implementation of class Core::Data::Grammar::MultiplyTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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


/**
 * Set the priority of the inner branch. This can either be the TiInt priority
 * value, or a Reference to it. The value can either be 1, which means priority
 * is for taking (or staying) in the branch, or 0 which means priority is to
 * leave the branch. If null is specified the default value of 1 is considered.
 */
void MultiplyTerm::setPriority(SharedPtr<TiObject> const &p)
{
  if (p != 0 && !p->isA<TiInt>() && !p->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, S("p"),
                    S("Must be of type TiInt or Reference."),
                    p->getMyTypeInfo()->getUniqueName());
  }
  UPDATE_OWNED_SHAREDPTR(this->priority, p);
}


//==============================================================================
// DataHaving Implementation

void MultiplyTerm::unsetIndexes(Int from, Int to)
{
  if (this->term != 0) Data::unsetIndexes(this->term.get(), from, to);
  if (this->min != 0) Data::unsetIndexes(this->min.get(), from, to);
  if (this->max != 0) Data::unsetIndexes(this->max.get(), from, to);
  if (this->priority != 0) Data::unsetIndexes(this->priority.get(), from, to);
  this->innerTextBasedDecisionCache.clear();
  this->innerIdBasedDecisionCache.clear();
}

} // namespace
