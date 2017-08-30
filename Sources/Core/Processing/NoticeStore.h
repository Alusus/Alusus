/**
 * @file Core/Processing/NoticeStore.h
 * Contains the header of class Core::Processing::NoticeStore.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_NOTICESTORE_H
#define CORE_PROCESSING_NOTICESTORE_H

namespace Core { namespace Processing
{

// TODO: DOC

class NoticeStore
{
  friend class Parser;
  friend class ParserState;

  //============================================================================
  // Member Variables

  private: std::vector<SharedPtr<Data::Notice>> notices;

  /// A notice store from which this store branches.
  private: NoticeStore *trunkStore = 0;

  /// Count of notices in the trunk shared by this state.
  private: Word trunkSharedCount = 0;


  //============================================================================
  // Constructor / Destructor

  public: NoticeStore()
  {
  }


  //============================================================================
  // Member Functions

  /// Add a notice to the end of the list.
  public: void add(SharedPtr<Data::Notice> const &notice)
  {
    this->notices.push_back(notice);
  }

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
  public: SharedPtr<Data::Notice> const& get(Int i) const
  {
    if (static_cast<Word>(i) >= this->notices.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("i"), STR("Index out of range."), i);
    }
    return this->notices[i];
  }

  /// Remove a number of notices from the beginning of the array.
  protected: void flush(Word count)
  {
    if (count <= 0 || static_cast<Word>(count) > this->notices.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("count"),
                      STR("Count is less than 0, or exceeds the total number of notices."), count);
    }
    this->notices.erase(this->notices.begin(), this->notices.begin()+count);
  }

  protected: void setTrunkStore(NoticeStore *store, Word sharedNoticeCount=0)
  {
    this->trunkStore = store;
    this->setTrunkSharedCount(sharedNoticeCount);
  }

  public: NoticeStore* getTrunkStore() const
  {
    return this->trunkStore;
  }

  /// Set the count of trunk notices shared by this state.
  protected: void setTrunkSharedCount(Word count);

  /**
   * @brief Get the count of trunk notices shared by this state.
   * @sa setTrunkSharedCount
   */
  public: Word getTrunkSharedCount() const
  {
    return this->trunkSharedCount;
  }

  /// Copy the shared notices from the trunk into this state's list.
  protected: void copyTrunkSharedNotices();

  protected: void clear()
  {
    this->notices.clear();
    this->trunkStore = 0;
    this->trunkSharedCount = 0;
  }

}; // class

} } // namespace

#endif
