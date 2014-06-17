/**
 * @file Core/Basic/IdentifiableInterface.h
 * Contains the header of class Core::Basic::IdentifiableInterface.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_IDENTIFIABLE_INTERFACE_H
#define BASIC_IDENTIFIABLE_INTERFACE_H

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
class IdentifiableInterface
{
  //============================================================================
  // Pure Virtual Destructor

  public: virtual ~IdentifiableInterface() {}


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
    return this->getMyInterfaceInfo() == T::getInterfaceInfo();
  }

  /// Get this interface's type info.
  public: static TypeInfo* getInterfaceInfo();

  /// Get a pointer to the requested interface.
  public: void* getInterfacePtr(TypeInfo *info)
  {
    if (info == IdentifiableInterface::getInterfaceInfo())
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
    return this->isInterfaceDerivedFrom(T::getInterfaceInfo());
  }

  /// Get the identifiable object that owns this interface.
  public: virtual IdentifiableObject* getIdentifiableObject() = 0;

  /**
   * @brief A const version of getIdentifiableObject.
   * @sa getIdentifiableObject()
   */
  public: virtual IdentifiableObject const* getIdentifiableObject() const = 0;

}; // class


/**
 * @brief A macro used to define the type info for an identifiable interface.
 * @ingroup basic_utils
 *
 * This macro is used to define the type info methods for interfaces derived
 * from IdentifiableInterface. It defines and implements the virtual
 * getMyInterfaceInfo and the static getInterfaceInfo functions.
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
    virtual Core::Basic::TypeInfo * getMyInterfaceInfo() const { return myType::getInterfaceInfo(); } \
    static Core::Basic::TypeInfo * getInterfaceInfo() {   \
      static Core::Basic::TypeInfo *typeInfo = 0; \
      if (typeInfo == 0) { \
        Char const* uniqueName = url "#" moduleName "#" typeNamespace "." #myType; \
        typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName)); \
        if (typeInfo == 0) { \
          typeInfo = new Core::Basic::TypeInfo(#myType, typeNamespace, moduleName, url, \
          baseType::getInterfaceInfo()); \
          GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(typeInfo)); \
        } \
      } \
      return typeInfo; \
    } \
    void* getInterfacePtr(Core::Basic::TypeInfo *info) {\
      if (info == myType::getInterfaceInfo()) \
      return reinterpret_cast<void*>(this); \
      else \
      return baseType::getInterfacePtr(info); \
    }


/**
 * @brief Dynamically cast an IdentifiableObject to an implemented interface.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * IdentifiableObject class to dynamically cast a pointer of a base class to
 * one of its derived classes. If the object is not of that type, the result of
 * the function will be 0.
 */
template <class T> inline T* ii_cast(IdentifiableObject *object)
{
  return object==0 ? 0 : object->getInterface<T>();
}


/**
 * @brief Dynamically cast an IdentifiableInterface to a dervied type.
 * @ingroup basic_utils
 *
 * This inline template function uses the runt-time type info functions of the
 * IdentifiableInterface class to dynamically cast a pointer of a base
 * interface to one of its derived interfaces. If the object is not of that type
 * the result of the function will be 0.
 */
template <class T> inline T* ii_cast(IdentifiableInterface *interface)
{
  return interface==0 ? 0 : interface->getIdentifiableObject()->getInterface<T>();
}


/**
 * @brief Dynamically cast an IdentifiableInterface to a derived class.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * IdentifiableObject class to dynamically cast a pointer of an interface to
 * one of the derived classes of its IdentifiableObject. This is simply a
 * shortcut to getting the IdentifiableObject of that interface and then
 * casting it to the right type. If the object is not of that type, the result
 * of the function will be 0.
 */
template <class T> inline T* ii2io_cast(IdentifiableInterface *interface)
{
  return io_cast<T>(interface==0 ? 0 : interface->getIdentifiableObject());
}


/**
 * @brief Check if the given pointer is of the given type.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * IdentifiableObject to check whether the given pointer is of the given type.
 * The function returns true only if the object is of that type. If the object
 * is of a type derived from the given type the function will return false.
 */
template <class T> inline bool isInterfaceA(IdentifiableInterface *interface)
{
  return (interface==0?false:interface->isInterfaceA(T::getTypeInfo()));
}


#define _IMPLEMENT_INTERFACES_CONDITION(info, interface) \
  interfacePtr = static_cast<interface*>(this)->getInterfacePtr(info); \
  if (interfacePtr != 0) return interfacePtr;


/**
 * @brief A macro used in IdentifiableObjects to implement interfaces.
 * @ingroup basic_utils
 *
 * IdentifiableObjects need to use this macro to implement the getInterface
 * method which is used to query the object for impelemnted interfaces. It also
 * implements the getIdentifiableObject method of IdentifiableInterface that
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
#define IMPLEMENT_INTERFACES_1(parent, interface1) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
      return parent::_getInterface(info); \
    } \
  public: \
    virtual IdentifiableObject* getIdentifiableObject() { return this; } \
    virtual IdentifiableObject const* getIdentifiableObject() const { return this; }

#define IMPLEMENT_INTERFACES_2(parent, interface1, interface2) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface2) \
      return parent::_getInterface(info); \
    } \
    public: \
      virtual IdentifiableObject* getIdentifiableObject() { return this; } \
      virtual IdentifiableObject const* getIdentifiableObject() const { return this; }

#define IMPLEMENT_INTERFACES_3(parent, interface1, interface2, interface3) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface2) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface3) \
      return parent::_getInterface(info); \
    } \
    public: \
      virtual IdentifiableObject* getIdentifiableObject() { return this; } \
      virtual IdentifiableObject const* getIdentifiableObject() const { return this; }

#define IMPLEMENT_INTERFACES_4(parent, interface1, interface2, interface3, interface4) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface2) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface3) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface4) \
      return parent::_getInterface(info); \
    } \
    public: \
      virtual IdentifiableObject* getIdentifiableObject() { return this; } \
      virtual IdentifiableObject const* getIdentifiableObject() const { return this; }

#define IMPLEMENT_INTERFACES_5(parent, interface1, interface2, interface3, interface4, interface5) \
  protected: \
    virtual void* _getInterface(TypeInfo *info) { \
      void* interfacePtr; \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface1) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface2) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface3) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface4) \
      _IMPLEMENT_INTERFACES_CONDITION(info, interface5) \
      return parent::_getInterface(info); \
    } \
    public: \
      virtual IdentifiableObject* getIdentifiableObject() { return this; } \
      virtual IdentifiableObject const* getIdentifiableObject() const { return this; }

} } // namespace

#endif
