/**
 * @file Core/Basic/TiInterface.h
 * Contains the header of class Core::Basic::TiInterface.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
  public: virtual InterfaceTypeInfo const* getMyInterfaceInfo() const = 0;

  /**
   * @brief Check if this interface is of the given type.
   *
   * @return Returns true if the given type info is for this interface's type,
   *         false otherwise.
   */
  public: Bool isInterfaceA(TypeInfo const *info) const
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
  public: static InterfaceTypeInfo const* getTypeInfo();

  /// Get a pointer to the requested interface.
  public: TiInterface* getInterfacePtr(TypeInfo const *info)
  {
    if (info == TiInterface::getTypeInfo()) {
      return this;
    } else {
      return 0;
    }
  }

  /// Check if this interface is of the given type, or a derived type.
  public: Bool isInterfaceDerivedFrom(TypeInfo const *info) const;

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
template <class T> inline bool isInterfaceA(TiInterface *interface)
{
  return (interface==0?false:interface->isInterfaceA(T::getTypeInfo()));
}

} } // namespace

#endif
