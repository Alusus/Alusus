/**
 * @file Core/Basic/WeakPtr.h
 * Contains definition of Basic::WeakPtr template class.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using tio_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> tio_cast() const
  {
    return tio_cast<TO>(this->lock());
  }

  /**
   * @brief Cast this weak pointer to a shared pointer of another data type.
   *
   * Casting is done using tii_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to WeakPtr<My_Type>,
   *            TO must be My_Type, not WeakPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> tii_cast() const
  {
    return tii_cast<TO>(this->lock());
  }

  /// @}

}; // class


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


template <class T, class T2> SharedPtr<T>& tio_cast(const std::weak_ptr<T2> &src)
{
  return tio_cast<T>(src.lock());
}


template <class T, class T2> SharedPtr<T>& tii_cast(const std::weak_ptr<T2> &src)
{
  return tii_cast<T>(src.lock());
}

} } // namespace

#endif
