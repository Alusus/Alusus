/**
 * @file Core/Notices/Store.cpp
 * Contains the implementation of class Core::Notices::Store.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Notices
{

//==============================================================================
// Member Functions

void Store::add(SharedPtr<Notice> const &notice)
{
  auto count = this->prefixSourceLocationStack.getCount();
  if (count > 1 || (count == 1 && notice->getSourceLocation() != 0)) {
    // We have more than one record, so we need a new stack.
    auto stack = newSrdObj<Data::SourceLocationStack>();
    stack->push(&this->prefixSourceLocationStack);
    auto sl = notice->getSourceLocation().get();
    if (sl != 0) stack->push(sl);
    notice->setSourceLocation(stack);
  } else if (count == 1) {
    // We have a single record in the stack and no record in the notice.
    notice->setSourceLocation(this->prefixSourceLocationStack.get(0));
  }
  this->notices.push_back(notice);
  if (this->minEncounteredSeverity == -1 || notice->getSeverity() < this->minEncounteredSeverity) {
    this->minEncounteredSeverity = notice->getSeverity();
  }
}


/**
 * This value is used for optimization purposes. It's used to delay the copying
 * of notice objects as much as possible after a state branch. This can be
 * useful because in many cases we might not need to copy those notices at
 * all (notices gets flushed from the trunk, or this branched state dies).
 */
void Store::setTrunkSharedCount(Word count)
{
  if (this->trunkStore == 0 && count == 0) {
    this->trunkSharedCount = count;
    return;
  }

  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, S("No trunk store set for this store."));
  }
  if (count > this->trunkStore->getCount()) {
    throw EXCEPTION(InvalidArgumentException, S("count"), S("Out of range."), count);
  }
  this->trunkSharedCount = count;
}


/**
 * Copy the shared notices from the trunk state into this object then
 * reset the trunk shared notice count.
 *
 * @sa setTrunkSharedCount
 */
void Store::copyTrunkSharedNotices()
{
  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, S("No trunk store set for this store."));
  }
  ASSERT(this->trunkSharedCount >= 0 &&
         this->trunkSharedCount <= this->trunkStore->getCount());
  this->notices.insert(this->notices.begin(),
                         this->trunkStore->notices.begin(),
                         this->trunkStore->notices.begin()+this->trunkSharedCount);
  this->trunkSharedCount = 0;
}

} // namespace
