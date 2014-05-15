/**
 * @file Core/Data/DataStack.h
 * Contains the header of class Core::Data::DataStack.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_DATASTACK_H
#define DATA_DATASTACK_H

namespace Core { namespace Data
{

/**
 * @brief A stack of IdentifiableObject derived data objects.
 */
class DataStack : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(DataStack, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: std::vector<SharedPtr<IdentifiableObject>> stack;

  private: DataStack *trunkStack;
  private: Int trunkIndex;


  //============================================================================
  // Constructor & Destructor

  public: DataStack() : trunkStack(0), trunkIndex(-1)
  {
  }

  public: DataStack(Word reserveSize) : stack(reserveSize), trunkStack(0), trunkIndex(-1)
  {
    this->stack.resize(0);
  }

  public: virtual ~DataStack()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Data Functions
  /// @{

  public: void push(const SharedPtr<IdentifiableObject> &obj)
  {
    this->stack.push_back(obj);
  }

  public: void push(IdentifiableObject *obj=0)
  {
    this->stack.push_back(SharedPtr<IdentifiableObject>(obj));
  }

  public: void pop();

  public: void set(const SharedPtr<IdentifiableObject> &obj, Int index = -1);

  public: const SharedPtr<IdentifiableObject>& get(Int index = -1) const;

  /**
   * @brief Copy the content of another stack into this stack.
   * This will wipe all contents currently in the stack.
   */
  public: void copyFrom(const DataStack *src);

  public: void clear()
  {
    this->stack.clear();
    this->trunkStack = 0;
    this->trunkIndex = -1;
  }

  public: Word getCount() const
  {
    return this->trunkIndex + 1 + this->stack.size();
  }

  /// Checks whether another SharedPtr is sharing the object at given level.
  public: Bool isShared(Int index = -1) const;

  /// @}

  /// @name Branching Functions
  /// @{

  public: void setBranchingInfo(DataStack *ds, Int ti);

  public: DataStack* getTrunkStack() const
  {
    return this->trunkStack;
  }

  private: void ownTopLevel();

  /// @}

  /// @name Misc Functions
  /// @{

  public: void reserveLevels(Word size)
  {
    this->stack.reserve(size);
  }

  public: Word getReservedLevelCount() const
  {
    return this->stack.capacity();
  }

  /// @}

}; // class

} } // namespace

#endif
