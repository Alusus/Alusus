/**
 * @file Core/Data/MapContainer.h
 * Contains the header of interface Data::MapContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MAPCONTAINER_H
#define DATA_MAPCONTAINER_H

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

  public: using Container::set;

  public: virtual Int set(Char const *key, IdentifiableObject *val) = 0;

  public: using Container::remove;

  public: virtual void remove(Char const *key) = 0;

  public: using Container::get;

  public: virtual IdentifiableObject* get(Char const *key) const = 0;

  public: virtual const SbStr& getKey(Int index) const = 0;

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(Char const *key) const = 0;

}; // class

} } // namespace

#endif
