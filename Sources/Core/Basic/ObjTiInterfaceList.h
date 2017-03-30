/**
 * @file Core/Basic/ObjTiInterfaceList.h
 * Contains the header of class Core::Basic::ObjTiInterfaceList.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_OBJTIINTERFACELIST_H
#define CORE_BASIC_OBJTIINTERFACELIST_H

namespace Core { namespace Basic
{

typedef SharedList<ObjTiInterface, TiObject> ObjTiInterfaceListBase;

class ObjTiInterfaceList : public ObjTiInterfaceListBase
{
  //============================================================================
  // Type Info

  TYPE_INFO(ObjTiInterfaceList, ObjTiInterfaceListBase, "Core.Basic", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: TiObject *obj;


  //============================================================================
  // Constructors & Destructor

  public: ObjTiInterfaceList(TiObject *o) : obj(o)
  {
  }

  public: ObjTiInterfaceList(TiObject *o, std::initializer_list<SharedPtr<ObjTiInterface>> const &args)
    : obj(o), ObjTiInterfaceListBase(args)
  {
  }


  //============================================================================
  // Member Functions

  using ObjTiInterfaceListBase::remove;

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

  public: ObjTiInterface* get(TypeInfo *ti) const
  {
    for (Int i = this->getCount() - 1; i >= 0; --i) {
      ObjTiInterface *pi = this->get(i).get();
      if (pi->isInterfaceDerivedFrom(ti)) return pi;
    }
    return 0;
  }

  using ObjTiInterfaceListBase::get;

  public: template<class T> T* get() const
  {
    return static_cast<T*>(this->get(T::getTypeInfo()));
  }

  private: virtual SharedPtr<ObjTiInterface> getFromBase(Int index)
  {
    auto typeInfo = this->base->get(index)->getMyObjInterfaceInfo();
    return typeInfo->getFactory()->createShared(this->obj);
  }

}; // class

} } // namespace

#endif
