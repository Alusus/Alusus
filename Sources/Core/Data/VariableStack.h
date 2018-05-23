/**
 * @file Core/Data/VariableStack.h
 * Contains the header of class Core::Data::VariableStack.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_VARIABLESTACK_H
#define CORE_DATA_VARIABLESTACK_H

namespace Core::Data
{

// TODO: DOC

class VariableStack : public TiObject, public virtual MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(VariableStack, TiObject, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(MapContaining<TiObject>)
  ));


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
    return this->maxStrSize * sizeof(Char) + sizeof(Node*);
  }

  private: static Word getRecordSize(Word maxStrSize)
  {
    return maxStrSize * sizeof(Char) + sizeof(Node*);
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
    return this->getLevelCount() == 0 || (this->getLevelCount() == 1 && this->getElementCount() == 0);
  }

  /// @}

  /// @name Variable Access Functions
  /// @{

  public: Int add(Char const *key, TiObject *val);

  public: Int set(Char const *key, TiObject *val, Bool insertIfNew);

  public: Int getCount(Int levelIndex) const;

  public: TiObject* get(Char const *key, Int levelIndex) const;

  public: TiObject* get(Int index, Int levelIndex) const;

  public: const SbStr& getKey(Int index, Int levelIndex) const;

  public: Int getIndex(Char const *key, Int levelIndex = -1) const;

  public: Int findIndex(Char const *key, Int levelIndex) const;

  private: Int findIndex(Char const *key, Int start, Int end) const;

  /// @}


  //============================================================================
  // MapContaining Implementation

  public: virtual void setElement(Int index, TiObject *val);

  public: virtual void removeElement(Int index);

  public: virtual Word getElementCount() const
  {
    return this->getCount(-1);
  }

  public: virtual TiObject* getElement(Int index) const
  {
    return this->get(index, -1);
  }

  public: virtual Int setElement(Char const *key, TiObject *val)
  {
    return this->set(key, val, true);
  }

  public: virtual Int removeElement(Char const *key);

  public: virtual TiObject* getElement(Char const *key) const
  {
    return this->get(key, -1);
  }

  public: virtual const SbStr& getElementKey(Int index) const
  {
    return this->getKey(index, -1);
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findElementIndex(Char const *key) const
  {
    return this->findIndex(key, -1);
  }

}; // class

} // namespace

#endif
