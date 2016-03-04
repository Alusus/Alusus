/**
 * @file Core/Data/SharedList.h
 * Contains the header of class Core::Data::SharedList.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SHAREDLIST_H
#define DATA_SHAREDLIST_H

namespace Core { namespace Data
{

// TODO: Doc

// TODO: Should we change this into a template class?

/**
 * @brief An identifiable object that holds a list.
 * @ingroup data_containers
 *
 * This IdentifiableObject derived class is used to hold a list of identifiable
 * objects. This class is used mainly to store data used by the grammar. This
 * class stores the pointers to the identifiable objects, rather than the
 * objects themselves. Destructing this object will not result in the deletion
 * of contained objects, the user must manually delete objects that are no
 * longer needed.
 */
class SharedList : public Node, public virtual DataOwner, public virtual ListContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedList, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(Node, DataOwner, ListContainer);


  //============================================================================
  // Member Variables

  /// The vector in which the object pointers will be stored.
  private: std::vector<SharedPtr<IdentifiableObject>> list;


  //============================================================================
  // Constructors

  public: SharedList()
  {
  }

  public: SharedList(const std::initializer_list<SharedPtr<IdentifiableObject>> &args);

  public: virtual ~SharedList();

  public: static SharedPtr<SharedList> create(const std::initializer_list<SharedPtr<IdentifiableObject>> &args)
  {
    return std::make_shared<SharedList>(args);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Adds a list of new objects to the list.
   * This is more efficient than individually calling add() on each item
   * because it preallocates any needed memory in advance.
   */
  public: void add(const std::initializer_list<SharedPtr<IdentifiableObject>> &objs);

  /// Add a new object to the list.
  public: Int add(SharedPtr<IdentifiableObject> const &val);

  /// Insert an element at a specific index.
  public: void insert(Int index, SharedPtr<IdentifiableObject> const &val);

  /// Change the element at the specified index.
  public: void set(Int index, SharedPtr<IdentifiableObject> const &val);

  /// Get the object at the specified index.
  public: SharedPtr<IdentifiableObject> const& getShared(Int index) const;

  /**
   * @brief Clear the entire list.
   * This will not delete the contained objects, only removes them from the
   * list.
   */
  public: void clear();

  public: void reserve(Int size)
  {
    this->list.reserve(size);
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()void
  public: virtual void unsetIndexes(Int from, Int to);


  //============================================================================
  // ListContainer Implementation

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
    return this->list.size();
  }

  /// Get the object at the specified index.
  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->getShared(index).get();
  }

  /// Add a new object to the list.
  public: virtual Int add(IdentifiableObject *val)
  {
    return this->add(getSharedPtr(val, true));
  }

  public: virtual void insert(Int index, IdentifiableObject *val)
  {
    this->insert(index, getSharedPtr(val, true));
  }

}; // class

} } // namespace

#endif
