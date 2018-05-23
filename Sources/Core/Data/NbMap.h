/**
 * @file Core/Data/NbMap.h
 * Contains the header of class Core::Data::NbMap.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NBMAP_H
#define CORE_DATA_NBMAP_H

namespace Core::Data
{

class NbMap : public Node, public virtual MapContaining<TiObject>, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(NbMap, Node, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(MapContaining<TiObject>, DataHaving)
  ));


  //============================================================================
  // Member Variables

  private: SharedMap<TiObject> map;
  private: NbMap *base = 0;


  //============================================================================
  // Signals and Slots

  public: Signal<void, NbMap*> destroyNotifier;
  public: Signal<void, NbMap*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, SharedMap<TiObject>*, ContentChangeOp, Int> contentChangeSlot = {
    [=](SharedMap<TiObject>* map, ContentChangeOp changeOp, Int index)->void
    {
      this->changeNotifier.emit(this, changeOp, index);
    }
  };


  //============================================================================
  // Constructors

  public: NbMap(Bool useIndex = false) : map(useIndex)
  {
    this->setupSignals();
  }

  IMPLEMENT_MAP_CONSTRUCTOR(NbMap, this->setupSignals());

  public: virtual ~NbMap()
  {
    this->destroyNotifier.emit(this);
    for (Int i = 0; i < this->getCount(); ++i) {
      if (!this->isInherited(i)) {
        DISOWN_SHAREDPTR(this->get(i));
      }
    }
  }

  public: static SharedPtr<NbMap> create(Bool useIndex = false)
  {
    return std::make_shared<NbMap>(useIndex);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void setupSignals()
  {
    this->map.changeNotifier.connect(this->contentChangeSlot);
  }

  /// @}

  /// @name Inheritance Functions
  /// @{

  public: void setBase(NbMap *base)
  {
    this->map.setBase(base == 0 ? 0 : base->getSharedMap());
    this->base = base;
  }

  public: NbMap* getBase() const
  {
    return this->base;
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  protected: SharedMap<TiObject>* getSharedMap()
  {
    return &this->map;
  }

  public: Int add(Char const *key, TioSharedPtr const &val)
  {
    auto index = this->map.add(key, val);
    OWN_SHAREDPTR(val);
    return index;
  }

  public: void insert(Int index, Char const *key, TioSharedPtr const &val)
  {
    this->map.insert(index, key, val);
    OWN_SHAREDPTR(val);
  }

  public: Int set(Char const *key, TioSharedPtr const &val, Bool insertIfNew=true)
  {
    Int index = this->map.findIndex(key);
    if (index != -1) {
      TiObject *old = this->map.getElement(index);
      DISOWN_PLAINPTR(old);
      this->map.set(index, val);
      OWN_SHAREDPTR(val);
    } else {
      index = this->map.set(key, val, insertIfNew);
      OWN_SHAREDPTR(val);
    }
    return index;
  }

  public: void set(Int index, SharedPtr<TiObject> const &val)
  {
    TiObject *old = this->map.getElement(index);
    DISOWN_PLAINPTR(old);
    this->map.set(index, val);
    OWN_SHAREDPTR(val);
  }

  public: Int remove(Char const *key)
  {
    Int index = this->map.findIndex(key);
    if (index != -1) {
      TiObject *old = this->map.getElement(index);
      DISOWN_PLAINPTR(old);
      this->map.remove(index);
      return index;
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Element not found."), key);
    }
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Index out of range."), index);
    }
    DISOWN_SHAREDPTR(this->get(index));
    this->map.remove(index);
  }

  public: Word getCount() const
  {
    return this->map.getCount();
  }

  public: TioSharedPtr const& get(Char const *key) const
  {
    return this->map.get(key);
  }

  public: TioSharedPtr const& get(Int index) const
  {
    return this->map.get(index);
  }

  public: SbStr const& getKey(Int index) const
  {
    return this->map.getKey(index);
  }

  public: Int getIndex(Char const *key) const
  {
    return this->map.getIndex(key);
  }

  public: Int findIndex(Char const *key) const
  {
    return this->map.findIndex(key);
  }

  public: void clear()
  {
    this->map.clear();
  }

  public: Bool isInherited(Int index) const
  {
    return this->map.isInherited(index);
  }

  /// @}

  /// @name MapContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val)
  {
    this->set(index, getSharedPtr(val));
  }
  public: virtual Int setElement(Char const *key, TiObject *val)
  {
    return this->set(key, getSharedPtr(val));
  }

  public: virtual void removeElement(Int index)
  {
    this->remove(index);
  }
  public: virtual Int removeElement(Char const *key)
  {
    return this->remove(key);
  }

  public: virtual Word getElementCount() const
  {
    return this->getCount();
  }

  public: virtual TiObject* getElement(Int index) const
  {
    return this->get(index).get();
  }
  public: virtual TiObject* getElement(Char const *key) const
  {
    return this->get(key).get();
  }

  public: virtual SbStr const& getElementKey(Int index) const
  {
    return this->getKey(index);
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findElementIndex(Char const *key) const
  {
    return this->findIndex(key);
  }

  /// @}

  /// @name DataHaving Implementation
  /// @{

  /// @sa DataHaving::unsetIndexes()void
  public: virtual void unsetIndexes(Int from, Int to)
  {
    for (Word i = 0; i < this->getCount(); ++i) {
      TiObject *obj = this->get(i).get();
      if (obj != 0) Data::unsetIndexes(obj, from, to);
    }
  }

  /// @}

}; // class

} // namespace

#endif
