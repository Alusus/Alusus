/**
 * @file Core/Basic/MapContaining.h
 * Contains the header of interface Basic::MapContaining.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TEMPLATE_INTERFACE_INFO(MapContaining, Containing<CTYPE>, "Core.Data", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual void setElement(Int index, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }
  public: virtual Int setElement(Char const *key, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
  }

  public: virtual void removeElement(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }
  public: virtual Int removeElement(Char const *key)
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
  }

  public: virtual Word getElementCount() const
  {
    return 0;
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }
  public: virtual CTYPE* getElement(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
  }

  public: virtual SbStr const& getElementKey(Int index) const
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
