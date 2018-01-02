/**
 * @file Core/Processing/NoticeStore.cpp
 * Contains the implementation of class Core::Processing::NoticeStore.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

void NoticeStore::add(SharedPtr<Data::Notice> const &notice)
{
  auto count = this->prefixSourceLocationStack.getCount();
  if (count > 1 || (count == 1 && notice->getSourceLocation() != 0)) {
    // We have more than one record, so we need a new stack.
    auto stack = std::make_shared<Data::SourceLocationStack>();
    stack->push(&this->prefixSourceLocationStack);
    auto sl = notice->getSourceLocation().get();
    if (sl != 0) stack->push(sl);
    notice->setSourceLocation(stack);
  } else if (count == 1) {
    // We have a single record in the stack and no record in the notice.
    notice->setSourceLocation(this->prefixSourceLocationStack.get(0));
  }
  this->notices.push_back(notice);
}


/**
 * This value is used for optimization purposes. It's used to delay the copying
 * of notice objects as much as possible after a state branch. This can be
 * useful because in many cases we might not need to copy those notices at
 * all (notices gets flushed from the trunk, or this branched state dies).
 */
void NoticeStore::setTrunkSharedCount(Word count)
{
  if (this->trunkStore == 0 && count == 0) {
    this->trunkSharedCount = count;
    return;
  }

  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, STR("No trunk store set for this store."));
  }
  if (count < 0 || count > this->trunkStore->getCount()) {
    throw EXCEPTION(InvalidArgumentException, STR("count"), STR("Out of range."), count);
  }
  this->trunkSharedCount = count;
}


/**
 * Copy the shared notices from the trunk state into this object then
 * reset the trunk shared notice count.
 *
 * @sa setTrunkSharedCount
 */
void NoticeStore::copyTrunkSharedNotices()
{
  if (this->trunkStore == 0) {
    throw EXCEPTION(GenericException, STR("No trunk store set for this store."));
  }
  ASSERT(this->trunkSharedCount >= 0 &&
         this->trunkSharedCount <= this->trunkStore->getCount());
  this->notices.insert(this->notices.begin(),
                         this->trunkStore->notices.begin(),
                         this->trunkStore->notices.begin()+this->trunkSharedCount);
  this->trunkSharedCount = 0;
}

} } // namespace
