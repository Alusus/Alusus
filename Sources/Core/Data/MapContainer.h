/**
 * @file Core/Data/MapContainer.h
 * Contains the header of interface Data::MapContainer.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_MAPCONTAINER_H
#define CORE_DATA_MAPCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class MapContainer : public Container
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(MapContainer, Container, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void set(Int index, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual Int set(Char const *key, TiObject *val)
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual void remove(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual void remove(Char const *key)
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual Word getCount() const
  {
    return 0;
  }

  public: virtual TiObject* get(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }
  public: virtual TiObject* get(Char const *key) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key);
  }

  public: virtual SbStr const& getKey(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index);
  }

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(Char const *key) const
  {
    return -1;
  }

}; // class

} } // namespace

#endif
