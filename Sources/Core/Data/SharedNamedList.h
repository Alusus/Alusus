/**
 * @file Core/Data/SharedNamedList.h
 * Contains the header of class Core::Data::SharedNamedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SHAREDNAMEDLIST_H
#define CORE_DATA_SHAREDNAMEDLIST_H

namespace Core { namespace Data
{

// TODO: DOC

class SharedNamedList : public Node,
                        public virtual DataOwner, public virtual NamedListContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedNamedList, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(Node, DataOwner, NamedListContainer);


  //============================================================================
  // Member Variables

  private: static const Int recordCountIncrement;

  private: Byte *buffer;
  private: Word count;
  private: Word maxStrSize;
  private: Word reservedCount;
  private: Bool owningEnabled;


  //============================================================================
  // Constructor & Destructor

  public: SharedNamedList(Bool oe=true) : buffer(0), owningEnabled(oe)
  {
  }

  public: SharedNamedList(Word maxStrSize, Word rsrvdCount, Bool oe=true) :
    buffer(0), owningEnabled(oe)
  {
    this->initialize(maxStrSize, rsrvdCount);
  }

  public: virtual ~SharedNamedList()
  {
    this->release();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void initialize(Word maxStrSize, Word reservedCount);

  public: void reinitialize(Word maxStrSize, Word reservedCount);

  public: void copy(const SharedNamedList *src);

  public: void release();

  public: Word getMaxStrSize() const
  {
    return this->maxStrSize;
  }

  public: Word getReservedCount() const
  {
    return this->reservedCount;
  }

  public: void clear();

  public: Bool isEmpty() const
  {
    return this->count == 0;
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  public: Int add(Char const *name, SharedPtr<IdentifiableObject> const &val);

  /// Add a new object to the list.
  public: Int add(SharedPtr<IdentifiableObject> const &val)
  {
    return this->add(0, val);
  }

  public: void insert(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val);

  /// Add a new object to the list at a specific index.
  public: void insert(Int index, SharedPtr<IdentifiableObject> const &val)
  {
    return this->insert(index, 0, val);
  }

  public: void set(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val);

  /// Change the element at the specified index.
  public: void set(Int index, SharedPtr<IdentifiableObject> const &val);

  /// Get the object at the specified index.
  public: SharedPtr<IdentifiableObject> const& getShared(Int index) const;

  /// @}

  /// @name Helper Functions
  /// @{

  private: Word getRecordSize() const
  {
    return SharedNamedList::getRecordSize(this->maxStrSize);
  }

  private: static Word getRecordSize(Word maxStrSize)
  {
    return maxStrSize * sizeof(Char) + sizeof(SharedPtr<IdentifiableObject>);
  }

  /// @}

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}

  /// @name NamedListContainer Implementation
  /// @{

  /// Change the element at the specified index.
  public: virtual void set(Int index, IdentifiableObject *val)
  {
    this->set(index, getSharedPtr(val, true));
  }

  /// Remove the element at the specified index.
  public: virtual void remove(Int index);

  /// Get the count of elements in the list.
  public: virtual Word getCount() const
  {
    return this->count;
  }

  /// Get the object at the specified index.
  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->getShared(index).get();
  }

  /// Add a new object to the list.
  public: virtual Int add(IdentifiableObject *val)
  {
    return this->add(0, getSharedPtr(val, true));
  }

  public: virtual Int add(Char const *name, IdentifiableObject *val)
  {
    return this->add(name, getSharedPtr(val, true));
  }

  public: virtual void insert(Int index, IdentifiableObject *val)
  {
    this->insert(index, 0, getSharedPtr(val, true));
  }

  public: virtual void insert(Int index, Char const *name, IdentifiableObject *val)
  {
    this->insert(index, name, getSharedPtr(val, true));
  }

  public: virtual void set(Int index, Char const *name, IdentifiableObject *val)
  {
    this->set(index, name, getSharedPtr(val, true));
  }

  public: virtual const SbStr& getName(Int index) const;

  /// @}

}; // class

} } // namespace

#endif
