/**
 * @file Core/Data/SharedMap.h
 * Contains the header of class Core::Data::SharedMap.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MAP_H
#define DATA_MAP_H

namespace Core { namespace Data
{

// TODO: DOC

// TODO: Should we change this into a template class?

/**
 * @brief An identifiable object that holds an associative array.
 * @ingroup data_containers
 *
 * This IdentifiableObject derived class is used to hold a map of identifiable
 * objects. This class is used mainly to store data used by the grammar. This
 * class stores the pointers to the identifiable objects, rather than the
 * objects themselves. Destructing this object will not result in the deletion
 * of contained objects, the user must manually delete objects that are no
 * longer needed. This map allows the user to access elements by both their
 * key, which is a string, and by their index. This is to allow users to cache
 * the position of the elements for faster access.
 */
class SharedMap : public IdentifiableObject,
            public virtual Initializable, public virtual DataOwner, public virtual MapSharedContainer
{
  //============================================================================
  // Type Info

  TYPE_INFO(SharedMap, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_3(IdentifiableObject, Initializable, DataOwner, MapSharedContainer);


  //============================================================================
  // Data Types

  /**
   * @brief A type of an entry in the list of elements.
   * This is a key/value pair. The keys are strings and the values must be
   * IdentifiableObject inherited objects.
   */
  private: typedef std::pair<Str, SharedPtr<IdentifiableObject>> Entry;

  /// The type for the sorted index used to index the string key of the list.
  private: typedef DirectSortedIndex<Entry, Str, &Entry::first> Index;


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> parentReference;
  private: WeakPtr<SharedMap> parent;
  private: SharedMap *plainParent;

  /// The vector in which the list of key/value pairs are stored.
  private: std::vector<Entry> list;

  /**
   * @brief The index used to speed up searching within this map.
   * If this object is null, searching will be done sequentially without the
   * index. The decision whether to use an index or not is done through params
   * passed to the constructor.
   */
  private: Index *index;

  private: std::vector<Bool> *inherited;


  //============================================================================
  // Signals

  public: SIGNAL(destroyNotifier, (SharedMap *obj), (obj));


  //============================================================================
  // Constructors

  /**
   * @brief Create the index, if required.
   * If useIndex is true, a sorted index will be created to speed up
   * searching, otherwise the object will use sequential searching instead of
   * binary search.
   */
  public: SharedMap(Bool useIndex=false) : inherited(0), plainParent(0)
  {
    if (useIndex) this->index = new Index(&this->list);
    else this->index = 0;
  }

  /// Initialize the map and create the index, if required.
  public: SharedMap(Bool useIndex, const std::initializer_list<Argument<Char const*>> &args);

  /// Delete the index created in the constructor, if any.
  public: virtual ~SharedMap()
  {
    if (this->index != 0) delete this->index;
    if (this->plainParent != 0) this->detachFromParent();
    this->destroyNotifier.emit(this);
  }

  public: static SharedPtr<SharedMap> create(Bool useIndex, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<SharedMap>(useIndex, args);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setParentReference(SharedPtr<Reference> const &p)
  {
    this->parentReference = p;
  }

  public: SharedPtr<Reference> const& getParentReference() const
  {
    return this->parentReference;
  }

  public: void setParent(SharedPtr<SharedMap> const &p)
  {
    this->setParent(p.get());
    this->parent = p;
  }

  public: void setParent(SharedMap *p)
  {
    if (this->plainParent != 0) this->detachFromParent();
    if (p != 0) this->attachToParent(p);
  }

  public: SharedPtr<SharedMap> getParent() const
  {
    return this->parent.lock();
  }

  public: SharedMap* getPlainParent() const
  {
    return this->plainParent;
  }

  private: Word getParentDefCount() const
  {
    if (this->plainParent != 0) return this->plainParent->getCount();
    else return 0;
  }

  private: void attachToParent(SharedMap *p);

  private: void detachFromParent();

  private: void inheritFromParent();

  private: void removeInheritted();

  private: void onParentContentChanged(SharedContainer *obj, ContentChangeOp op, Int index);

  private: void onAdded(Int index);

  private: void onUpdated(Int index);

  private: void onRemoved(Int index);

  private: void onParentDestroyed(SharedMap *obj)
  {
    this->detachFromParent();
  }

  /// @}

  /// @name Definitions Access Functions
  /// @{

  public: Int add(Char const *key, SharedPtr<IdentifiableObject> const &val);

  public: void insert(Int index, Char const *key, SharedPtr<IdentifiableObject> const &val);

  public: Int set(Char const *key, SharedPtr<IdentifiableObject> const &val, Bool insertIfNew);

  public: void clear();

  public: Bool isInherited(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::isInherited"),
                                     STR("Out of range."));
    }
    if (this->inherited == 0) return false;
    else return this->inherited->at(index);
  }

  /**
   * @brief Get the index of a specified key.
   * If the key is not found an exception will be raised. If you want to check
   * for the existance of a key use findIndex instead.
   * @sa findIndex()
   */
  public: Int getIndex(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::getIndex"),
                                     STR("Not found in the map."), key);
    }
    return idx;
  }

  /// @}


  //============================================================================
  // Initializable Implementation

  /// @name Initializable Implementation
  /// @{

  public: virtual void initialize(IdentifiableObject *owner);

  /// @}


  //============================================================================
  // DataOwner Implementation

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}


  //============================================================================
  // MapSharedContainer Implementation

  /// @name MapSharedContainer Implementation
  /// @{

  /// Change the value at the specified index.
  public: virtual void set(Int index, SharedPtr<IdentifiableObject> const &val);

  /// Remove the element at the specified index.
  public: virtual void remove(Int index);

  /// Get the count of elements in the map.
  public: virtual Word getCount() const
  {
    return this->list.size();
  }

  /// Get the value (object) at a specified index.
  public: virtual SharedPtr<IdentifiableObject> const& get(Int index) const;

  public: virtual Int set(Char const *key, SharedPtr<IdentifiableObject> const &val)
  {
    return this->set(key, val, true);
  }

  public: virtual void remove(Char const *key);

  public: virtual SharedPtr<IdentifiableObject> const& get(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::get"),
                                     STR("Not found in the map."));
    }
    return this->list[idx].second;
  }

  public: virtual const SbStr& getKey(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::getKey"),
                                     STR("Out of range."), index);
    }
    return this->list[index].first.sbstr();
  }

  public: virtual Int findIndex(Char const *key) const;

  /// @}

}; // class

} } // namespace

#endif
