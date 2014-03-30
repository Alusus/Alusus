/**
 * @file Core/Basic/IdentifiableObject.h
 * Contains the header of class Basic::IdentifiableObject.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_IDENTIFIABLE_OBJECT_H
#define BASIC_IDENTIFIABLE_OBJECT_H

namespace Core { namespace Basic
{

class IdentifiableInterface;

/**
 * @brief The base of all identifiable classes.
 * @ingroup basic_utils
 *
 * Identifiable classes are classes that have run-time type information. This
 * abstract class uses polymorphism to provide RTTI.
 */
class IdentifiableObject
{
  //============================================================================
  // Virtual Destructor

  public: virtual ~IdentifiableObject() {}


  //============================================================================
  // Member Functions

  /**
   * @brief Get the type info of this object.
   *
   * Returns a pointer to the type information object associated with this
   * object. Derived classes must implement this function. However, derived
   * classes should not manually implement this function, instead, they should
   * use the TYPE_INFO macro.
   *
   * @return A pointer to the TypeInfo object associated with this object.
   *         All object of the same type should return the same specific
   *         object, not a different object with the same info.
   */
  public: virtual TypeInfo* getMyTypeInfo() const = 0;

  /**
   * @brief Check if this object is of the given type.
   *
   * @return Returns true if the given type info is for the type from which
   *         this object is instantiated, false otherwise.
   */
  public: Bool isA(TypeInfo *info) const
  {
    return this->getMyTypeInfo() == info;
  }

  /**
   * @brief A template equivalent to isA.
   * @sa isA()
   */
  public: template <class T> Bool isA() const
  {
    return this->getMyTypeInfo() == T::getTypeInfo();
  }

  /// Get this type's info.
  public: static TypeInfo* getTypeInfo();

  /// Check if this object is of the given type, or a derived type.
  public: Bool isDerivedFrom(TypeInfo *info) const;

  /**
     * @brief A template equivalent to isDerivedFrom.
     * @sa isDerivedFrom()
     */
  public: template <class T> Bool isDerivedFrom() const
  {
    return this->isDerivedFrom(T::getTypeInfo());
  }

  /**
   * @brief Get a pointer to the given interface, if implemented.
   * When a class implements IdentifiableInterfaces it should override this
   * method and return a pointer to the requested type (by casting itself to
   * that type). If the requested interface is not implemented by the class it
   * should return 0, which is the default implementation. Classes should use
   * the IMPLEMENT_INTERFACE family of macros to do this instead of manually
   * implementing this method.
   * @note The method returns void* instead of IdentifiableInterface in
   *       order to avoid casting that can result in changing the value
   *       of the pointer due to virtual inheritance. Users should use the
   *       template version of getInterface.
   */
  protected: virtual void* _getInterface(TypeInfo *info)
  {
    return 0;
  }

  /**
   * @brief Get a pointer to the given interface, if implemented.
   * @return A pointer to the requested interface, or 0 if that interface is
   *         not implemented in this class.
   */
  public: template<class T> T* getInterface()
  {
    return reinterpret_cast<T*>(this->_getInterface(T::getInterfaceInfo()));
  }

  /**
   * @brief A const version of getInterface.
   * @sa getInterface()
   */
  public: template<class T> const T* getInterface() const
  {
    return reinterpret_cast<const T*>(this->_getInterface(T::getInterfaceInfo()));
  }

}; // class


/**
 * @brief A macro that defines the body of getTypeInfo static functions.
 * @ingroup basic_utils
 *
 * This macro is used indirectly by the TYPE_INFO and INTERFACE_INFO macros, so
 * the user does not need to use this macro directly.
 */
#define GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
  static Core::Basic::TypeInfo *typeInfo = 0; \
  if (typeInfo == 0) { \
    const Char* uniqueName = url "#" moduleName "#" typeNamespace "." #myType; \
    typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName)); \
    if (typeInfo == 0) { \
      typeInfo = new Core::Basic::TypeInfo(#myType, typeNamespace, moduleName, url, \
      baseType::getTypeInfo()); \
      GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(typeInfo)); \
    } \
  } \
  return typeInfo;


/**
 * @brief A macro used to define the type info for an identifiable object.
 * @ingroup basic_utils
 *
 * This macro is used to define the type info methods for classes derived
 * from IdentifiableObject. It defines and implements the virtual
 * getMyTypeInfo and the static getTypeInfo functions.
 *
 * @param myType The type name as a keyword (rather than stringized).
 * @param baseType The base type name as a keyword (rather than stringized).
 * @param typeNamespace This type's full namespace resolution, in a string.
 *                        namespaces in this string should be seperated by a
 *                        dot.
 * @param moduleName The name of the module that contains the type. This can
 *                    be anything that identifies the module, whether it's a
 *                    filename or just a title.
 * @param url A URL associated with the module. This is used to help identify
 *            the module. It can be an empty string.
 */
#define TYPE_INFO(myType, baseType, typeNamespace, moduleName, url) \
  public: \
    virtual Core::Basic::TypeInfo * getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::TypeInfo * getTypeInfo() { \
      GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
    }


// TODO: Do we need TYPE_INFO_HEADER and TYPE_INFO_IMPLEMENTATION?

/**
 * @brief A macro used to declare the type info for an identifiable object.
 * @ingroup basic_utils
 *
 * This macro is used to declare the type info methods for classes derived
 * from IdentifiableObject. It only declares the methods and leaves the
 * implementation for the TYPE_INFO_IMPLEMENTATION macro which must then be
 * used inside the class's implementation file.<br>
 * This macro, along with the TYPE_INFO_IMPLEMENTATION macro, are provided
 * as an alternative to the TYPE_INFO macro in cases where the implementation
 * needs to be separated from the declaration due to the base class not being
 * defined yet (only declared) (Is that even possible though??)
 *
 * @param myType The class name as a keyword (rather than stringized).
 */
#define TYPE_INFO_HEADER(myType) \
  public: \
    virtual Core::Basic::TypeInfo * getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::TypeInfo * getTypeInfo();


/**
 * @brief A macro used to implement the type info for an identifiable object.
 * @ingroup basic_utils
 *
 * This macro is used to implement the type info methods declared using the
 * TYPE_INFO_HEADER macro for classes derived from IdentifiableObject. It
 * defines the implementation for the static getTypeInfo function.
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
#define TYPE_INFO_IMPLEMENTATION(myType, baseType, typeNamespace, moduleName, url) \
  Core::Basic::TypeInfo * myType::getTypeInfo() { \
    GET_TYPE_INFO_BODY(myType, baseType, typeNamespace, moduleName, url) \
  }


/**
 * @brief Dynamically cast an IdentifiableObject to a derived class.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * IdentifiableObject class to dynamically cast a pointer of a base class to
 * one of its derived classes. If the object is not of that type, the result of
 * the function will be 0.
 */
template <class T> inline T* io_cast(IdentifiableObject *object)
{
  return static_cast<T*>(object==0?0:object->isDerivedFrom(T::getTypeInfo())?object:0);
}

/// @sa io_cast()
template <class T> inline const T* io_cast(const IdentifiableObject *object)
{
  return static_cast<const T*>(object==0?0:object->isDerivedFrom(T::getTypeInfo())?object:0);
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
template <class T> inline bool isA(const IdentifiableObject *object)
{
  return (object==0?false:object->isA(T::getTypeInfo()));
}

} } // namespace

#endif
