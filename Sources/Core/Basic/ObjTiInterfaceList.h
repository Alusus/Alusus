/**
 * @file Core/Basic/ObjTiInterfaceList.h
 * Contains the header of class Core::Basic::ObjTiInterfaceList.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_OBJTIINTERFACELIST_H
#define CORE_BASIC_OBJTIINTERFACELIST_H

namespace Core::Basic
{

class ObjTiInterfaceList : public SharedListBase<ObjTiInterface, TiObject>
{
  //============================================================================
  // Type Info

  typedef SharedListBase<ObjTiInterface, TiObject> _MyBase;
  TYPE_INFO(ObjTiInterfaceList, _MyBase, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiObject *obj;


  //============================================================================
  // Constructors & Destructor

  public: ObjTiInterfaceList(TiObject *o) : obj(o)
  {
  }

  public: ObjTiInterfaceList(TiObject *o, std::initializer_list<SharedPtr<ObjTiInterface>> const &args)
    : obj(o)
  {
    for (auto arg : args) this->add(arg);
  }

  public: virtual ~ObjTiInterfaceList()
  {
    this->destruct();
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<ObjTiInterface> prepareForSet(
    Int index, SharedPtr<ObjTiInterface> const &obj, Bool inherited, Bool newEntry
  ) {
    if (inherited && obj != 0) {
      auto typeInfo = obj->getMyObjInterfaceInfo();
      return typeInfo->getFactory()->createShared(this->obj);
    } else {
      return obj;
    }
  }

  protected: virtual void finalizeSet(
    Int index, SharedPtr<ObjTiInterface> const &obj, Bool inherited, Bool newEntry
  ) {
  }

  protected: virtual void prepareForUnset(
    Int index, SharedPtr<ObjTiInterface> const &obj, Bool inherited
  ) {
  }

  /// @}

  /// @name Inheritted Functions
  /// @{

  public: void setBase(ObjTiInterfaceList *b)
  {
    SharedListBase<ObjTiInterface, TiObject>::setBase(b);
  }

  public: ObjTiInterfaceList* getBase() const
  {
    return static_cast<ObjTiInterfaceList*>(this->base);
  }

  /// @}

  /// @name Interface Access Functions
  /// @{

  public: Int add(SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    return SharedListBase<ObjTiInterface, TiObject>::add(val);
  }

  public: void insert(Int index, SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    SharedListBase<ObjTiInterface, TiObject>::insert(index, val);
  }

  public: void set(Int index, SharedPtr<ObjTiInterface> const &val)
  {
    this->remove(val->getMyInterfaceInfo());
    SharedListBase<ObjTiInterface, TiObject>::set(index, val);
  }

  public: using SharedListBase<ObjTiInterface, TiObject>::remove;

  public: void remove(TypeInfo *ti)
  {
    for (Int i = this->getCount() - 1; i >= 0; --i) {
      ObjTiInterface *pi = this->get(i).get();
      if (!this->isInherited(i) && pi->isInterfaceA(ti)) {
        this->remove(i);
      }
    }
  }

  public: template<class T> void remove()
  {
    this->remove(T::getTypeInfo());
  }

  public: using SharedListBase<ObjTiInterface, TiObject>::get;

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

  /// @}

}; // class

} // namespace

#endif
