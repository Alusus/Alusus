/**
 * @file Core/Processing/BuildMsgStore.cpp
 * Contains the implementation of class Core::Processing::BuildMsgStore.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

//==============================================================================
// Member Functions

/**
 * This value is used for optimization purposes. It's used to delay the copying
 * of build msg objects as much as possible after a state branch. This can be
 * useful because in many cases we might not need to copy those build msgs at
 * all (build msgs gets flushed from the trunk, or this branched state dies).
 */
void BuildMsgStore::setTrunkSharedCount(Word count)
{
  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, STR("No trunk store set for this store."));
  }
  if (count < 0 || count > this->trunkStore->getCount()) {
    throw EXCEPTION(InvalidArgumentException, STR("count"), STR("Out of range."), count);
  }
  this->trunkSharedCount = count;
}


/**
 * Copy the shared build msgs from the trunk state into this object then
 * reset the trunk shared build msg count.
 *
 * @sa setTrunkSharedBuildMsgCount
 */
void BuildMsgStore::copyTrunkSharedMsgs()
{
  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, STR("No trunk store set for this store."));
  }
  ASSERT(this->trunkSharedCount >= 0 &&
         this->trunkSharedCount <= this->trunkStore->getTrunkSharedCount());
  this->buildMsgs.insert(this->buildMsgs.begin(),
                         this->trunkStore->buildMsgs.begin(),
                         this->trunkStore->buildMsgs.begin()+this->trunkSharedCount);
  this->trunkSharedCount = 0;
}

} } // namespace
