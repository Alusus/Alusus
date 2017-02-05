/**
 * @file Core/Basic/TiObject.h
 * Contains the header of class Core::Basic::TiObject.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIOBJECT_H
#define CORE_BASIC_TIOBJECT_H

namespace Core { namespace Basic
{

class TiInterface;

/**
 * @brief The base of all identifiable classes.
 * @ingroup basic_utils
 *
 * Identifiable classes are classes that have run-time type information. This
 * abstract class uses polymorphism to provide RTTI.
 */
class TiObject : public std::enable_shared_from_this<TiObject>
{
  //============================================================================
  // Virtual Destructor

  public: virtual ~TiObject() {}


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
  public: Bool isDerivedFrom(TypeInfo const *info) const;

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
   * When a class implements TiInterfaces it should override this
   * method and return a pointer to the requested type (by casting itself to
   * that type). If the requested interface is not implemented by the class it
   * should return 0, which is the default implementation. Classes should use
   * the IMPLEMENT_INTERFACE family of macros to do this instead of manually
   * implementing this method.
   * @note The method returns void* instead of TiInterface in
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
    return reinterpret_cast<T*>(this->_getInterface(T::getTypeInfo()));
  }

  /**
   * @brief A const version of getInterface.
   * @sa getInterface()
   */
  public: template<class T> const T* getInterface() const
  {
    return reinterpret_cast<const T*>(const_cast<TiObject*>(this)->_getInterface(T::getTypeInfo()));
  }

  /**
   * @brief Gets a shared pointer to this.
   * This function returns a shared pointer that shares ownership of this object
   * with existing shared pointer.
   */
  public: std::shared_ptr<TiObject> getSharedThis()
  {
    try
    {
      return enable_shared_from_this::shared_from_this();
    }
    catch(const std::bad_weak_ptr &e)
    {
      return std::shared_ptr<TiObject>();
    }
  }

  /**
   * @brief A const version of getSharedThis.
   * @sa getSharedThis()
   */
  public: std::shared_ptr<TiObject const> getSharedThis() const
  {
    try
    {
      return enable_shared_from_this::shared_from_this();
    }
    catch(const std::bad_weak_ptr &e)
    {
      return std::shared_ptr<TiObject>();
    }
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
    Char const* uniqueName = url "/" moduleName "/" typeNamespace "." myType; \
    typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName)); \
    if (typeInfo == 0) { \
      typeInfo = new Core::Basic::TypeInfo(myType, typeNamespace, moduleName, url, \
      baseType::getTypeInfo()); \
      GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(typeInfo)); \
    } \
  } \
  return typeInfo;


/**
 * @brief A template version of GET_TYPE_INFO_BODY.
 * @ingroup basic_utils
 * @sa GET_TYPE_INFO_BODY
 */
#define GET_TYPE_INFO_TEMPLATE_BODY(myType, baseType, typeNamespace, moduleName, url, ...) \
  static Core::Basic::TypeInfo *typeInfo = 0; \
  if (typeInfo == 0) { \
    Str typeName = Str(#myType) + "<" + TypeName<__VA_ARGS__>::get() + ">"; \
    Str uniqueName = Str(url "/" moduleName "/" typeNamespace ".") + typeName; \
    typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName.c_str())); \
    if (typeInfo == 0) { \
      typeInfo = new Core::Basic::TypeInfo(typeName.c_str(), typeNamespace, moduleName, url, \
                                           baseType::getTypeInfo()); \
      GLOBAL_STORAGE->setObject(uniqueName.c_str(), reinterpret_cast<void*>(typeInfo)); \
    } \
  } \
  return typeInfo;


/**
 * @brief A macro used to define the type info for an identifiable object.
 * @ingroup basic_utils
 *
 * This macro is used to define the type info methods for classes derived
 * from TiObject. It defines and implements the virtual
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
    virtual Core::Basic::TypeInfo* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::TypeInfo* getTypeInfo() { \
      GET_TYPE_INFO_BODY(#myType, baseType, typeNamespace, moduleName, url) \
    }


/**
 * @brief A template version of TYPE_INFO.
 * @ingroup basic_utils
 * @sa TYPE_INFO
 */
#define TEMPLATE_TYPE_INFO(myType, baseType, typeNamespace, moduleName, url, ...) \
  public: \
    virtual Core::Basic::TypeInfo* getMyTypeInfo() const { return myType<__VA_ARGS__>::getTypeInfo(); } \
    static Core::Basic::TypeInfo* getTypeInfo() { \
      GET_TYPE_INFO_TEMPLATE_BODY(myType, baseType, typeNamespace, moduleName, url, __VA_ARGS__) \
    }


/**
 * @brief A custom version of TYPE_INFO allowing defining the name manually.
 * @ingroup basic_utils
 * @sa TYPE_INFO
 */
#define CUSTOM_NAME_TYPE_INFO(myType, myCustomTypeName, baseType, typeNamespace, moduleName, url) \
  public: \
    virtual Core::Basic::TypeInfo* getMyTypeInfo() const { return myType::getTypeInfo(); } \
    static Core::Basic::TypeInfo* getTypeInfo() { \
      GET_TYPE_INFO_BODY(myCustomTypeName, baseType, typeNamespace, moduleName, url) \
    }


/**
 * @brief Dynamically cast an TiObject to a derived class.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject class to dynamically cast a pointer of a base class to
 * one of its derived classes. If the object is not of that type, the result of
 * the function will be 0.
 */
template <class T> inline T* tio_cast(TiObject *object)
{
  return static_cast<T*>(object==0?0:object->isDerivedFrom(T::getTypeInfo())?object:0);
}

/// @sa tio_cast()
template <class T> inline T const* tio_cast(TiObject const *object)
{
  return static_cast<const T*>(object==0?0:object->isDerivedFrom(T::getTypeInfo())?object:0);
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
template <class T> inline Bool isA(TiObject const *object)
{
  return (object == 0 ? false : object->isA(T::getTypeInfo()));
}

/**
 * @brief Check if the given pointer is of a type derived from the given type.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject to check whether the given pointer is of the given type or a type
 * derived from it. It will return false if the pointer is null.
 */
template <class T> inline Bool isDerivedFrom(TiObject const *object)
{
  return (object == 0 ? false : object->isDerivedFrom(T::getTypeInfo()));
}

} } // namespace

#endif
