/**
 * @file Core/Basic/SharedPtr.h
 * Contains definition of Basic::SharedPtr template class.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SHARED_PTR_H
#define BASIC_SHARED_PTR_H

namespace Core { namespace Basic
{

// TODO: DOC

template <class T> class SharedPtr : public std::shared_ptr<T>
{
  //============================================================================
  // Constructors / Destructor

  // TODO: Replace these constructors with constructor inheritance once supported by the compiler.

  public: explicit SharedPtr(T *s = 0) : std::shared_ptr<T>(s)
  {
  }

  public: template <class T2> explicit SharedPtr(T2 *s) : std::shared_ptr<T>(s)
  {
  }

  public: template <class T2> explicit SharedPtr(const std::weak_ptr<T2> &src) : std::shared_ptr<T>(src)
  {
  }

  public: template <class T2> SharedPtr(const std::shared_ptr<T2> &src) : std::shared_ptr<T>(src)
  {
  }

  public: template <class T2> SharedPtr(const std::shared_ptr<T2> &&src) : std::shared_ptr<T>(src)
  {
  }

  public: template <class T2> SharedPtr(const std::shared_ptr<T2> &src, T *p) : std::shared_ptr<T>(src, p)
  {
  }


  //============================================================================
  // Overloaded Operators

  /// @name Copy Operators
  /// @{

  public: template <class T2> SharedPtr<T>& operator=(const std::shared_ptr<T2> &src)
  {
    std::shared_ptr<T>::operator=(src);
    return *this;
  }

  public: template <class T2> SharedPtr<T>& operator=(const std::shared_ptr<T2> &&src)
  {
    std::shared_ptr<T>::operator=(src);
    return *this;
  }

  /// @}

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
  public: template <class T2> Bool operator==(const std::shared_ptr<T2> &ptr) const
  {
    return this->get() == ptr.get();
  }

  /// Check whether the given pointer has a different subject with this one.
  public: template <class T2> Bool operator!=(const std::shared_ptr<T2> &ptr) const
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

  /// @name Dereferencing Operators
  /// @{

  public: T& operator*() const
  {
    return std::shared_ptr<T>::operator*();
  }

  public: T* operator->() const
  {
    return std::shared_ptr<T>::operator->();
  }

  /// @}


  //==============================================================================
  // Member Functions

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
    return SharedPtr<TO>(*this, static_cast<TO*>(this->get()));
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
    return SharedPtr<TO>(*this, reinterpret_cast<TO*>(this->get()));
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
    return SharedPtr<TO>(*this, const_cast<TO*>(this->get()));
  }

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using io_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> io_cast() const
  {
    return SharedPtr<TO>(*this, Core::Basic::io_cast<TO>(this->get()));
  }

  /**
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using ii_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> ii_cast() const
  {
    return SharedPtr<TO>(*this, Core::Basic::ii_cast<TO>(this->get()));
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
   * Casting is done using io_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* io_cast_get() const
  {
    return Core::Basic::io_cast<TO>(this->get());
  }

  /// @}

}; // class


template <class T, class T2> SharedPtr<T> s_cast(const std::shared_ptr<T2> &src)
{
  return SharedPtr<T>(src, static_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T>& r_cast(const std::shared_ptr<T2> &src)
{
  return SharedPtr<T>(src, reinterpret_cast<T*>(src.get()));
}


template <class T> SharedPtr<T>& c_cast(const std::shared_ptr<const T> &src)
{
  return SharedPtr<T>(src, const_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T>& io_cast(const std::shared_ptr<T2> &src)
{
  return SharedPtr<T>(src, Core::Basic::io_cast<T*>(src.get()));
}


template <class T, class T2> SharedPtr<T>& ii_cast(const std::shared_ptr<T2> &src)
{
  return SharedPtr<T>(src, Core::Basic::ii_cast<T*>(src.get()));
}

} } // namespace

#endif
