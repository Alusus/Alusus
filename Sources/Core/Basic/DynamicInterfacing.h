/**
 * @file Core/Basic/DynamicInterfacing.h
 * Contains the header of interface Core::Basic::DynamicInterfacing.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICINTERFACING_H
#define CORE_BASIC_DYNAMICINTERFACING_H

namespace Core::Basic
{

class DynamicInterfacing : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(DynamicInterfacing, TiInterface, "Core.Basic", "Core", "alusus.org");


  //============================================================================
  // Member Functions

  public: virtual ObjTiInterfaceList* getDynamicInterfaceList() = 0;

  public: virtual ObjTiInterfaceList const* getDynamicInterfaceList() const
  {
    return const_cast<DynamicInterfacing*>(this)->getDynamicInterfaceList();
  }

  public: virtual void inheritInterfaces(DynamicInterfacing *base)
  {
    this->getDynamicInterfaceList()->setBase(base->getDynamicInterfaceList());
  }

  public: virtual void addDynamicInterface(SharedPtr<ObjTiInterface> const &interface)
  {
    this->getDynamicInterfaceList()->add(interface);
  }

  public: virtual void removeDynamicInterface(TypeInfo *ti)
  {
    this->getDynamicInterfaceList()->remove(ti);
  }

  public: template<class T> void removeDynamicInterface()
  {
    this->removeDynamicInterface(T::getTypeInfo());
  }

  public: virtual ObjTiInterface* getDynamicInterface(TypeInfo *ti) const
  {
    return this->getDynamicInterfaceList()->get(ti);
  }

  public: template<class T> T* getDynamicInterface() const
  {
    return static_cast<T*>(this->getDynamicInterface(T::getTypeInfo()));
  }

}; // class


//==============================================================================
// Macros

#define IMPLEMENT_DYNAMIC_INTERFACING(interfaceList) \
  private: Core::Basic::ObjTiInterfaceList interfaceList = { this }; \
  public: virtual ObjTiInterfaceList* getDynamicInterfaceList() \
  { \
    return &this->interfaceList; \
  }

} // namespace

#endif
