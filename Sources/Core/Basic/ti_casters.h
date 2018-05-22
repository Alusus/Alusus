/**
 * @file Core/Basic/ti_casters.h
 * Contains casting functions for TiObject and TiInterface.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TICASTERS_H
#define CORE_BASIC_TICASTERS_H

namespace Core::Basic
{

//==============================================================================
// Casting from TiObject to TiObject

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<DT, ST>::value &&
                                  std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *object)
{
  return static_cast<DT*>(object==0?0:object->isDerivedFrom(DT::getTypeInfo())?object:0);
}

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<DT, ST>::value &&
                                  std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *object)
{
  return static_cast<DT const*>(object==0?0:object->isDerivedFrom(DT::getTypeInfo())?object:0);
}

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<DT, ST>::value &&
                                  std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *object)
{
  return static_cast<DT*>(object==0 ? 0 : object);
}

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<DT, ST>::value &&
                                  std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *object)
{
  return static_cast<DT const*>(object==0? 0 : object);
}


//==============================================================================
// Casting from TiObject to TiInterface

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<TiObject, ST>::value &&
                                  !std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *object)
{
  return object==0 ? 0 : object->template getInterface<DT>();
}

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<TiObject, ST>::value &&
                                  !std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *object)
{
  return object==0 ? 0 : object->template getInterface<DT>();
}


//==============================================================================
// Casting from TiInterface to TiObject

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *interface)
{
  return ti_cast<DT>(interface==0 ? 0 : interface->getTiObject());
}

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *interface)
{
  return ti_cast<DT>(interface==0 ? 0 : interface->getTiObject());
}


//==============================================================================
// Casting from TiInterface to TiInterface

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<TiObject, ST>::value &&
                                  !std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->template getInterface<DT>();
}

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<TiObject, ST>::value &&
                                  !std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->template getInterface<DT>();
}


//==============================================================================
// Obsolete Casting Functions

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
 * @brief Dynamically cast an TiObject to an implemented interface.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject class to dynamically cast a pointer of a base class to
 * one of its derived classes. If the object is not of that type, the result of
 * the function will be 0.
 */
template <class T> inline T* tii_cast(TiObject *object)
{
  return object==0 ? 0 : object->getInterface<T>();
}

/**
 * @brief Dynamically cast an TiInterface to a dervied type.
 * @ingroup basic_utils
 *
 * This inline template function uses the runt-time type info functions of the
 * TiInterface class to dynamically cast a pointer of a base
 * interface to one of its derived interfaces. If the object is not of that type
 * the result of the function will be 0.
 */
template <class T> inline T* tii_cast(TiInterface *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->getInterface<T>();
}


/**
 * @brief Dynamically cast an TiInterface to a derived class.
 * @ingroup basic_utils
 *
 * This inline template function uses the run-time type info functions of the
 * TiObject class to dynamically cast a pointer of an interface to
 * one of the derived classes of its TiObject. This is simply a
 * shortcut to getting the TiObject of that interface and then
 * casting it to the right type. If the object is not of that type, the result
 * of the function will be 0.
 */
template <class T> inline T* tii2tio_cast(TiInterface *interface)
{
  return tio_cast<T>(interface==0 ? 0 : interface->getTiObject());
}

} // namespace

#endif
