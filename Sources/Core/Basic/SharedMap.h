/**
 * @file Core/Basic/SharedMap.h
 * Contains the header of class Core::Basic::SharedMap.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_MAP_H
#define CORE_BASIC_MAP_H

namespace Core { namespace Basic
{

template<class CTYPE, class PTYPE> class SharedMap : public PTYPE
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedMap, PTYPE, "Core.Data", "Core", "alusus.net", CTYPE, PTYPE);


  //============================================================================
  // Data Types

  /**
   * @brief A type of an entry in the list of elements.
   * This is a key/value pair. The keys are strings and the values must be
   * TiObject inherited objects.
   */
  private: typedef std::pair<Str, SharedPtr<TiObject>> Entry;

  /// The type for the sorted index used to index the string key of the list.
  private: typedef DirectSortedIndex<Entry, Str, &Entry::first> Index;

  s_enum(ChangeOp, ADD, UPDATE, REMOVE);


  //============================================================================
  // Member Variables

  private: SharedMap<CTYPE, PTYPE> *base;

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

  public: SIGNAL(destroyNotifier, (SharedMap<CTYPE, PTYPE> *obj), (obj));
  public: SIGNAL(changeNotifier, (SharedMap<CTYPE, PTYPE> *obj, ChangeOp op, Int index), (obj, op, index));


  //============================================================================
  // Constructors

  /**
   * @brief Create the index, if required.
   * If useIndex is true, a sorted index will be created to speed up
   * searching, otherwise the object will use sequential searching instead of
   * binary search.
   */
  public: SharedMap(Bool useIndex=false) : inherited(0), base(0)
  {
    if (useIndex) this->index = new Index(&this->list);
    else this->index = 0;
  }

  /// Initialize the map and create the index, if required.
  public: SharedMap(Bool useIndex, const std::initializer_list<Argument<Char const*>> &args)
  {
    if (useIndex) this->index = new Index(&this->list);
    else this->index = 0;

    for (auto arg : args) this->add(arg.id, arg.tiShared);
  }

  /// Delete the index created in the constructor, if any.
  public: virtual ~SharedMap()
  {
    if (this->index != 0) delete this->index;
    if (this->base != 0) this->detachFromBase();
    this->destroyNotifier.emit(this);
  }

  public: static SharedPtr<SharedMap<CTYPE, PTYPE>> create(
    Bool useIndex, const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<SharedMap<CTYPE, PTYPE>>(useIndex, args);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setBase(SharedMap<CTYPE, PTYPE> *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: SharedMap<CTYPE, PTYPE>* getBase() const
  {
    return this->base;
  }

  private: Word getBaseDefCount() const
  {
    if (this->base != 0) return this->base->getCount();
    else return 0;
  }

  private: void attachToBase(SharedMap<CTYPE, PTYPE> *b)
  {
    ASSERT(this->base == 0);
    ASSERT(this->inherited == 0);
    this->base = b;
    this->base->changeNotifier.connect(this, &SharedMap::onBaseContentChanged);
    this->base->destroyNotifier.connect(this, &SharedMap::onBaseDestroyed);
    this->inherited = new std::vector<Bool>(this->list.size(), false);
    this->inheritFromBase();
  }

  private: void detachFromBase()
  {
    ASSERT(this->base != 0);
    this->removeInheritted();
    this->base->changeNotifier.unconnect(this, &SharedMap::onBaseContentChanged);
    this->base->destroyNotifier.unconnect(this, &SharedMap::onBaseDestroyed);
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

  private: void onAdded(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    Char const *key = this->base->getKey(index).c_str();
    Int myIndex = this->findIndex(key);
    SharedPtr<CTYPE> obj;
    if (myIndex != -1 && myIndex != index) {
      obj = this->get(myIndex);
      this->list.erase(this->list.begin()+myIndex);
      this->inherited->erase(this->inherited->begin()+myIndex);
      if (this->index != 0) this->index->remove(myIndex);
      this->changeNotifier.emit(this, ChangeOp::REMOVE, myIndex);
      this->list.insert(this->list.begin()+index, Entry(key, obj));
      this->inherited->insert(this->inherited->begin()+index, false);
    } else if (myIndex == -1) {
      obj = this->base->get(index);
      this->list.insert(this->list.begin()+index, Entry(key, obj));
      this->inherited->insert(this->inherited->begin()+index, true);
    }
    if (this->index != 0) this->index->add(index);
    this->changeNotifier.emit(this, ChangeOp::ADD, index);
  }

  private: void onUpdated(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    if (this->inherited->at(index)) {
      this->list[index].second = this->base->get(index);
      this->changeNotifier.emit(this, ChangeOp::UPDATE, index);
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
      this->changeNotifier.emit(this, ChangeOp::REMOVE, index);
    } else {
      Str key = this->getKey(index);
      SharedPtr<CTYPE> obj = this->get(index);
      this->list.erase(this->list.begin()+index);
      this->inherited->erase(this->inherited->begin()+index);
      if (this->index != 0) this->index->remove(index);
      this->changeNotifier.emit(this, ChangeOp::REMOVE, index);
      this->add(key.c_str(), obj);
    }
  }

  private: void onBaseContentChanged(SharedMap<CTYPE, PTYPE> *obj, ChangeOp op, Int index)
  {
    if (op == ChangeOp::ADD) this->onAdded(index);
    else if (op == ChangeOp::UPDATE) this->onUpdated(index);
    else if (op == ChangeOp::REMOVE) this->onRemoved(index);
  }

  private: void onBaseDestroyed(SharedMap *obj)
  {
    this->detachFromBase();
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  public: Int add(Char const *key, SharedPtr<CTYPE> const &val)
  {
    Int i = this->findIndex(key);
    if (i != -1) {
      if (this->inherited != 0 && this->inherited->at(i)) {
        this->list[i].second = val;
        this->inherited->at(i) = false;
        this->changeNotifier.emit(this, ChangeOp::UPDATE, i);
      } else {
        throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Already exists."), key);
      }
    } else {
      i = this->list.size();
      this->list.push_back(Entry(key, val));
      if (this->inherited != 0) this->inherited->push_back(false);
      if (this->index != 0) this->index->add();
      this->changeNotifier.emit(this, ChangeOp::ADD, i);
    }
    return i;
  }

  public: void insert(Int index, Char const *key, SharedPtr<CTYPE> const &val)
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
    this->changeNotifier.emit(this, ChangeOp::ADD, index);
  }

  public: Int set(Char const *key, SharedPtr<CTYPE> const &val, Bool insertIfNew=true)
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      if (insertIfNew) {
        idx = this->list.size();
        this->list.push_back(Entry(key, val));
        if (this->inherited != 0) this->inherited->push_back(false);
        if (this->index != 0) this->index->add();
        this->changeNotifier.emit(this, ChangeOp::ADD, idx);
      } else {
        throw EXCEPTION(InvalidArgumentException, STR("key"),STR("Not found."), key);
      }
    } else {
      this->list[idx].second = val;
      if (this->inherited != 0) this->inherited->at(idx) = false;
      this->changeNotifier.emit(this, ChangeOp::UPDATE, idx);
    }
    return idx;
  }

  public: void set(Int index, SharedPtr<CTYPE> const &val)
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    this->list[index].second = val;
    if (this->inherited != 0) this->inherited->at(index) = false;
    this->changeNotifier.emit(this, ChangeOp::UPDATE, index);
  }

  public: void remove(Char const *key)
  {
    Int idx = this->findIndex(key);
    if (idx == -1 || (this->inherited != 0 && this->inherited->at(idx) == true)) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Not found."), key);
    }
    if (static_cast<Word>(idx) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->list[idx].second = this->base->get(idx);
      this->inherited->at(idx) = true;
      this->changeNotifier.emit(this, ChangeOp::UPDATE, idx);
    } else {
      this->list.erase(this->list.begin()+idx);
      if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+idx);
      if (this->index != 0) this->index->remove(idx);
      this->changeNotifier.emit(this, ChangeOp::REMOVE, idx);
    }
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
      this->list[index].second = this->base->get(index);
      this->inherited->at(index) = true;
      this->changeNotifier.emit(this, ChangeOp::UPDATE, index);
    } else {
      this->list.erase(this->list.begin()+index);
      if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+index);
      if (this->index != 0) this->index->remove(index);
      this->changeNotifier.emit(this, ChangeOp::REMOVE, index);
    }
  }

  public: Word getCount() const
  {
    return this->list.size();
  }

  public: SharedPtr<CTYPE> const& get(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Not found in the map."), key);
    }
    return this->list[idx].second;
  }

  public: SharedPtr<CTYPE> const& get(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    return this->list[index].second;
  }

  public: const SbStr& getKey(Int index) const
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
        this->list[i].second = this->base->get(i);
        this->inherited->at(i) = true;
        this->changeNotifier.emit(this, ChangeOp::UPDATE, i);
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

}; // class

} } // namespace

#endif
