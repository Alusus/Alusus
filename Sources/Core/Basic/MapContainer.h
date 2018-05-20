/**
 * @file Core/Basic/MapContainer.h
 * Contains the header of interface Basic::MapContainer.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_MAPCONTAINER_H
#define CORE_BASIC_MAPCONTAINER_H

namespace Core { namespace Basic
{

template<class CTYPE> class MapContainer : public Container<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(MapContainer, Container<CTYPE>, "Core.Data", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual void setElement(Int index, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual Int setElement(Char const *key, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual void removeElement(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual Int removeElement(Char const *key)
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual Word getElementCount() const
  {
    return 0;
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual CTYPE* getElement(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual SbStr const& getElementKey(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
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

} } // namespace

#endif
