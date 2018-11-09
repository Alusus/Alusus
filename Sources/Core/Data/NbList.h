/**
 * @file Core/Data/NbList.h
 * Contains the header of class Core::Data::NbList.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NBLIST_H
#define CORE_DATA_NBLIST_H

namespace Core::Data
{

class NbList : public Node, public virtual DynamicContaining<TiObject>, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(NbList, Node, "Core.Data", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicContaining<TiObject>, DataHaving)
  ));


  //============================================================================
  // Member Variables

  private: SharedList<TiObject> list;
  private: NbList *base = 0;


  //============================================================================
  // Signals and Slots

  public: Signal<void, NbList*> destroyNotifier;
  public: Signal<void, NbList*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, SharedList<TiObject>*, ContentChangeOp, Int> contentChangeSlot = {
    [=](SharedList<TiObject>* l, ContentChangeOp changeOp, Int index)->void
    {
      this->changeNotifier.emit(this, changeOp, index);
    }
  };


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(NbList, this->setupSignals());

  IMPLEMENT_LIST_CONSTRUCTOR(NbList, this->setupSignals());

  public: virtual ~NbList()
  {
    this->destroyNotifier.emit(this);
    for (Int i = 0; i < this->getCount(); ++i) {
      if (!this->isInherited(i)) {
        DISOWN_SHAREDPTR(this->get(i));
      }
    }
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void setupSignals()
  {
    this->list.changeNotifier.connect(this->contentChangeSlot);
  }

  /// @}

  /// @name Inheritance Functions
  /// @{

  public: void setBase(NbList *base)
  {
    this->list.setBase(base == 0 ? 0 : base->getSharedList());
    this->base = base;
  }

  public: NbList* getBase() const
  {
    return this->base;
  }

  /// @}

  /// @name Data Access Functions
  /// @{

  protected: SharedList<TiObject>* getSharedList()
  {
    return &this->list;
  }

  public: void add(const std::initializer_list<TioSharedPtr> &objs)
  {
    this->list.add(objs);
    for (auto obj : objs) {
      OWN_SHAREDPTR(obj);
    }
  }

  public: Int add(TioSharedPtr const &val)
  {
    this->list.add(val);
    OWN_SHAREDPTR(val);
    return this->list.getCount() - 1;
  }

  public: void insert(Int index, TioSharedPtr const &val)
  {
    this->list.insert(index, val);
    OWN_SHAREDPTR(val);
  }

  public: void set(Int index, TioSharedPtr const &val)
  {
    if (static_cast<Word>(index) >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index out of range."), index);
    }
    auto old = this->get(index);
    this->list.set(index, val);
    DISOWN_SHAREDPTR(this->get(index));
    OWN_SHAREDPTR(val);
  }

  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Index out of range."), index);
    }
    DISOWN_SHAREDPTR(this->get(index));
    this->list.remove(index);
  }

  public: Word getCount() const
  {
    return this->list.getCount();
  }

  public: TioSharedPtr const& get(Int index) const
  {
    return this->list.get(index);
  }

  public: void clear()
  {
    this->list.clear();
  }

  public: Bool isInherited(Int index) const
  {
    return this->list.isInherited(index);
  }

  public: void reserve(Int size)
  {
    this->list.reserve(size);
  }

  /// @}

  /// @name DynamicContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val)
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

  public: virtual TiObject* getElement(Int index) const
  {
    return this->get(index).get();
  }

  public: virtual Int addElement(TiObject *val)
  {
    return this->add(getSharedPtr(val));
  }

  public: virtual void insertElement(Int index, TiObject *val)
  {
    this->insert(index, getSharedPtr(val));
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

  public: virtual TypeInfo* getElementsNeededType() const
  {
    return TiObject::getTypeInfo();
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    return HoldMode::SHARED_REF;
  }

  /// @}

}; // class

} // namespace

#endif
