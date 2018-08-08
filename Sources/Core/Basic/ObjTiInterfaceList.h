/**
 * @file Core/Basic/ObjTiInterfaceList.h
 * Contains the header of class Core::Basic::ObjTiInterfaceList.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_OBJTIINTERFACELIST_H
#define CORE_BASIC_OBJTIINTERFACELIST_H

namespace Core::Basic
{

class ObjTiInterfaceList : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(ObjTiInterfaceList, TiObject, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: TiObject *obj;
  private: SharedList<ObjTiInterface> list;
  private: std::vector<Bool> *inherited = 0;
  private: ObjTiInterfaceList *base = 0;


  //============================================================================
  // Signals and Slots

  public: Signal<void, ObjTiInterfaceList*> destroyNotifier;
  public: Signal<void, ObjTiInterfaceList*, ContentChangeOp, Int> changeNotifier;

  private: Slot<void, SharedList<ObjTiInterface>*, ContentChangeOp, Int> contentChangeSlot = {
    [=](SharedList<ObjTiInterface>* l, ContentChangeOp changeOp, Int index)->void
    {
      this->changeNotifier.emit(this, changeOp, index);
    }
  };

  private: Slot<void, ObjTiInterfaceList*> baseDestroySlot = {
    this, &ObjTiInterfaceList::onBaseDestroyed
  };
  private: Slot<void, ObjTiInterfaceList*, ContentChangeOp, Int> baseChangeSlot = {
    this, &ObjTiInterfaceList::onBaseContentChanged
  };


  //============================================================================
  // Constructors & Destructor

  public: ObjTiInterfaceList(TiObject *o) : obj(o)
  {
    this->list.changeNotifier.connect(this->contentChangeSlot);
  }

  public: ObjTiInterfaceList(TiObject *o, std::initializer_list<SharedPtr<ObjTiInterface>> const &args)
    : obj(o), list(args)
  {
    this->list.changeNotifier.connect(this->contentChangeSlot);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setBase(ObjTiInterfaceList *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: ObjTiInterfaceList* getBase() const
  {
    return this->base;
  }

  private: Word getBaseDefCount() const
  {
    return this->base != 0 ? this->base->getCount() : 0;
  }

  private: void attachToBase(ObjTiInterfaceList *b)
  {
    ASSERT(this->base == 0);
    ASSERT(this->inherited == 0);
    this->base = b;
    this->base->changeNotifier.connect(this->baseChangeSlot);
    this->base->destroyNotifier.connect(this->baseDestroySlot);
    this->inherited = new std::vector<Bool>(this->list.getCount(), false);
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

  private: SharedPtr<ObjTiInterface> getFromBase(Int index)
  {
    auto typeInfo = this->base->get(index)->getMyObjInterfaceInfo();
    return typeInfo->getFactory()->createShared(this->obj);
  }

  private: void onAdded(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    this->inherited->insert(this->inherited->begin()+index, true);
    this->list.insert(index, this->getFromBase(index));
  }

  private: void onUpdated(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount());
    if (this->inherited->at(index)) {
      this->list.set(index, this->getFromBase(index));
    }
  }

  private: void onRemoved(Int index)
  {
    ASSERT(this->base != 0);
    ASSERT(this->inherited != 0);
    ASSERT(static_cast<Word>(index) < this->getBaseDefCount()+1);
    if (this->inherited->at(index)) {
      this->inherited->erase(this->inherited->begin()+index);
      this->list.remove(index);
    } else {
      SharedPtr<ObjTiInterface> obj = this->list.get(index);
      this->inherited->erase(this->inherited->begin()+index);
      this->list.remove(index);
      this->list.insert(this->getBaseDefCount(), obj);
    }
  }

  private: void onBaseContentChanged(ObjTiInterfaceList *obj, ContentChangeOp op, Int index)
  {
    if (op == ContentChangeOp::ADDED) this->onAdded(index);
    else if (op == ContentChangeOp::UPDATED) this->onUpdated(index);
    else if (op == ContentChangeOp::REMOVED) this->onRemoved(index);
  }

  private: void onBaseDestroyed(ObjTiInterfaceList *obj)
  {
    this->detachFromBase();
  }

  /// @}

  /// @name Interface Access Functions
  /// @{

  public: Int add(SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    return this->list.add(val);
  }

  public: void insert(Int index, SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    this->list.insert(index, val);
  }

  public: void set(Int index, SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    this->list.set(index, val);
  }

  public: void remove(Int index)
  {
    this->list.remove(index);
  }

  public: void remove(TypeInfo *ti)
  {
    for (Int i = this->list.getCount() - 1; i >= 0; --i) {
      ObjTiInterface *pi = this->list.get(i).get();
      if (!this->isInherited(i) && pi->isInterfaceA(ti)) {
        this->remove(i);
      }
    }
  }

  public: template<class T> void remove()
  {
    this->remove(T::getTypeInfo());
  }

  public: SharedPtr<ObjTiInterface> const& get(Int index) const
  {
    return this->list.get(index);
  }

  public: ObjTiInterface* get(TypeInfo *ti) const
  {
    for (Int i = this->getCount() - 1; i >= 0; --i) {
      ObjTiInterface *pi = this->get(i).get();
      if (pi->isInterfaceDerivedFrom(ti)) return pi;
    }
    return 0;
  }

  public: template<class T> T* get() const
  {
    return static_cast<T*>(this->get(T::getTypeInfo()));
  }

  public: Word getCount() const
  {
    return this->list.getCount();
  }

  public: void clear()
  {
    Int i = 0;
    while (static_cast<Word>(i) < this->getBaseDefCount()) {
      ASSERT(this->base != 0);
      ASSERT(this->inherited != 0);
      if (!this->inherited->at(i)) {
        this->inherited->at(i) = true;
        this->list.set(i, this->getFromBase(i));
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
    if (static_cast<Word>(index) >= this->list.getCount()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    if (this->inherited == 0) return false;
    else return this->inherited->at(index);
  }

  /// @}

}; // class

} // namespace

#endif
