/**
 * @file Core/Data/PlainNamedList.h
 * Contains the header of class Core::Data::PlainNamedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PLAINNAMEDLIST_H
#define DATA_PLAINNAMEDLIST_H

namespace Core { namespace Data
{

// TODO: DOC

class PlainNamedList : public Node,
                       public virtual DataOwner, public virtual NamedListContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(PlainNamedList, Node, "Core.Data", "Core", "alusus.net");
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

  public: PlainNamedList(Bool oe=true) : buffer(0), owningEnabled(oe)
  {
  }

  public: PlainNamedList(Word maxStrSize, Word rsrvdCount, Bool oe=true) :
    buffer(0), owningEnabled(oe)
  {
    this->initialize(maxStrSize, rsrvdCount);
  }

  public: virtual ~PlainNamedList()
  {
    this->release();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void initialize(Word maxStrSize, Word reservedCount);

  public: void reinitialize(Word maxStrSize, Word reservedCount);

  public: void copy(const PlainNamedList *src);

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

  /// @name Helper Functions
  /// @{

  private: Word getRecordSize() const
  {
    return PlainNamedList::getRecordSize(this->maxStrSize);
  }

  private: static Word getRecordSize(Word maxStrSize)
  {
    return maxStrSize * sizeof(Char) + sizeof(IdentifiableObject*);
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
  public: virtual void set(Int index, IdentifiableObject *val);

  /// Remove the element at the specified index.
  public: virtual void remove(Int index);

  /// Get the count of elements in the list.
  public: virtual Word getCount() const
  {
    return this->count;
  }

  /// Get the object at the specified index.
  public: virtual IdentifiableObject* get(Int index) const;

  /// Add a new object to the list.
  public: virtual Int add(IdentifiableObject *val)
  {
    return this->add(0, val);
  }

  public: virtual Int add(Char const *name, IdentifiableObject *val);

  public: virtual void insert(Int index, IdentifiableObject *val)
  {
    this->insert(index, 0, val);
  }

  public: virtual void insert(Int index, Char const *name, IdentifiableObject *val);

  public: virtual void set(Int index, Char const *name, IdentifiableObject *val);

  public: virtual const SbStr& getName(Int index) const;

  /// @}

}; // class

} } // namespace

#endif
