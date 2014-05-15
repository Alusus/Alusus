/**
 * @file Core/Data/List.h
 * Contains the header of class Core::Data::List.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_LIST_H
#define DATA_LIST_H

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
class List : public IdentifiableObject, public virtual DataOwner, public virtual ListSharedContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(List, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(IdentifiableObject, DataOwner, ListSharedContainer);


  //============================================================================
  // Member Variables

  /// The vector in which the object pointers will be stored.
  private: std::vector<SharedPtr<IdentifiableObject>> list;


  //============================================================================
  // Constructors

  public: List()
  {
  }

  public: List(const std::initializer_list<SharedPtr<IdentifiableObject>> &args);

  public: static SharedPtr<List> create(const std::initializer_list<SharedPtr<IdentifiableObject>> &args)
  {
    return std::make_shared<List>(args);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Adds a list of new objects to the list.
   * This is more efficient than individually calling add() on each item
   * because it preallocates any needed memory in advance.
   */
  public: void add(const std::initializer_list<SharedPtr<IdentifiableObject>> &objs)
  {
    if (this->list.capacity() < this->list.size() + objs.size()) this->list.reserve(this->list.size() + objs.size());
    for (auto obj : objs) this->list.push_back(obj);
  }

  /**
   * @brief Clear the entire list.
   * This will not delete the contained objects, only removes them from the
   * list.
   */
  public: void clear()
  {
    this->list.clear();
  }

  public: void reserve(Int size)
  {
    this->list.reserve(size);
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);


  //============================================================================
  // ListSharedContainer Implementation

  /// Change the element at the specified index.
  public: virtual void set(Int index, const SharedPtr<IdentifiableObject> &val);

  /// Remove the element at the specified index.
  public: virtual void remove(Int index);

  /// Get the count of elements in the list.
  public: virtual Word getCount() const
  {
    return this->list.size();
  }

  /// Get the object at the specified index.
  public: virtual const SharedPtr<IdentifiableObject>& get(Int index) const;

  /// Add a new object to the list.
  public: virtual void add(const SharedPtr<IdentifiableObject> &val)
  {
    this->list.push_back(val);
  }

}; // class

} } // namespace

#endif
