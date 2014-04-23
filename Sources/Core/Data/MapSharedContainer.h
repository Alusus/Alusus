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

  public: virtual Int set(const Char *key, const SharedPtr<IdentifiableObject> &val) = 0;

  public: using SharedContainer::remove;

  public: virtual void remove(const Char *key) = 0;

  public: using SharedContainer::get;

  public: virtual const SharedPtr<IdentifiableObject>& get(const Char *key) const = 0;

  public: virtual const Str& getKey(Int index) const = 0;

  /**
   * @brief Find the index of a specified key.
   * @return The index of the key, or -1 if the key doesn't exist.
   */
  public: virtual Int findIndex(const Char *key) const = 0;

}; // class

} } // namespace

#endif
