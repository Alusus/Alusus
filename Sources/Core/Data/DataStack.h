/**
 * @file Core/Data/DataStack.h
 * Contains the header of class Core::Data::DataStack.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_DATASTACK_H
#define CORE_DATA_DATASTACK_H

namespace Core::Data
{

/**
 * @brief A stack of TiObject derived data objects.
 */
class DataStack : public TiObject, public virtual Basic::ListContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(DataStack, TiObject, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Basic::ListContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  private: Basic::SharedList<TiObject> stack;

  private: DataStack *trunkStack;
  private: Int trunkIndex;


  //============================================================================
  // Constructor & Destructor

  public: DataStack() : trunkStack(0), trunkIndex(-1)
  {
  }

  public: virtual ~DataStack()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Data Functions
  /// @{

  public: void push(SharedPtr<TiObject> const &obj)
  {
    this->stack.add(obj);
  }

  public: void pop();

  public: void set(SharedPtr<TiObject> const &obj, Int index = -1);

  public: SharedPtr<TiObject> const& get(Int index = -1) const;

  public: Word getCount() const
  {
    return this->trunkIndex + 1 + this->stack.getCount();
  }

  /**
   * @brief Copy the content of another stack into this stack.
   * This will wipe all contents currently in the stack.
   */
  public: void copyFrom(DataStack const *src);

  public: void clear()
  {
    this->stack.clear();
    this->trunkStack = 0;
    this->trunkIndex = -1;
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

  private: void ownTop();

  /// @}

  /// @name ListContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val)
  {
    this->set(getSharedPtr(val), index);
  }

  public: virtual void removeElement(Int index);

  public: virtual Word getElementCount() const
  {
    return this->getCount();
  }

  public: virtual TiObject* getElement(Int index) const
  {
    return this->get(index).get();
  }

  public: virtual Int addElement(TiObject *val)
  {
    return this->stack.addElement(val);
  }

  public: virtual void insertElement(Int index, TiObject *val);

  /// @}

}; // class

} // namespace

#endif
