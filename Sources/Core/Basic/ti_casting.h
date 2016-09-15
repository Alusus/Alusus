/**
 * @file Core/Basic/ti_casting.h
 * Contains casting functions for TiObject and TiInterface.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_TICASTING_H
#define CORE_BASIC_TICASTING_H

namespace Core { namespace Basic
{

//==============================================================================
// Casting from TiObject to TiObject

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT* ti_cast(ST *object)
{
  return static_cast<DT*>(object==0?0:object->isDerivedFrom(DT::getTypeInfo())?object:0);
}

template <class DT, class ST,
          typename std::enable_if<std::is_base_of<TiObject, ST>::value &&
                                  std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* ti_cast(ST const *object)
{
  return static_cast<DT const*>(object==0?0:object->isDerivedFrom(DT::getTypeInfo())?object:0);
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
inline DT* tii_cast(ST *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->template getInterface<DT>();
}

template <class DT, class ST,
          typename std::enable_if<!std::is_base_of<TiObject, ST>::value &&
                                  !std::is_base_of<TiObject, DT>::value, int>::type = 0>
inline DT const* tii_cast(ST const *interface)
{
  return interface==0 ? 0 : interface->getTiObject()->template getInterface<DT>();
}

} } // namespace

#endif
