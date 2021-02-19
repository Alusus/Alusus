/**
 * @file Core/Basic/SharedPtr.h
 * Contains definition of Basic::SharedPtr template class.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDPTR_H
#define CORE_BASIC_SHAREDPTR_H

namespace Core::Basic
{

// TODO:

template <class T> class SharedPtr : public Srl::SrdRef<T>
{
  //============================================================================
  // Constructors

  public: using Srl::SrdRef<T>::SrdRef;

  public: SharedPtr(Srl::SrdRef<T> const &r) : Srl::SrdRef<T>(r) {}

  public: SharedPtr(Srl::SrdRef<T> &&r) : Srl::SrdRef<T>(r) {}

  public: template<class T2> SharedPtr(Srl::SrdRef<T2> const &p) : Srl::SrdRef<T>(p) {}

  public: template<class T2> SharedPtr(Srl::SrdRef<T2> &&p) : Srl::SrdRef<T>(p) {}


  //============================================================================
  // Static Variables

  public: static SharedPtr<T> const null;


  //============================================================================
  // Overloaded Operators

  /// @name Comparison Operators
  /// @{

  /// Check whether the given subject is the same as this one.
  public: template <class T2> Bool operator==(T2 *subject) const
  {
    return this->get() == subject;
  }

  /// Check whether the given subject is not the same as this one.
  public: template <class T2> Bool operator!=(T2 *subject) const
  {
    return this->get() != subject;
  }

  /// Check whether the given pointer share the same subject with this one.
  public: template <class T2> Bool operator==(const Srl::SrdRef<T2> &ptr) const
  {
    return this->get() == ptr.get();
  }

  /// Check whether the given pointer has a different subject with this one.
  public: template <class T2> Bool operator!=(const Srl::SrdRef<T2> &ptr) const
  {
    return this->get() != ptr.get();
  }

  /// Check whether the given pointer share the same subject with this one.
  public: template <class T2> Bool operator==(const std::weak_ptr<T2> &ptr) const
  {
    return this->get() == ptr.get();
  }

  /// Check whether the given pointer has a different subject with this one.
  public: template <class T2> Bool operator!=(const std::weak_ptr<T2> &ptr) const
  {
    return this->get() != ptr.get();
  }

  /// @}


  //==============================================================================
  // Member Functions

  /// @name General Functions
  /// @{

  public: void reset() {
    this->release();
  }

  /// @}

  /// @name Casting Functions
  /// @{

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using static_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> s_cast() const
  {
    return SharedPtr<TO>(this->refCounter, static_cast<TO*>(this->get()));
  }

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using reinterpret_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> r_cast() const
  {
    return SharedPtr<TO>(this->refCounter, reinterpret_cast<TO*>(this->get()));
  }

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using const_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> c_cast() const
  {
    return SharedPtr<TO>(this->refCounter, const_cast<TO*>(this->get()));
  }

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using ti_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> ti_cast() const
  {
    return SharedPtr<TO>(this->refCounter, Core::Basic::ti_cast<TO>(this->get()));
  }

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using static_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* s_cast_get() const
  {
    return static_cast<TO*>(this->get());
  }

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using reinterpret_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* r_cast_get() const
  {
    return reinterpret_cast<TO*>(this->get());
  }

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using const_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* c_cast_get() const
  {
    return const_cast<TO*>(this->get());
  }

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using ti_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* ti_cast_get() const
  {
    return Core::Basic::ti_cast<TO>(this->get());
  }

  /// @}

}; // class


//==============================================================================
// Static Variables Initializations

template<class T> SharedPtr<T> const SharedPtr<T>::null = 0;


//==============================================================================
// Helper Definitions

template <class T, class T2> inline SharedPtr<T> s_cast(Srl::SrdRef<T2> const &src)
{
  return SharedPtr<T>(src.refCounter, static_cast<T*>(src.get()));
}


template <class T, class T2> inline SharedPtr<T> r_cast(Srl::SrdRef<T2> const &src)
{
  return SharedPtr<T>(src.refCounter, reinterpret_cast<T*>(src.get()));
}


template <class T> inline SharedPtr<T> c_cast(Srl::SrdRef<const T> const &src)
{
  return SharedPtr<T>(src.refCounter, const_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T> ti_cast(Srl::SrdRef<T2> const &src)
{
  return SharedPtr<T>(src.getRefCounter(), Core::Basic::ti_cast<T>(src.get()));
}


/**
 * @brief Get a shared pointer to the given TiObject based pointer.
 * The function will get a shared pointer that shares ownership with existing
 * shared pointers to this object. It will automatically cast it to the type of
 * the passed pointer.
 * @param obj A pointer to an object derived from TiObject.
 * @param ownIfUnowned Specify whether to own this object if it's not already
 *                     owned. If true and no shared pointer owns this object,
 *                     the function will create a new shared pointer to own
 *                     this, otherwise it will return an empty pointer.
 */
template <class T,
          typename std::enable_if<std::is_base_of<TiObject, T>::value, int>::type = 0>
SharedPtr<T> getSharedPtr(T *obj, Bool ownIfUnowned=false)
{
  if (obj == 0) return SharedPtr<T>();
  else if (obj->getWkThis() != 0) return SharedPtr<T>(obj->getWkThis().getRefCounter(), obj);
  else if (ownIfUnowned) return SharedPtr<T>::ownToNew(obj);
  else return SharedPtr<T>();
}

template <class T,
          typename std::enable_if<std::is_base_of<TiObject, T>::value, int>::type = 0>
SharedPtr<T const> getSharedPtr(T const *obj, Bool ownIfUnowned=false)
{
  if (obj == 0) return SharedPtr<T const>();
  else if (obj->getWkThis() != 0) return SharedPtr<T const>(obj->getWkThis().getRefCounter(), obj);
  else if (ownIfUnowned) return SharedPtr<T const>::ownToNew(obj);
  else return SharedPtr<T const>();
}

template <class T,
          typename std::enable_if<!std::is_base_of<TiObject, T>::value, int>::type = 0>
SharedPtr<T> getSharedPtr(T *obj, Bool ownIfUnowned=false)
{
  if (obj == 0) return SharedPtr<T>();
  else if (obj->getTiObject()->getWkThis() != 0) {
    return SharedPtr<T>(obj->getTiObject()->getWkThis().getRefCounter(), obj);
  } else if (ownIfUnowned) return SharedPtr<T>::ownToNew(obj);
  else return SharedPtr<T>();
}

template <class T,
          typename std::enable_if<!std::is_base_of<TiObject, T>::value, int>::type = 0>
SharedPtr<T const> getSharedPtr(T const *obj, Bool ownIfUnowned=false)
{
  if (obj == 0) return SharedPtr<T const>();
  else if (obj->getTiObject()->getWkThis() != 0) {
    return SharedPtr<T const>(obj->getTiObject()->getWkThis().getRefCounter(), obj);
  } else if (ownIfUnowned) return SharedPtr<T const>::ownToNew(obj);
  else return SharedPtr<T const>();
}


typedef SharedPtr<TiObject> TioSharedPtr;

} // namespace

#endif
