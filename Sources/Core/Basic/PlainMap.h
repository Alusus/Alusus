/**
 * @file Core/Basic/PlainMap.h
 * Contains the header of class Core::Basic::PlainMap.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_PLAINMAP_H
#define CORE_BASIC_PLAINMAP_H

namespace Core { namespace Basic
{

template<class CTYPE, class PTYPE> class PlainMap : public PTYPE, public virtual MapContaining<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(PlainMap, PTYPE, "Core.Basic", "Core", "alusus.net", (CTYPE, PTYPE), (
    INHERITANCE_INTERFACES(MapContaining<CTYPE>)
  ));


  //============================================================================
  // Data Types

  /**
   * @brief A type of an entry in the list of elements.
   * This is a key/value pair. The keys are strings and the values must be
   * CTYPE inherited objects.
   */
  private: typedef std::pair<Str, CTYPE*> Entry;

  /// The type for the sorted index used to index the string key of the list.
  private: typedef DirectSortedIndex<Entry, Str, &Entry::first> Index;


  //============================================================================
  // Member Variables

  protected: PlainMap<CTYPE, PTYPE> *base;

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
  // Signals & Slots

  public: Signal<void, PlainMap<CTYPE, PTYPE>*> destroyNotifier;
  public: Signal<void, PlainMap<CTYPE, PTYPE>*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, PlainMap<CTYPE, PTYPE>*> baseDestroySlot = {
    this, &PlainMap<CTYPE, PTYPE>::onBaseDestroyed
  };
  private: Slot<void, PlainMap<CTYPE, PTYPE>*, ContentChangeOp, Int> baseChangeSlot = {
    this, &PlainMap<CTYPE, PTYPE>::onBaseContentChanged
  };


  //============================================================================
  // Constructors

  /**
   * @brief Create the index, if required.
   * If useIndex is true, a sorted index will be created to speed up
   * searching, otherwise the object will use sequential searching instead of
   * binary search.
   */
  public: PlainMap(Bool useIndex = false) : inherited(0), base(0)
  {
    if (useIndex) this->index = new Index(&this->list);
    else this->index = 0;
  }

  /// Initialize the map and create the index, if required.
  public: PlainMap(const std::initializer_list<Argument> &args, Bool useIndex = false)
    : inherited(0), base(0)
  {
    if (useIndex) this->index = new Index(&this->list);
    else this->index = 0;

    for (auto arg : args) this->add(arg.id, arg.ptr);
  }

  /// Delete the index created in the constructor, if any.
  public: virtual ~PlainMap()
  {
    this->destroyNotifier.emit(this);
    if (this->index != 0) delete this->index;
    if (this->base != 0) this->detachFromBase();
  }

  public: static SharedPtr<PlainMap<CTYPE, PTYPE>> create(
    Bool useIndex, const std::initializer_list<Argument> &args)
  {
    return std::make_shared<PlainMap<CTYPE, PTYPE>>(useIndex, args);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setBase(PlainMap<CTYPE, PTYPE> *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: PlainMap<CTYPE, PTYPE>* getBase() const
  {
    return this->base;
  }

  private: Word getBaseDefCount() const
  {
    if (this->base != 0) return this->base->getCount();
    else return 0;
  }

  private: void attachToBase(PlainMap<CTYPE, PTYPE> *b)
  {
    ASSERT(this->base == 0);
    ASSERT(this->inherited == 0);
    this->base = b;
    this->base->changeNotifier.connect(this->baseChangeSlot);
    this->base->destroyNotifier.connect(this->baseDestroySlot);
    this->inherited = new std::vector<Bool>(this->list.size(), false);
    this->inheritFromBase();
  }

  private: void detachFromBase()
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    this->removeInheritted();
    this->base->changeNotifier.disconnect(this->baseChangeSlot);
    this->base->destroyNotifier.disconnect(this->baseDestroySlot);
    this->base = 0;
    delete this->inherited;
    this->inherited = 0;
  }

  private: void inheritFromBase()
  {
    ASSERT(this->base != 0);
    for (Int i = 0; static_cast<Word>(i) < this->getBaseDefCount(); ++i) this->onAdded(i);
  }

  private: void removeInheritted()
  {
    ASSERT(this->inherited != 0);
    for (Int i = this->inherited->size()-1; i >= 0; --i) {
      if (this->inherited->at(i)) {
        this->onRemoved(i);
      }
    }
  }

  private: CTYPE* getFromBase(Int index)
  {
    return this->base->get(index);
  }

  private: SbStr const& getKeyFromBase(Int index)
  {
    return this->base->getKey(index);
  }

  private: void onAdded(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    Char const *key = this->getKeyFromBase(index).c_str();
    Int myIndex = this->findIndex(key);
    CTYPE *obj;
    if (myIndex != -1 && myIndex != index) {
      obj = this->get(myIndex);
      this->list.erase(this->list.begin()+myIndex);
      this->inherited->erase(this->inherited->begin()+myIndex);
      if (this->index != 0) this->index->remove(myIndex);
      this->changeNotifier.emit(this, ContentChangeOp::REMOVED, myIndex);
      this->list.insert(this->list.begin()+index, Entry(key, obj));
      this->inherited->insert(this->inherited->begin()+index, false);
    } else if (myIndex == -1) {
      obj = this->getFromBase(index);
      this->list.insert(this->list.begin()+index, Entry(key, obj));
      this->inherited->insert(this->inherited->begin()+index, true);
    }
    if (this->index != 0) this->index->add(index);
    this->changeNotifier.emit(this, ContentChangeOp::ADDED, index);
  }

  private: void onUpdated(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    if (this->inherited->at(index)) {
      this->list[index].second = this->getFromBase(index);
      this->changeNotifier.emit(this, ContentChangeOp::UPDATED, index);
    }
  }

  private: void onRemoved(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount()+1);
    if (this->inherited->at(index)) {
      this->list.erase(this->list.begin()+index);
      this->inherited->erase(this->inherited->begin()+index);
      if (this->index != 0) this->index->remove(index);
      this->changeNotifier.emit(this, ContentChangeOp::REMOVED, index);
    } else {
      Str key = this->getKey(index);
      CTYPE *obj = this->get(index);
      this->list.erase(this->list.begin()+index);
      this->inherited->erase(this->inherited->begin()+index);
      if (this->index != 0) this->index->remove(index);
      this->changeNotifier.emit(this, ContentChangeOp::REMOVED, index);
      this->add(key.c_str(), obj);
    }
  }

  private: void onBaseContentChanged(PlainMap<CTYPE, PTYPE> *obj, ContentChangeOp op, Int index)
  {
    if (op == ContentChangeOp::ADDED) this->onAdded(index);
    else if (op == ContentChangeOp::UPDATED) this->onUpdated(index);
    else if (op == ContentChangeOp::REMOVED) this->onRemoved(index);
  }

  private: void onBaseDestroyed(PlainMap<CTYPE, PTYPE> *obj)
  {
    this->detachFromBase();
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  public: Int add(Char const *key, CTYPE *val)
  {
    Int i = this->findIndex(key);
    if (i != -1) {
      if (this->inherited != 0 && this->inherited->at(i)) {
        this->list[i].second = val;
        this->inherited->at(i) = false;
        this->changeNotifier.emit(this, ContentChangeOp::UPDATED, i);
      } else {
        throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Already exists."), key);
      }
    } else {
      i = this->list.size();
      this->list.push_back(Entry(key, val));
      if (this->inherited != 0) this->inherited->push_back(false);
      if (this->index != 0) this->index->add();
      this->changeNotifier.emit(this, ContentChangeOp::ADDED, i);
    }
    return i;
  }

  public: void insert(Int index, Char const *key, CTYPE *val)
  {
    if (this->findIndex(key) != -1) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Already exists."), key);
    }
    if (static_cast<Word>(index) < this->getBaseDefCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"),
                      STR("Cannot insert at a position that breaks base's sequence."), index);
    }
    this->list.insert(this->list.begin()+index, Entry(key, val));
    if (this->inherited != 0) this->inherited->insert(this->inherited->begin()+index, false);
    if (this->index != 0) this->index->add(index);
    this->changeNotifier.emit(this, ContentChangeOp::ADDED, index);
  }

  public: Int set(Char const *key, CTYPE *val, Bool insertIfNew=true)
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      if (insertIfNew) {
        idx = this->list.size();
        this->list.push_back(Entry(key, val));
        if (this->inherited != 0) this->inherited->push_back(false);
        if (this->index != 0) this->index->add();
        this->changeNotifier.emit(this, ContentChangeOp::ADDED, idx);
      } else {
        throw EXCEPTION(InvalidArgumentException, STR("key"),STR("Not found."), key);
      }
    } else {
      this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, idx);
      this->list[idx].second = val;
      if (this->inherited != 0) this->inherited->at(idx) = false;
      this->changeNotifier.emit(this, ContentChangeOp::UPDATED, idx);
    }
    return idx;
  }

  public: void set(Int index, CTYPE *val)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, index);
    this->list[index].second = val;
    if (this->inherited != 0) this->inherited->at(index) = false;
    this->changeNotifier.emit(this, ContentChangeOp::UPDATED, index);
  }

  public: Int remove(Char const *key)
  {
    Int idx = this->findIndex(key);
    if (idx == -1 || (this->inherited != 0 && this->inherited->at(idx) == true)) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Not found."), key);
    }
    if (static_cast<Word>(idx) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, idx);
      this->list[idx].second = this->getFromBase(idx);
      this->inherited->at(idx) = true;
      this->changeNotifier.emit(this, ContentChangeOp::UPDATED, idx);
    } else {
      this->changeNotifier.emit(this, ContentChangeOp::WILL_REMOVE, idx);
      this->list.erase(this->list.begin()+idx);
      if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+idx);
      if (this->index != 0) this->index->remove(idx);
      this->changeNotifier.emit(this, ContentChangeOp::REMOVED, idx);
    }
    return idx;
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    if (this->inherited != 0 && this->inherited->at(index)) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Given entry belongs to base."), index);
    }
    if (static_cast<Word>(index) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, index);
      this->list[index].second = this->getFromBase(index);
      this->inherited->at(index) = true;
      this->changeNotifier.emit(this, ContentChangeOp::UPDATED, index);
    } else {
      this->changeNotifier.emit(this, ContentChangeOp::WILL_REMOVE, index);
      this->list.erase(this->list.begin()+index);
      if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+index);
      if (this->index != 0) this->index->remove(index);
      this->changeNotifier.emit(this, ContentChangeOp::REMOVED, index);
    }
  }

  public: Word getCount() const
  {
    return this->list.size();
  }

  public: CTYPE* get(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Not found in the map."), key);
    }
    return this->list[idx].second;
  }

  public: CTYPE* get(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    return this->list[index].second;
  }

  public: SbStr const& getKey(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    return this->list[index].first.sbstr();
  }

  public: Int getIndex(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Not found in the map."), key);
    }
    return idx;
  }

  public: Int findIndex(Char const *key) const
  {
    if (key == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Cannot be null."));
    }
    // Do we have an index to speed up search?
    if (this->index != 0) {
      return this->index->find(Str(key));
    } else {
      for (Word i = 0; i < this->list.size(); ++i) {
        if (this->list[i].first == key) return i;
      }
      return -1;
    }
  }

  public: void clear()
  {
    Int i = 0;
    while (static_cast<Word>(i) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      if (!this->inherited->at(i)) {
        this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, i);
        this->list[i].second = this->getFromBase(i);
        this->inherited->at(i) = true;
        this->changeNotifier.emit(this, ContentChangeOp::UPDATED, i);
      }
      ++i;
    }
    Int j = this->getCount() - 1;
    while (j >= i) {
      this->remove(j);
      --j;
    }
  }

  public: Bool isInherited(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    if (this->inherited == 0) return false;
    else return this->inherited->at(index);
  }

  /// @}

  /// @name Containing Implementation
  /// @{

  public: virtual void setElement(Int index, CTYPE *val)
  {
    this->set(index, val);
  }
  public: virtual Int setElement(Char const *key, CTYPE *val)
  {
    return this->set(key, val);
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

  public: virtual CTYPE* getElement(Int index) const
  {
    return this->get(index);
  }
  public: virtual CTYPE* getElement(Char const *key) const
  {
    return this->get(key);
  }

  public: virtual SbStr const& getElementKey(Int index) const
  {
    return this->getKey(index);
  }

  public: virtual Int findElementIndex(Char const *key) const
  {
    return this->findIndex(key);
  }

  /// @}

}; // class

} } // namespace

#endif
