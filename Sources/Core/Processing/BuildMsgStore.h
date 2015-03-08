/**
 * @file Core/Processing/BuildMsgStore.h
 * Contains the header of class Core::Processing::BuildMsgStore.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_BUILDMSGSTORE_H
#define PROCESSING_BUILDMSGSTORE_H

namespace Core { namespace Processing
{

// TODO: DOC

class BuildMsgStore
{
  friend class Parser;
  friend class ParserState;

  //============================================================================
  // Member Variables

  /// The list of error/warning build messages created for this state.
  private: std::vector<SharedPtr<Processing::BuildMsg> > buildMsgs;

  /// A build message store from which this store branches.
  private: BuildMsgStore *trunkStore = 0;

  /// Count of build messages in the trunk shared by this state.
  private: Word trunkSharedCount = 0;


  //============================================================================
  // Constructor / Destructor

  public: BuildMsgStore()
  {
  }


  //============================================================================
  // Member Functions

  /// Add a build message to the end of the list.
  public: void add(SharedPtr<Processing::BuildMsg> const &msg)
  {
    this->buildMsgs.push_back(msg);
  }

  /**
   * @brief Get the count of build messages in this state.
   *
   * This does not include shared messages in the trunk state.
   */
  public: Word getCount() const
  {
    return this->buildMsgs.size();
  }

  /**
   * @brief Get the build message with the given index.
   *
   * @param i The index of the requested message within this state. Messages
   *          contained in the trunk state (if any) that are also shared by
   *          this state are not counted by this index.
   */
  public: SharedPtr<Processing::BuildMsg> const& get(Int i) const
  {
    if (static_cast<Word>(i) >= this->buildMsgs.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("i"), STR("Index out of range."), i);
    }
    return this->buildMsgs[i];
  }

  /// Remove a number of build messages from the beginning of the array.
  protected: void flush(Word count)
  {
    if (count <= 0 || static_cast<Word>(count) > this->buildMsgs.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("count"),
                      STR("Count is less than 0, or exceeds the total number of notificatoins."), count);
    }
    this->buildMsgs.erase(this->buildMsgs.begin(), this->buildMsgs.begin()+count);
  }

  protected: void setTrunkStore(BuildMsgStore *store, Word sharedMsgCount=0)
  {
    this->trunkStore = store;
    this->setTrunkSharedCount(sharedMsgCount);
  }

  public: BuildMsgStore* getTrunkStore() const
  {
    return this->trunkStore;
  }

  /// Set the count of trunk build messages shared by this state.
  protected: void setTrunkSharedCount(Word count);

  /**
   * @brief Get the count of trunk build messages shared by this state.
   * @sa setTrunkSharedBuildMsgCount
   */
  public: Word getTrunkSharedCount() const
  {
    return this->trunkSharedCount;
  }

  /// Copy the shared build messages from the trunk into this state's list.
  protected: void copyTrunkSharedMsgs();

  protected: void clear()
  {
    this->buildMsgs.clear();
    this->trunkStore = 0;
    this->trunkSharedCount = 0;
  }

}; // class

} } // namespace

#endif
