/**
 * @file Core/Basic/TiInterface.h
 * Contains the header of class Core::Basic::TiInterface.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIINTERFACE_H
#define CORE_BASIC_TIINTERFACE_H

namespace Core { namespace Basic
{

/**
 * @brief The base of all identifiable interfaces.
 * @ingroup basic_utils
 *
 * Identifiable interfaces are interfaces that have run-time type information.
 * This abstract class uses polymorphism to provide RTTI. Identifiable
 * interfaces works with identifiable objects, which has special methods for
 * querying those interfaces.
 */
class TiInterface
{
  //============================================================================
  // Pure Virtual Destructor

  public: virtual ~TiInterface() {}


  //============================================================================
  // Member Functions

  /**
   * @brief Get the type info of this interface.
   *
   * Returns a pointer to the type information object associated with this
   * interface. Derived interfaces must implement this function. However,
   * derived interfaces should not manually implement this function, instead,
   * they should use the INTERFACE_INFO macro.
   *
   * @return A pointer to the TypeInfo object associated with this interface.
   *         All interfaces of the same type should return the same specific
   *         object, not a different object with the same info.
   */
  public: virtual TypeInfo* getMyInterfaceInfo() const = 0;

  /**
   * @brief Check if this interface is of the given type.
   *
   * @return Returns true if the given type info is for this interface's type,
   *         false otherwise.
   */
  public: Bool isInterfaceA(TypeInfo *info) const
  {
    return this->getMyInterfaceInfo() == info;
  }

  /**
   * @brief A template equivalent to isInterfaceA.
   * @sa isInterfaceA()
   */
  public: template <class T> Bool isInterfaceA() const
  {
    return this->getMyInterfaceInfo() == T::getTypeInfo();
  }

  /// Get this interface's type info.
  public: static TypeInfo* getTypeInfo();

  /// Get a pointer to the requested interface.
  public: void* getInterfacePtr(TypeInfo *info)
  {
    if (info == TiInterface::getTypeInfo())
      return reinterpret_cast<void*>(this);
    else
      return 0;
  }

  /// Check if this interface is of the given type, or a derived type.
  public: Bool isInterfaceDerivedFrom(TypeInfo *info) const;

  /**
   * @brief A template equivalent to isInterfaceDerivedFrom.
   * @sa isInterfaceDerivedFrom()
   */
  public: template <class T> Bool isInterfaceDerivedFrom() const
  {
    return this->isInterfaceDerivedFrom(T::getTypeInfo());
  }

  /// Get the identifiable object that owns this interface.
  public: virtual TiObject* getTiObject() = 0;

  /**
   * @brief A const version of getTiObject.
   * @sa getTiObject()
   */
  public: virtual TiObject const* getTiObject() const = 0;

}; // class


/**
 * @brief A macro used to define the type info for an identifiable interface.
 * @ingroup basic_utils
 *
 * This macro is used to define the type info methods for interfaces derived
 * from TiInterface. It defines and implements the virtual
 * getMyInterfaceInfo and the static getTypeInfo functions.
 *
 * @param myType The class name as a keyword (rather than stringized).
 * @param baseType The base class name as a keyword (rather than stringized).
 * @param typeNamespace This class's full namespace resolution, in a string.
 *                        namespaces in this string should be seperated by a
 *                        dot.
 * @param moduleName The name of the module that contains the class. This can
 *                    be anything that identifies the module, whether it's a
 *                    filename or just a title.
 * @param url A URL associated with the module. This is used to help identify
 *            the module. It can be an empty string.
 */
#define INTERFACE_INFO(myType, baseType, typeNamespace, moduleName, url) \
  public: \
    virtual Core::Basic::TypeInfo * getMyInterfaceInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::TypeInfo * getTypeInfo() {   \
      static Core::Basic::TypeInfo *typeInfo = 0; \
      if (typeInfo == 0) { \
        Char const* uniqueName = url "#" moduleName "#" typeNamespace "." #myType; \
        typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName)); \
        if (typeInfo == 0) { \
          typeInfo = new Core::Basic::TypeInfo(#myType, typeNamespace, moduleName, url, \
          baseType::getTypeInfo()); \
          GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(typeInfo)); \
        } \
      } \
      return typeInfo; \
    } \
    void* getInterfacePtr(Core::Basic::TypeInfo *info) {\
      if (info == myType::getTypeInfo()) \
      return reinterpret_cast<void*>(this); \
      else \
      return baseType::getInterfacePtr(info); \
    }


/**
 * @brief Dynamically cast an TiObject to an implemented interface.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject class to dynamically cast a pointer of a base class to
 * one of its derived classes. If the object is not of that type, the result of
 * the function will be 0.
 */
template <class T> inline T* tii_cast(TiObject *object)
{
  return object==0 ? 0 : object->getInterface<T>();
}


/**
 * @brief Dynamically cast an TiInterface to a dervied type.
 * @ingroup basic_utils
 *
 * This inline template function uses the runt-time type info functions of the
 * TiInterface class to dynamically cast a pointer of a base
 * interface to one of its derived interfaces. If the object is not of that type
 * the result of the function will be 0.
 */
template <class T> inline T* tii_cast(TiInterface *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->getInterface<T>();
}


/**
 * @brief Dynamically cast an TiInterface to a derived class.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject class to dynamically cast a pointer of an interface to
 * one of the derived classes of its TiObject. This is simply a
 * shortcut to getting the TiObject of that interface and then
 * casting it to the right type. If the object is not of that type, the result
 * of the function will be 0.
 */
template <class T> inline T* tii2tio_cast(TiInterface *interface)
{
  return tio_cast<T>(interface==0 ? 0 : interface->getTiObject());
}


/**
 * @brief Check if the given pointer is of the given type.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject to check whether the given pointer is of the given type.
 * The function returns true only if the object is of that type. If the object
 * is of a type derived from the given type the function will return false.
 */
template <class T> inline bool isInterfaceA(TiInterface *interface)
{
  return (interface==0?false:interface->isInterfaceA(T::getTypeInfo()));
}


#define _IMPLEMENT_INTERFACES_CONDITION(info, interface) \
  interfacePtr = static_cast<interface*>(this)->getInterfacePtr(info); \
  if (interfacePtr != 0) return interfacePtr;
#define _IMPLEMENT_INTERFACES_CONDITIONS1(info, interface1) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface1)
#define _IMPLEMENT_INTERFACES_CONDITIONS2(info, interface1, interface2) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface2)
#define _IMPLEMENT_INTERFACES_CONDITIONS3(info, interface1, interface2, interface3) \
  _IMPLEMENT_INTERFACES_CONDITIONS2(info, interface1, interface2) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface3)
#define _IMPLEMENT_INTERFACES_CONDITIONS4(info, interface1, interface2, interface3, interface4) \
  _IMPLEMENT_INTERFACES_CONDITIONS3(info, interface1, interface2, interface3) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface4)
#define _IMPLEMENT_INTERFACES_CONDITIONS5(info, interface1, interface2, interface3, interface4, interface5) \
  _IMPLEMENT_INTERFACES_CONDITIONS4(info, interface1, interface2, interface3, interface4) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface5)
#define _IMPLEMENT_INTERFACES_CONDITIONS6(info, interface1, interface2, interface3, interface4, interface5, interface6) \
  _IMPLEMENT_INTERFACES_CONDITIONS5(info, interface1, interface2, interface3, interface4, interface5) \
  _IMPLEMENT_INTERFACES_CONDITION(info, interface6)
#define _IMPLEMENT_INTERFACES_CONDITIONS(info, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_INTERFACES_CONDITIONS6, \
               _IMPLEMENT_INTERFACES_CONDITIONS5, \
               _IMPLEMENT_INTERFACES_CONDITIONS4, \
               _IMPLEMENT_INTERFACES_CONDITIONS3, \
               _IMPLEMENT_INTERFACES_CONDITIONS2, \
               _IMPLEMENT_INTERFACES_CONDITIONS1)(info, __VA_ARGS__)

/**
 * @brief A macro used in TiObjects to implement interfaces.
 * @ingroup basic_utils
 *
 * TiObjects need to use this macro to implement the getInterface
 * method which is used to query the object for impelemnted interfaces. It also
 * implements the getTiObject method of TiInterface that
 * enables the user the query the original object from the interface. This macro
 * allows the user to implement only one interface. If the user's object
 * implements more than one interface, the following equivalent macros can be
 * used instead: IMPLEMENT_INTERFACES_2, IMPLEMENT_INTERFACES_3,
 * IMPELEMENT_INTERFACES_4, and IMPLEMENT_INTERFACES_5.
 *
 * @param parent The type of the parent class. This is needed so the method can
 *               allow the call to propagate to the parent if the interface is
 *               not implemented by the child.
 * @param interface1 The type of the first interface implemented by the class.
 * @param interface2 The type of the second interface implemented by the class.
 * @param interface3 The type of the third interface implemented by the class.
 * @param interface4 The type of the fourth interface implemented by the class.
 * @param interface5 The type of the fifth interface implemented by the class.
 * @note IMPLEMENT_INTERFACES_1 uses only interface1 param.
 *       IMPLEMENT_INTERFACES_2 uses interface1 and interface2, and so on.
 */
#define IMPLEMENT_INTERFACES(parent, ...) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITIONS(info, __VA_ARGS__) \
      return parent::_getInterface(info); \
    } \
  public: \
    virtual TiObject* getTiObject() { return this; } \
    virtual TiObject const* getTiObject() const { return this; }

#define IMPLEMENT_INTERFACES_1(parent, interface1) \
  IMPLEMENT_INTERFACES(parent, interface1)

#define IMPLEMENT_INTERFACES_2(parent, interface1, interface2) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2)

#define IMPLEMENT_INTERFACES_3(parent, interface1, interface2, interface3) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3)

#define IMPLEMENT_INTERFACES_4(parent, interface1, interface2, interface3, interface4) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3, interface4)

#define IMPLEMENT_INTERFACES_5(parent, interface1, interface2, interface3, interface4, interface5) \
  IMPLEMENT_INTERFACES(parent, interface1, interface2, interface3, interface4, interface5)

} } // namespace

#endif
