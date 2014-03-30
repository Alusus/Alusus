/**
 * @file Core/Data/VariableStack.h
 * Contains the header of class Data::VariableStack.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_VARIABLE_STACK_H
#define DATA_VARIABLE_STACK_H

namespace Core { namespace Data
{

// TODO: DOC

class VariableStack : public IdentifiableObject, public virtual MapPlainContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(VariableStack, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, MapPlainContainer);


  //============================================================================
  // Member Variables

  private: static const Int recordCountIncrement;

  private: Byte *buffer;
  private: Word maxStrSize;
  private: Word reservedRecordCount;
  private: std::vector<Word> levels;

  private: VariableStack *trunkStack;
  private: Int trunkLevelIndex;


  //============================================================================
  // Constructor & Destructor

  public: VariableStack() : buffer(0), trunkStack(0), trunkLevelIndex(-1)
  {
  }

  public: VariableStack(Word maxStrSize, Word rsrvdRcrdCount, Word reservedLevelCount) :
    buffer(0), trunkStack(0), trunkLevelIndex(-1), levels(reservedLevelCount)
  {
    this->levels.resize(0);
    this->initialize(maxStrSize, rsrvdRcrdCount, reservedLevelCount);
  }

  public: virtual ~VariableStack()
  {
    this->release();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Related Functions
  /// @{

  public: void initialize(Word maxStrSize, Word reservedRecordCount, Word reservedLevelCount);

  public: void reinitialize(Word maxStrSize, Word reservedRecordCount, Word reservedLevelCount);

  public: void copy(const VariableStack *src);

  public: void copyLevel(const VariableStack *src, Int level);

  public: void release();

  public: Word getMaxStrSize() const
  {
    return this->maxStrSize;
  }

  public: Word getReservedRecordCount() const
  {
    return this->reservedRecordCount;
  }

  public: Word getReservedLevelCount() const
  {
    return this->levels.capacity();
  }

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setBranchingInfo(VariableStack *vs, Int tli);

  public: VariableStack* getTrunkStack() const
  {
    return this->trunkStack;
  }

  private: void ownTopLevel();

  /// @}

  /// @name Helper Functions
  /// @{

  private: Word getRecordSize() const
  {
    return this->maxStrSize * sizeof(Char) + sizeof(IdentifiableObject*);
  }

  private: static Word getRecordSize(Word maxStrSize)
  {
    return maxStrSize * sizeof(Char) + sizeof(IdentifiableObject*);
  }

  /// @}

  /// @name Level Management Functions
  /// @{

  public: void pushLevel();

  public: void popLevel();

  public: void clear()
  {
    this->levels.clear();
    this->trunkStack = 0;
    this->trunkLevelIndex = -1;
  }

  public: Word getLevelCount() const
  {
    return this->trunkLevelIndex + 1 + this->levels.size();
  }

  public: Bool isEmpty() const
  {
    return this->getCount() == 0 || (this->getLevelCount() == 1 && this->getCount() == 0);
  }

  /// @}

  /// @name Variable Access Functions
  /// @{

  public: Int add(const Char *key, IdentifiableObject *val);

  public: Int set(const Char *key, IdentifiableObject *val, Bool insertIfNew);

  public: Int getCount(Int levelIndex) const;

  public: IdentifiableObject* get(const Char *key, Int levelIndex) const;

  public: IdentifiableObject* get(Int index, Int levelIndex) const;

  public: const SbStr& getKey(Int index, Int levelIndex) const;

  public: Int getIndex(const Char *key, Int levelIndex = -1) const;

  public: Int findIndex(const Char *key, Int levelIndex) const;

  private: Int findIndex(const Char *key, Int start, Int end) const;

  /// @}


  //============================================================================
  // MapPlainContainer Implementation

  public: virtual void set(Int index, IdentifiableObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return this->getCount(-1);
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->get(index, -1);
  }

  public: virtual Int set(const Char *key, IdentifiableObject *val)
  {
    return this->set(key, val, true);
  }

  public: virtual void remove(const Char *key);

  public: virtual IdentifiableObject* get(const Char *key) const
  {
    return this->get(key, -1);
  }

  public: virtual const Str& getKey(Int index) const
  {
    // TODO: Use thread_local.
    static Str s = this->getKey(index, -1);
    return s;
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(const Char *key) const
  {
    return this->findIndex(key, -1);
  }

}; // class

} } // namespace

#endif
