/**
 * @file Core/Basic/MapContaining.h
 * Contains the header of interface Basic::MapContaining.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_MAPCONTAINING_H
#define CORE_BASIC_MAPCONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class MapContaining : public Containing<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(MapContaining, Containing<CTYPE>, "Core.Data", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Abstract Functions

  using Containing<CTYPE>::setElement;
  public: virtual Int setElement(Char const *key, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
  }

  using Containing<CTYPE>::getElement;
  public: virtual CTYPE* getElement(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
  }

  using Containing<CTYPE>::getElementNeededType;
  public: virtual TypeInfo const* getElementNeededType(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Element not found"), key);
  }

  using Containing<CTYPE>::getElementHoldMode;
  public: virtual HoldMode getElementHoldMode(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Element not found"), key);
  }

  public: virtual SbStr const getElementKey(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findElementIndex(Char const *key) const
  {
    return -1;
  }

}; // class

} // namespace

#endif
