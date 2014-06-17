/**
 * @file Core/Data/MapSharedContainer.h
 * Contains the header of interface Data::MapSharedContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MAPSHAREDCONTAINER_H
#define DATA_MAPSHAREDCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class MapSharedContainer : public SharedContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(MapSharedContainer, SharedContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: using SharedContainer::set;

  public: virtual Int set(Char const *key, SharedPtr<IdentifiableObject> const &val) = 0;

  public: using SharedContainer::remove;

  public: virtual void remove(Char const *key) = 0;

  public: using SharedContainer::get;

  public: virtual SharedPtr<IdentifiableObject> const& get(Char const *key) const = 0;

  public: virtual const SbStr& getKey(Int index) const = 0;

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(Char const *key) const = 0;

}; // class

} } // namespace

#endif
