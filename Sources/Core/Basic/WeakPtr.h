/**
 * @file Core/Basic/WeakPtr.h
 * Contains definition of Basic::WeakPtr template class.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_WEAKPTR_H
#define CORE_BASIC_WEAKPTR_H

namespace Core::Basic
{

// TODO: DOC

template <class T> class WeakPtr : public Srl::WkRef<T>
{
  //============================================================================
  // Constructors / Destructor

  public: using Srl::WkRef<T>::WkRef;


  //============================================================================
  // Member Functions

  /// @name Casting Functions
  /// @{

  /**
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using static_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> s_cast() const
  {
    return SharedPtr<TO>(this->refCounter, static_cast<TO*>(this->obj));
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
    return static_cast<TO*>(this->obj);
  }

  /**
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using reinterpret_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> r_cast() const
  {
    return SharedPtr<TO>(this->refCounter, reinterpret_cast<TO*>(this->obj));
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
    return reinterpret_cast<TO*>(this->obj);
  }

  /**
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using const_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> c_cast() const
  {
    return SharedPtr<TO>(this->refCounter, const_cast<TO*>(this->obj));
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
    return const_cast<TO*>(this->obj);
  }

  /**
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using ti_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> ti_cast() const
  {
    return SharedPtr<TO>(this->refCounter, ti_cast<TO>(this->obj));
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
    return Core::Basic::ti_cast<TO>(this->obj);
  }

  /// @}

}; // class


//==============================================================================
// Helper Definitions

template <class T, class T2> SharedPtr<T> s_cast(Srl::WkRef<T2> const &src)
{
  return SharedPtr<T>(src.getRefCounter(), static_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T>& r_cast(Srl::WkRef<T2> const &src)
{
  return SharedPtr<T>(src.getRefCounter(), reinterpret_cast<T*>(src.get()));
}


template <class T> SharedPtr<T>& c_cast(Srl::WkRef<T> const &src)
{
  return SharedPtr<T>(src.getRefCounter(), const_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T>& ti_cast(Srl::WkRef<T2> const &src)
{
  return SharedPtr<T>(src.getRefCounter(), ti_cast<T>(src.get()));
}


/**
 * @brief Get a weak pointer to the given TiObject based pointer.
 * The function will get a weak pointer that shares ownership with existing
 * shared pointers to this object. It will automatically cast it to the type of
 * the passed pointer.
 * @param obj A pointer to an object derived from TiObject.
 */
template<class T> WeakPtr<T> getWeakPtr(T *obj)
{
  if (obj == 0) return WeakPtr<T>();
  else return WeakPtr<T>(obj->getWkThis().getRefCounter(), static_cast<T*>(obj->getWkThis().get()));
}


template<class T> WeakPtr<T const> getWeakPtr(T const *obj)
{
  if (obj == 0) return WeakPtr<T const>();
  else return WeakPtr<T const>(obj->getWkThis().getRefCounter(), static_cast<T*>(obj->getWkThis().get()));
}


typedef WeakPtr<TiObject> TioWeakPtr;

} // namespace

#endif
