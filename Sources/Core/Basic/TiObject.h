/**
 * @file Core/Basic/TiObject.h
 * Contains the header of class Core::Basic::TiObject.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
  public: virtual ObjectTypeInfo* getMyTypeInfo() const = 0;

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
  public: static ObjectTypeInfo* getTypeInfo();

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

  /**
   * @brief Gets a weak pointer to this.
   * This function returns a weak pointer that shares ownership of this object
   * with existing weak pointer.
   */
  public: std::weak_ptr<TiObject> getWeakThis()
  {
    try
    {
      return enable_shared_from_this::weak_from_this();
    }
    catch(const std::bad_weak_ptr &e)
    {
      return std::weak_ptr<TiObject>();
    }
  }

  /**
   * @brief A const version of getWeakThis.
   * @sa getWeakThis()
   */
  public: std::weak_ptr<TiObject const> getWeakThis() const
  {
    try
    {
      return enable_shared_from_this::weak_from_this();
    }
    catch(const std::bad_weak_ptr &e)
    {
      return std::weak_ptr<TiObject>();
    }
  }

}; // class


//==============================================================================
// Global Helper Functions

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
