/**
 * @file Core/Basic/SharedMapBase.h
 * Contains the header of class Core::Basic::SharedMapBase.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDMAPBASE_H
#define CORE_BASIC_SHAREDMAPBASE_H

namespace Core::Basic
{

template<class CTYPE, class PTYPE> class SharedMapBase : public PTYPE, public DynamicMapContaining<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedMapBase, PTYPE, "Core.Basic", "Core", "alusus.org", (CTYPE, PTYPE), (
    INHERITANCE_INTERFACES(DynamicMapContaining<CTYPE>)
  ));


  //============================================================================
  // Member Variables

  protected: SharedMapBase<CTYPE, PTYPE> *base;

  private: Map<Str, SharedPtr<CTYPE>> map;

  private: Array<Bool> *inherited;


  //============================================================================
  // Signals & Slots

  public: Signal<void, SharedMapBase<CTYPE, PTYPE>*> destroyNotifier;
  public: Signal<void, SharedMapBase<CTYPE, PTYPE>*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, SharedMapBase<CTYPE, PTYPE>*> baseDestroySlot = {
    this, &SharedMapBase<CTYPE, PTYPE>::onBaseDestroyed
  };
  private: Slot<void, SharedMapBase<CTYPE, PTYPE>*, ContentChangeOp, Int> baseChangeSlot = {
    this, &SharedMapBase<CTYPE, PTYPE>::onBaseContentChanged
  };


  //============================================================================
  // Constructors

  /**
   * @brief Create the index, if required.
   * If useIndex is true, a sorted index will be created to speed up
   * searching, otherwise the object will use sequential searching instead of
   * binary search.
   */
  protected: SharedMapBase(Bool useIndex = false) : map(useIndex), inherited(0), base(0)
  {
  }

  /// Delete the index created in the constructor, if any.
  public: virtual ~SharedMapBase() = 0;

  protected: void destruct()
  {
    this->destroyNotifier.emit(this);
    if (this->base != 0) this->detachFromBase();
    this->clear();
  }

  //============================================================================
  // Member Functions

  /// @name Abstract Functions
  /// @{

  protected: virtual SharedPtr<CTYPE> prepareForSet(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) = 0;

  protected: virtual void finalizeSet(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) = 0;

  protected: virtual void prepareForUnset(
    Char const *key, Int index, SharedPtr<CTYPE> const &obj, Bool inherited
  ) = 0;

  /// @}

  /// @name Notification Functions
  /// @{

  protected: virtual void onAdded(Int index)
  {
    this->changeNotifier.emit(this, ContentChangeOp::ADDED, index);
  }

  protected: virtual void onWillUpdate(Int index)
  {
    this->changeNotifier.emit(this, ContentChangeOp::WILL_UPDATE, index);
  }

  protected: virtual void onUpdated(Int index)
  {
    this->changeNotifier.emit(this, ContentChangeOp::UPDATED, index);
  }

  protected: virtual void onWillRemove(Int index)
  {
    this->changeNotifier.emit(this, ContentChangeOp::WILL_REMOVE, index);
  }

  protected: virtual void onRemoved(Int index)
  {
    this->changeNotifier.emit(this, ContentChangeOp::REMOVED, index);
  }

  /// @}

  /// @name Inheritance Functions
  /// @{

  protected: void setBase(SharedMapBase<CTYPE, PTYPE> *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: SharedMapBase<CTYPE, PTYPE>* getBase() const
  {
    return this->base;
  }

  private: Word getBaseDefCount() const
  {
    if (this->base != 0) return this->base->getCount();
    else return 0;
  }

  private: void attachToBase(SharedMapBase<CTYPE, PTYPE> *b)
  {
    ASSERT(this->base == 0);
    ASSERT(this->inherited == 0);
    this->base = b;
    this->base->changeNotifier.connect(this->baseChangeSlot);
    this->base->destroyNotifier.connect(this->baseDestroySlot);
    this->inherited = new Array<Bool>(this->map.getLength(), false);
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
    for (Int i = 0; static_cast<Word>(i) < this->getBaseDefCount(); ++i) this->onBaseElementAdded(i);
  }

  private: void removeInheritted()
  {
    ASSERT(this->inherited != 0);
    for (Int i = this->inherited->getLength()-1; i >= 0; --i) {
      if (this->inherited->at(i)) {
        this->onBaseElementRemoved(i);
      }
    }
  }

  private: SharedPtr<CTYPE> getFromBase(Int index)
  {
    return this->base->get(index);
  }

  private: SbStr const getKeyFromBase(Int index)
  {
    return this->base->getKey(index);
  }

  private: void onBaseElementAdded(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    Char const *key = this->getKeyFromBase(index);
    Int myIndex = this->findIndex(key);
    SharedPtr<CTYPE> obj;
    if (myIndex != -1 && myIndex != index) {
      obj = this->get(myIndex);
      this->prepareForUnset(key, myIndex, this->map.valAt(myIndex), this->inherited->at(myIndex));
      this->map.removeAt(myIndex);
      this->inherited->remove(myIndex);
      this->onRemoved(myIndex);
      obj = this->prepareForSet(key, index, obj, true, true);
      this->map.insert(index, key, obj);
      this->inherited->insert(index, false);
      this->finalizeSet(key, index, obj, true, true);
    } else if (myIndex == -1) {
      obj = this->getFromBase(index);
      obj = this->prepareForSet(key, index, obj, true, true);
      this->map.insert(index, key, obj);
      this->inherited->insert(index, true);
      this->finalizeSet(key, index, obj, true, true);
    }
    this->onAdded(index);
  }

  private: void onBaseElementUpdated(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    if (this->inherited->at(index)) {
      Char const *key = this->getKeyFromBase(index);
      this->prepareForUnset(key, index, this->map.valAt(index), true);
      auto obj = this->prepareForSet(key, index, this->getFromBase(index), true, false);
      this->map.valAt(index) = obj;
      this->finalizeSet(key, index, obj, true, false);
      this->onUpdated(index);
    }
  }

  private: void onBaseElementRemoved(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount()+1);
    if (this->inherited->at(index)) {
      this->prepareForUnset(this->map.keyAt(index), index, this->map.valAt(index), true);
      this->map.removeAt(index);
      this->inherited->remove(index);
      this->onRemoved(index);
    } else {
      auto key = this->getKey(index).getBuf();
      SharedPtr<CTYPE> obj = this->get(index);
      this->prepareForUnset(key, index, obj, false);
      this->map.removeAt(index);
      this->inherited->remove(index);
      this->onRemoved(index);
      this->add(key, obj);
    }
  }

  private: void onBaseContentChanged(SharedMapBase<CTYPE, PTYPE> *obj, ContentChangeOp op, Int index)
  {
    if (op == ContentChangeOp::ADDED) this->onBaseElementAdded(index);
    else if (op == ContentChangeOp::UPDATED) this->onBaseElementUpdated(index);
    else if (op == ContentChangeOp::REMOVED) this->onBaseElementRemoved(index);
  }

  private: void onBaseDestroyed(SharedMapBase<CTYPE, PTYPE> *obj)
  {
    this->detachFromBase();
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  public: void add(std::initializer_list<Argument> const &args)
  {
    for (auto arg : args) this->add(arg.id, arg.shared.ti_cast<CTYPE>());
  }

  public: Int add(Char const *key, SharedPtr<CTYPE> const &val)
  {
    Int i = this->findIndex(key);
    if (i != -1) {
      if (this->inherited != 0 && this->inherited->at(i)) {
        auto obj = this->prepareForSet(key, i, val, false, false);
        this->map.valAt(i) = obj;
        this->inherited->at(i) = false;
        this->finalizeSet(key, i, obj, false, false);
        this->onUpdated(i);
      } else {
        throw EXCEPTION(InvalidArgumentException, S("key"), S("Already exists."), key);
      }
    } else {
      i = this->map.getLength();
      auto obj = this->prepareForSet(key, i, val, false, true);
      this->map.set(key, obj);
      if (this->inherited != 0) this->inherited->add(false);
      this->finalizeSet(key, i, obj, false, true);
      this->onAdded(i);
    }
    return i;
  }

  public: void insert(Int index, Char const *key, SharedPtr<CTYPE> const &val)
  {
    if (this->findIndex(key) != -1) {
      throw EXCEPTION(InvalidArgumentException, S("key"), S("Already exists."), key);
    }
    if (static_cast<Word>(index) < this->getBaseDefCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"),
                      S("Cannot insert at a position that breaks base's sequence."), index);
    }
    auto obj = this->prepareForSet(key, index, val, false, true);
    this->map.insert(index, key, obj);
    if (this->inherited != 0) this->inherited->insert(index, false);
    this->finalizeSet(key, index, obj, false, true);
    this->onAdded(index);
  }

  public: Int set(Char const *key, SharedPtr<CTYPE> const &val, Bool insertIfNew=true)
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      if (insertIfNew) {
        idx = this->map.getLength();
        auto obj = this->prepareForSet(key, idx, val, false, true);
        this->map.set(key, obj);
        if (this->inherited != 0) this->inherited->add(false);
        this->finalizeSet(key, idx, obj, false, true);
        this->onAdded(idx);
      } else {
        throw EXCEPTION(InvalidArgumentException, S("key"),S("Not found."), key);
      }
    } else {
      this->onWillUpdate(idx);
      this->prepareForUnset(
        this->map.keyAt(idx), idx, this->map.valAt(idx), this->inherited && this->inherited->at(idx)
      );
      auto obj = this->prepareForSet(this->map.keyAt(idx), idx, val, false, false);
      this->map.valAt(idx) = obj;
      if (this->inherited != 0) this->inherited->at(idx) = false;
      this->finalizeSet(this->map.keyAt(idx), idx, obj, false, false);
      this->onUpdated(idx);
    }
    return idx;
  }

  public: void set(Int index, SharedPtr<CTYPE> const &val)
  {
    if (static_cast<Word>(index) >= this->map.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    this->onWillUpdate(index);
    this->prepareForUnset(
      this->map.keyAt(index), index, this->map.valAt(index), this->inherited && this->inherited->at(index)
    );
    auto obj = this->prepareForSet(this->map.keyAt(index), index, val, false, false);
    this->map.valAt(index) = obj;
    if (this->inherited != 0) this->inherited->at(index) = false;
    this->finalizeSet(this->map.keyAt(index), index, obj, false, false);
    this->onUpdated(index);
  }

  public: Int remove(Char const *key)
  {
    Int idx = this->findIndex(key);
    if (idx == -1 || (this->inherited != 0 && this->inherited->at(idx) == true)) {
      throw EXCEPTION(InvalidArgumentException, S("key"), S("Not found."), key);
    }
    if (static_cast<Word>(idx) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->onWillUpdate(idx);
      this->prepareForUnset(key, idx, this->map.valAt(idx), false);
      auto obj = this->prepareForSet(key, idx, this->getFromBase(idx), true, false);
      this->map.valAt(idx) = obj;
      this->inherited->at(idx) = true;
      this->finalizeSet(key, idx, obj, true, false);
      this->onUpdated(idx);
    } else {
      this->onWillRemove(idx);
      this->prepareForUnset(key, idx, this->map.valAt(idx), false);
      this->map.removeAt(idx);
      if (this->inherited != 0) this->inherited->remove(idx);
      this->onRemoved(idx);
    }
    return idx;
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->map.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    if (this->inherited != 0 && this->inherited->at(index)) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Given entry belongs to base."), index);
    }
    if (static_cast<Word>(index) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->onWillUpdate(index);
      this->prepareForUnset(this->map.keyAt(index), index, this->map.valAt(index), false);
      auto obj = this->prepareForSet(this->map.keyAt(index), index, this->getFromBase(index), true, false);
      this->map.valAt(index) = obj;
      this->inherited->at(index) = true;
      this->finalizeSet(this->map.keyAt(index), index, obj, true, false);
      this->onUpdated(index);
    } else {
      this->onWillRemove(index);
      this->prepareForUnset(this->map.keyAt(index), index, this->map.valAt(index), false);
      this->map.removeAt(index);
      if (this->inherited != 0) this->inherited->remove(index);
      this->onRemoved(index);
    }
  }

  public: Word getCount() const
  {
    return this->map.getLength();
  }

  public: SharedPtr<CTYPE> const& get(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw EXCEPTION(InvalidArgumentException, S("key"), S("Not found in the map."), key);
    }
    return this->map.valAt(idx);
  }

  public: SharedPtr<CTYPE> const& get(Int index) const
  {
    if (static_cast<Word>(index) >= this->map.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    return this->map.valAt(index);
  }

  public: SbStr const getKey(Int index) const
  {
    if (static_cast<Word>(index) >= this->map.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    return this->map.keyAt(index).sbstr();
  }

  public: Int getIndex(Char const *key) const
  {
    Int idx = this->findIndex(key);
    if (idx == -1) {
      throw EXCEPTION(InvalidArgumentException, S("key"), S("Not found in the map."), key);
    }
    return idx;
  }

  public: Array<Str> getKeys() const
  {
    return this->map.getKeys();
  }

  public: Array<SharedPtr<CTYPE>> getValues() const
  {
    return this->map.getValues();
  }

  public: Int findIndex(Char const *key) const
  {
    if (key == 0) {
      throw EXCEPTION(InvalidArgumentException, S("key"), S("Cannot be null."));
    }
    return this->map.findPos(Str(true, key));
  }

  public: void clear()
  {
    Int i = 0;
    while (static_cast<Word>(i) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      if (!this->inherited->at(i)) {
        this->onWillUpdate(i);
        this->prepareForUnset(this->map.keyAt(i), i, this->map.valAt(i), false);
        auto obj = this->prepareForSet(this->map.keyAt(i), i, this->getFromBase(i), true, false);
        this->map.valAt(i) = obj;
        this->inherited->at(i) = true;
        this->finalizeSet(this->map.keyAt(i), i, obj, true, false);
        this->onUpdated(i);
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
    if (static_cast<Word>(index) >= this->map.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    if (this->inherited == 0) return false;
    else return this->inherited->at(index);
  }

  /// @}

  /// @name Containing Implementation
  /// @{

  public: virtual Int addElement(Char const *key, CTYPE *val)
  {
    return this->add(key, getSharedPtr(val));
  }

  public: virtual void insertElement(Int index, Char const *key, CTYPE *val)
  {
    this->insert(index, key, getSharedPtr(val));
  }

  public: virtual void setElement(Int index, CTYPE *val)
  {
    this->set(index, getSharedPtr(val));
  }
  public: virtual Int setElement(Char const *key, CTYPE *val)
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

  public: virtual CTYPE* getElement(Int index) const
  {
    return this->get(index).get();
  }
  public: virtual CTYPE* getElement(Char const *key) const
  {
    return this->get(key).get();
  }

  public: virtual SbStr const getElementKey(Int index) const
  {
    return this->getKey(index);
  }

  public: virtual Int findElementIndex(Char const *key) const
  {
    return this->findIndex(key);
  }

  public: virtual TypeInfo const* getElementsNeededType() const
  {
    return CTYPE::getTypeInfo();
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    return HoldMode::SHARED_REF;
  }

  /// @}

}; // class

template<class CTYPE, class PTYPE> SharedMapBase<CTYPE, PTYPE>::~SharedMapBase()
{
}

} // namespace

#endif
