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

#include <memory>

namespace Core { namespace Basic
{

// TODO: DOC

template <class T> class SharedPtr : public std::shared_ptr<T>
{
  // Inherit constructors.
  using std::shared_ptr<T>::shared_ptr;


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
  public: template <class TO> inline SharedPtr<TO> s_cast() const &
  {
    return SharedPtr<TO>(*this, static_cast<TO*>(this->get()));
  }

  /// @sa s_cast()
  public: template <class TO> inline SharedPtr<TO> s_cast() &&
  {
    TO *p = static_cast<TO*>(this->get());
    if (reinterpret_cast<PtrInt>(p) == reinterpret_cast<PtrInt>(this->get())) {
      return std::move(*(reinterpret_cast<SharedPtr<TO>*>(this)));
    } else {
      return SharedPtr<TO>(*this, p);
    }
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
  public: template <class TO> inline SharedPtr<TO> r_cast() const &
  {
    return SharedPtr<TO>(*this, reinterpret_cast<TO*>(this->get()));
  }

  /// @sa r_cast()
  public: template <class TO> inline SharedPtr<TO> r_cast() &&
  {
    return std::move(*(reinterpret_cast<SharedPtr<TO>*>(this)));
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
  public: template <class TO> inline SharedPtr<TO> c_cast() const &
  {
    return SharedPtr<TO>(*this, const_cast<TO*>(this->get()));
  }

  /// @sa c_cast()
  public: template <class TO> inline SharedPtr<TO> c_cast() &&
  {
    return std::move(*(reinterpret_cast<SharedPtr<TO>*>(this)));
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
  public: template <class TO> inline SharedPtr<TO> io_cast() const &
  {
    return SharedPtr<TO>(*this, Core::Basic::io_cast<TO>(this->get()));
  }

  /// @sa io_cast()
  public: template <class TO> inline SharedPtr<TO> io_cast() &&
  {
    if (this->get() != 0 && this->get()->isDerivedFrom(TO::getTypeInfo())) {
      return std::move(*(reinterpret_cast<SharedPtr<TO>*>(this)));
    } else {
      return SharedPtr<TO>();
    }
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
   * @brief Cast this shared pointer to a shared pointer of another data type.
   *
   * Casting is done using ii2io_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to SharedPtr<My_Type>,
   *            TO must be My_Type, not SharedPtr<My_Type>.
   */
  public: template <class TO> inline SharedPtr<TO> ii2io_cast() const
  {
    return SharedPtr<TO>(*this, Core::Basic::ii2io_cast<TO>(this->get()));
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

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using ii_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* ii_cast_get() const
  {
    return Core::Basic::ii_cast<TO>(this->get());
  }

  /**
   * @brief Cast and return the subject of this shared pointer.
   *
   * Casting is done using ii2io_cast.
   *
   * @tparam TO The requested type of the subject, not the pointer. In other
   *            words, if you want to cast the pointer to My_Type,
   *            TO must be My_Type, not My_Type*.
   */
  public: template <class TO> inline TO* ii2io_cast_get() const
  {
    return Core::Basic::ii2io_cast<TO>(this->get());
  }

  /// @}

}; // class


template <class T, class T2> inline SharedPtr<T> s_cast(std::shared_ptr<T2> const &src)
{
  return SharedPtr<T>(src, static_cast<T*>(src.get()));
}


template <class T, class T2> inline SharedPtr<T> s_cast(std::shared_ptr<T2> &&src)
{
  T *p = static_cast<T*>(src.get());
  if (reinterpret_cast<PtrInt>(p) == reinterpret_cast<PtrInt>(src.get())) {
    return std::move(*(reinterpret_cast<SharedPtr<T>*>(&src)));
  } else {
    return SharedPtr<T>(src, p);
  }
}


template <class T, class T2> inline SharedPtr<T> r_cast(std::shared_ptr<T2> const &src)
{
  return SharedPtr<T>(src, reinterpret_cast<T*>(src.get()));
}


template <class T, class T2> inline SharedPtr<T> r_cast(std::shared_ptr<T2> &&src)
{
  return std::move(*(reinterpret_cast<SharedPtr<T>*>(&src)));
}


template <class T> inline SharedPtr<T> c_cast(std::shared_ptr<const T> const &src)
{
  return SharedPtr<T>(src, const_cast<T*>(src.get()));
}


template <class T> inline SharedPtr<T> c_cast(std::shared_ptr<const T> &&src)
{
  return std::move(*(reinterpret_cast<SharedPtr<T>*>(&src)));
}


template <class T, class T2> inline SharedPtr<T> io_cast(std::shared_ptr<T2> const &src)
{
  return SharedPtr<T>(src, Core::Basic::io_cast<T>(src.get()));
}


template <class T, class T2> inline SharedPtr<T> io_cast(std::shared_ptr<T2> &&src)
{
  if (src.get() != 0 && src.get()->isDerivedFrom(T::getTypeInfo())) {
    return std::move(*(reinterpret_cast<SharedPtr<T>*>(&src)));
  } else {
    return SharedPtr<T>();
  }
}


template <class T, class T2> SharedPtr<T> ii_cast(std::shared_ptr<T2> const &src)
{
  return SharedPtr<T>(src, Core::Basic::ii_cast<T*>(src.get()));
}

template <class T, class T2> SharedPtr<T> ii2io_cast(std::shared_ptr<T2> const &src)
{
  return SharedPtr<T>(src, Core::Basic::ii2io_cast<T*>(src.get()));
}

/**
 * @brief Get a shared pointer to the given IdentifiableObject based pointer.
 * The function will get a shared pointer that shares ownership with existing
 * shared pointers to this object. It will automatically cast it to the type of
 * the passed pointer.
 * @param obj A pointer to an object derived from IdentifiableObject.
 * @param ownIfUnowned Specify whether to own this object if it's not already
 *                     owned. If true and no shared pointer owns this object,
 *                     the function will create a new shared pointer to own
 *                     this, otherwise it will return an empty pointer.
 */
template <class T> SharedPtr<T> getSharedPtr(T *obj, Bool ownIfUnowned=false)
{
  if (obj == 0) {
    return SharedPtr<T>();
  }
  SharedPtr<IdentifiableObject> sp = obj->getSharedThis();
  if (sp == 0) {
    if (ownIfUnowned) return SharedPtr<T>(obj);
    else return SharedPtr<T>();
  }
  // Since T is derived from IdentifiableObject, casting will result in the same
  // pointer value, so a reinterpret cast should be enough; creating a new
  // temporary shared pointer is not necessary.
  return std::move(*(reinterpret_cast<SharedPtr<T>*>(&sp)));
}

} } // namespace

#endif
