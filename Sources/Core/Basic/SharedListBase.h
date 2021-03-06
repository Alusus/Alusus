/**
 * @file Core/Basic/SharedListBase.h
 * Contains the header of class Core::Basic::SharedListBase.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDLISTBASE_H
#define CORE_BASIC_SHAREDLISTBASE_H

namespace Core::Basic
{

template<class CTYPE, class PTYPE> class SharedListBase : public PTYPE, public DynamicContaining<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedListBase, PTYPE, "Core.Basic", "Core", "alusus.org", (CTYPE, PTYPE), (
    INHERITANCE_INTERFACES(DynamicContaining<CTYPE>)
  ));


  //============================================================================
  // Member Variables

  protected: SharedListBase<CTYPE, PTYPE> *base;

  /// The array in which the object pointers will be stored.
  private: Array<SharedPtr<CTYPE>> list;

  private: Array<Bool> *inherited;


  //============================================================================
  // Signals & Slots

  public: Signal<void, SharedListBase<CTYPE, PTYPE>*> destroyNotifier;
  public: Signal<void, SharedListBase<CTYPE, PTYPE>*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, SharedListBase<CTYPE, PTYPE>*> baseDestroySlot = {
    this, &SharedListBase<CTYPE, PTYPE>::onBaseDestroyed
  };
  private: Slot<void, SharedListBase<CTYPE, PTYPE>*, ContentChangeOp, Int> baseChangeSlot = {
    this, &SharedListBase<CTYPE, PTYPE>::onBaseContentChanged
  };


  //============================================================================
  // Constructors

  protected: SharedListBase() : inherited(0), base(0)
  {
  }

  public: virtual ~SharedListBase() = 0;

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
    Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) = 0;

  protected: virtual void finalizeSet(
    Int index, SharedPtr<CTYPE> const &obj, Bool inherited, Bool newEntry
  ) = 0;

  protected: virtual void prepareForUnset(
    Int index, SharedPtr<CTYPE> const &obj, Bool inherited
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

  protected: void setBase(SharedListBase<CTYPE, PTYPE> *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: SharedListBase<CTYPE, PTYPE>* getBase() const
  {
    return this->base;
  }

  private: Word getBaseDefCount() const
  {
    return this->base != 0 ? this->base->getCount() : 0;
  }

  private: void attachToBase(SharedListBase<CTYPE, PTYPE> *b)
  {
    ASSERT(this->base == 0);
    ASSERT(this->inherited == 0);
    this->base = b;
    this->base->changeNotifier.connect(this->baseChangeSlot);
    this->base->destroyNotifier.connect(this->baseDestroySlot);
    this->inherited = new Array<Bool>(this->list.getLength(), false);
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

  private: void onBaseElementAdded(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    auto obj = this->getFromBase(index);
    obj = this->prepareForSet(index, obj, true, true);
    this->list.insert(index, obj);
    this->inherited->insert(index, true);
    this->finalizeSet(index, obj, true, true);
    this->onAdded(index);
  }

  private: void onBaseElementUpdated(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    if (this->inherited->at(index)) {
      this->prepareForUnset(index, this->list(index), true);
      auto obj = this->prepareForSet(index, this->getFromBase(index), true, false);
      this->list(index) = obj;
      this->finalizeSet(index, obj, true, false);
      this->onUpdated(index);
    }
  }

  private: void onBaseElementRemoved(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount()+1);
    if (this->inherited->at(index)) {
      this->prepareForUnset(index, this->list(index), true);
      this->list.remove(index);
      this->inherited->remove(index);
      this->onRemoved(index);
    } else {
      SharedPtr<CTYPE> obj = this->get(index);
      this->prepareForUnset(index, this->list(index), false);
      this->list.remove(index);
      this->inherited->remove(index);
      this->onRemoved(index);
      this->insert(this->getBaseDefCount(), obj);
    }
  }

  private: void onBaseContentChanged(SharedListBase<CTYPE, PTYPE> *obj, ContentChangeOp op, Int index)
  {
    if (op == ContentChangeOp::ADDED) this->onBaseElementAdded(index);
    else if (op == ContentChangeOp::UPDATED) this->onBaseElementUpdated(index);
    else if (op == ContentChangeOp::REMOVED) this->onBaseElementRemoved(index);
  }

  private: void onBaseDestroyed(SharedListBase<CTYPE, PTYPE> *obj)
  {
    this->detachFromBase();
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  /**
   * @brief Adds a list of new objects to the list.
   * This is more efficient than individually calling add() on each item
   * because it preallocates any needed memory in advance.
   */
  public: void add(const std::initializer_list<SharedPtr<CTYPE>> &objs)
  {
    if (this->list.getBufSize() < this->list.getLength() + objs.size()) {
      this->list.reserve(this->list.getLength() + objs.size());
    }
    for (auto obj : objs) this->add(obj);
  }

  public: Int add(SharedPtr<CTYPE> const &val)
  {
    auto obj = this->prepareForSet(this->list.getLength(), val, false, true);
    this->list.add(obj);
    if (this->inherited != 0) this->inherited->add(false);
    this->finalizeSet(this->list.getLength() - 1, obj, false, true);
    this->onAdded(this->list.getLength() - 1);
    return this->list.getLength() - 1;
  }

  public: void insert(Int index, SharedPtr<CTYPE> const &val)
  {
    if (static_cast<Word>(index) > this->list.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
    }
    auto obj = this->prepareForSet(index, val, false, true);
    this->list.insert(index, obj);
    if (this->inherited != 0) this->inherited->insert(index, false);
    this->finalizeSet(index, obj, false, true);
    this->onAdded(index);
  }

  public: void set(Int index, SharedPtr<CTYPE> const &val)
  {
    if (index == this->getCount()) {
      this->add(val);
      return;
    }
    if (static_cast<Word>(index) >= this->list.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index out of range."), index);
    }
    this->onWillUpdate(index);
    this->prepareForUnset(index, this->list(index), this->inherited && this->inherited->at(index));
    auto obj = this->prepareForSet(index, val, false, false);
    this->list(index) = obj;
    if (this->inherited != 0) this->inherited->at(index) = false;
    this->finalizeSet(index, obj, false, false);
    this->onUpdated(index);
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->list.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index out of range."), index);
    }
    if (this->inherited != 0 && this->inherited->at(index)) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Given entry belongs to base."), index);
    }
    if (static_cast<Word>(index) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      this->onWillUpdate(index);
      this->prepareForUnset(index, this->list(index), false);
      auto obj = this->prepareForSet(index, this->getFromBase(index), true, false);
      this->list(index) = obj;
      this->inherited->at(index) = true;
      this->finalizeSet(index, obj, true, false);
      this->onUpdated(index);
    } else {
      this->onWillRemove(index);
      this->prepareForUnset(index, this->list(index), false);
      this->list.remove(index);
      if (this->inherited != 0) this->inherited->remove(index);
      this->onRemoved(index);
    }
  }

  public: Word getCount() const
  {
    return this->list.getLength();
  }

  public: SharedPtr<CTYPE> const& get(Int index) const
  {
    if (static_cast<Word>(index) >= this->list.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index out of range."), index);
    }
    return this->list(index);
  }

  public: void clear()
  {
    Int i = 0;
    while (static_cast<Word>(i) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      if (!this->inherited->at(i)) {
        this->onWillUpdate(i);
        this->prepareForUnset(i, this->list(i), false);
        auto obj = this->prepareForSet(i, this->getFromBase(i), true, false);
        this->list(i) = obj;
        this->inherited->at(i) = true;
        this->finalizeSet(i, obj, true, false);
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
    if (static_cast<Word>(index) >= this->list.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    if (this->inherited == 0) return false;
    else return this->inherited->at(index);
  }

  public: void reserve(Int size)
  {
    this->list.reserve(size);
  }

  /// @}

  /// @name Containing Implementation
  /// @{

  public: virtual void setElement(Int index, CTYPE *val)
  {
    this->set(index, getSharedPtr(val));
  }

  public: virtual void removeElement(Int index)
  {
    this->remove(index);
  }

  public: virtual Word getElementCount() const
  {
    return this->getCount();
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    return this->get(index).get();
  }

  public: virtual Int addElement(CTYPE *val)
  {
    return this->add(getSharedPtr(val));
  }

  public: virtual void insertElement(Int index, CTYPE *val)
  {
    this->insert(index, getSharedPtr(val));
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

template<class CTYPE, class PTYPE> SharedListBase<CTYPE, PTYPE>::~SharedListBase()
{
}

} // namespace

#endif
