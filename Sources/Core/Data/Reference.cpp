/**
 * @file Core/Data/Reference.cpp
 * Contains the implementation of class Data::Reference.
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
// Member Functions

/**
 * This is useful for things like getting the immediate name
 * of a variable rather than the full path leading to it. For example, if
 * this chain represents "module1.map1.var1" then calling this function will
 * return the Reference object corresponding to "var1".
 */
const SharedPtr<ReferenceSegment>& Reference::getLastSegment()
{
  if (this->segment == 0) return this->segment;
  else {
    const SharedPtr<ReferenceSegment> *seg = &this->segment;
    while ((*seg)->getNext() != 0) seg = &(*seg)->getNext();
    return *seg;
  }
}


/**
 * This recursive function makes sure the entire chain is equivalent in value.
 * It makes sure that the id of this object and all child objects matches the
 * type and value of their equivalent in r. The chain also has to be of the
 * same length for the result to be true. In other words, if one chain is
 * longer than the other the result is false even if the rest of the chain
 * matches.
 */
Bool Reference::compare(const Reference *r) const
{
  if (r == 0) return false;
  else {
    ReferenceSegment *seg1 = this->getSegment().get();
    ReferenceSegment *seg2 = r->getSegment().get();
    while (seg1 != 0 && seg2 != 0) {
      if (seg1->compare(seg2) == false) return false;
      seg1 = seg1->getNext().get();
      seg2 = seg2->getNext().get();
    }
    if (seg1 != 0 || seg2 != 0) return false;
    else return true;
  }
}

} } // namespace
