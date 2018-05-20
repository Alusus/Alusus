/**
 * @file Core/Notices/Store.h
 * Contains the header of class Core::Notices::Store.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_NOTICES_STORE_H
#define CORE_NOTICES_STORE_H

namespace Core::Notices
{

// TODO: DOC

class Store
{
  //============================================================================
  // Member Variables

  private: std::vector<SharedPtr<Notice>> notices;

  private: Data::SourceLocationStack prefixSourceLocationStack;

  /// A notice store from which this store branches.
  private: Store *trunkStore = 0;

  /// Count of notices in the trunk shared by this store.
  private: Word trunkSharedCount = 0;


  //============================================================================
  // Constructor / Destructor

  public: Store()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Notice Functions
  /// @{

  /// Add a notice to the end of the list.
  public: void add(SharedPtr<Notice> const &notice);

  /**
   * @brief Get the count of notices in this state.
   *
   * This does not include shared notices in the trunk state.
   */
  public: Word getCount() const
  {
    return this->notices.size();
  }

  /**
   * @brief Get the notice with the given index.
   *
   * @param i The index of the requested notice within this state. Notices
   *          contained in the trunk state (if any) that are also shared by
   *          this state are not counted by this index.
   */
  public: SharedPtr<Notice> const& get(Int i) const
  {
    if (static_cast<Word>(i) >= this->notices.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("i"), STR("Index out of range."), i);
    }
    return this->notices[i];
  }

  /// Remove a number of notices from the beginning of the array.
  public: void flush(Word count)
  {
    if (count <= 0 || static_cast<Word>(count) > this->notices.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("count"),
                      STR("Count is less than 0, or exceeds the total number of notices."), count);
    }
    this->notices.erase(this->notices.begin(), this->notices.begin()+count);
  }

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setTrunkStore(Store *store, Word sharedNoticeCount=0)
  {
    this->trunkStore = store;
    this->setTrunkSharedCount(sharedNoticeCount);
  }

  public: Store* getTrunkStore() const
  {
    return this->trunkStore;
  }

  /// Set the count of trunk notices shared by this state.
  public: void setTrunkSharedCount(Word count);

  /**
   * @brief Get the count of trunk notices shared by this state.
   * @sa setTrunkSharedCount
   */
  public: Word getTrunkSharedCount() const
  {
    return this->trunkSharedCount;
  }

  /// Copy the shared notices from the trunk into this state's list.
  public: void copyTrunkSharedNotices();

  public: void clear()
  {
    this->notices.clear();
    this->trunkStore = 0;
    this->trunkSharedCount = 0;
  }

  /// @}

  /// @name Prefix Source Location Stack Functions
  /// @{

  public: void clearPrefixSourceLocationStack()
  {
    this->prefixSourceLocationStack.clear();
  }

  public: Data::SourceLocationStack const& getPrefixSourceLocationStack() const
  {
    return this->prefixSourceLocationStack;
  }

  public: void pushPrefixSourceLocation(Data::SourceLocation *sl)
  {
    this->prefixSourceLocationStack.push(sl);
  }

  public: void popPrefixSourceLocation(Word count)
  {
    this->prefixSourceLocationStack.pop(count);
  }

  public: void copyPrefixSourceLocationStack(Store *store)
  {
    this->clearPrefixSourceLocationStack();
    for (Int i = 0; i < store->getPrefixSourceLocationStack().getCount(); ++i) {
      this->pushPrefixSourceLocation(store->getPrefixSourceLocationStack().get(i).get());
    }
  }

  /// @}

}; // class

} // namespace

#endif
