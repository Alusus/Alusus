/**
 * @file Core/Basic/WeakPtr.h
 * Contains definition of Basic::WeakPtr template class.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_WEAK_PTR_H
#define CORE_BASIC_WEAK_PTR_H

namespace Core { namespace Basic
{

// TODO: DOC

template <class T> class WeakPtr : public std::weak_ptr<T>
{
  //============================================================================
  // Constructors / Destructor

  // TODO: Replace these constructors with constructor inheritance once supported by the compiler.

  public: WeakPtr()
  {
  }

  public: template <class T2> WeakPtr(const std::weak_ptr<T2> &src) : std::weak_ptr<T>(src)
  {
  }

  public: template <class T2> WeakPtr(const std::shared_ptr<T2> &src) : std::weak_ptr<T>(src)
  {
  }


  //============================================================================
  // Overloaded Operators

  /// @name Copy Operators
  /// @{

  public: template <class T2> WeakPtr<T>& operator=(const std::weak_ptr<T2> &src)
  {
    std::weak_ptr<T>::operator=(src);
    return *this;
  }

  public: template <class T2> WeakPtr<T>& operator=(const std::shared_ptr<T2> &src)
  {
    std::weak_ptr<T>::operator=(src);
    return *this;
  }

  /// @}


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
    return s_cast<TO>(this->lock());
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
    return static_cast<TO*>(this->lock().get());
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
    return r_cast<TO>(this->lock());
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
    return reinterpret_cast<TO*>(this->lock().get());
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
    return c_cast(this->lock());
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
    return const_cast<TO*>(this->lock().get());
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
    return ti_cast<TO>(this->lock());
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
    return Core::Basic::ti_cast<TO>(this->lock().get());
  }

  /// @}

}; // class


//==============================================================================
// Helper Definitions

template <class T, class T2> SharedPtr<T> s_cast(const std::weak_ptr<T2> &src)
{
  return s_cast<T>(src.lock());
}


template <class T, class T2> SharedPtr<T>& r_cast(const std::weak_ptr<T2> &src)
{
  return r_cast<T>(src.lock());
}


template <class T> SharedPtr<T>& c_cast(const std::weak_ptr<T> &src)
{
  return c_cast(src.lock());
}


template <class T, class T2> SharedPtr<T>& ti_cast(const std::weak_ptr<T2> &src)
{
  return ti_cast<T>(src.lock());
}


/**
 * @brief Get a weak pointer to the given TiObject based pointer.
 * The function will get a weak pointer that shares ownership with existing
 * shared pointers to this object. It will automatically cast it to the type of
 * the passed pointer.
 * @param obj A pointer to an object derived from TiObject.
 */
template <class T,
          typename std::enable_if<std::is_base_of<TiObject, T>::value, int>::type = 0>
WeakPtr<T> getWeakPtr(T *obj)
{
  if (obj == 0) {
    return WeakPtr<T>();
  }
  WeakPtr<TiObject> sp = obj->getWeakThis();
  // Since T is derived from TiObject, casting will result in the same
  // pointer value, so a reinterpret cast should be enough; creating a new
  // temporary shared pointer is not necessary.
  return std::move(*(reinterpret_cast<WeakPtr<T>*>(&sp)));
}


template <class T,
          typename std::enable_if<std::is_base_of<TiObject, T>::value, int>::type = 0>
WeakPtr<T const> getWeakPtr(T const *obj)
{
  if (obj == 0) {
    return WeakPtr<T const>();
  }
  WeakPtr<TiObject const> sp = obj->getWeakThis();
  // Since T is derived from TiObject, casting will result in the same
  // pointer value, so a reinterpret cast should be enough; creating a new
  // temporary shared pointer is not necessary.
  return std::move(*(reinterpret_cast<WeakPtr<T const>*>(&sp)));
}


template <class T,
          typename std::enable_if<!std::is_base_of<TiObject, T>::value, int>::type = 0>
WeakPtr<T> getWeakPtr(T *obj)
{
  if (obj == 0) {
    return WeakPtr<T>();
  }
  WeakPtr<TiObject> sp = obj->getTiObject()->getWeakThis();
  // Since T is derived from TiObject, casting will result in the same
  // pointer value, so a reinterpret cast should be enough; creating a new
  // temporary shared pointer is not necessary.
  return std::move(*(reinterpret_cast<WeakPtr<T>*>(&sp)));
}


template <class T,
          typename std::enable_if<!std::is_base_of<TiObject, T>::value, int>::type = 0>
WeakPtr<T const> getWeakPtr(T const *obj)
{
  if (obj == 0) {
    return WeakPtr<T const>();
  }
  WeakPtr<TiObject const> sp = obj->getTiObject()->getWeakThis();
  // Since T is derived from TiObject, casting will result in the same
  // pointer value, so a reinterpret cast should be enough; creating a new
  // temporary shared pointer is not necessary.
  return std::move(*(reinterpret_cast<WeakPtr<T const>*>(&sp)));
}


typedef WeakPtr<TiObject> TioWeakPtr;

} } // namespace

#endif
