/**
 * @file Core/Data/NamedListSharedContainer.h
 * Contains the header of interface Data::NamedListSharedContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_NAMEDLISTSHAREDCONTAINER_H
#define DATA_NAMEDLISTSHAREDCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class NamedListSharedContainer : public virtual ListSharedContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(NamedListSharedContainer, ListSharedContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: using ListSharedContainer::add;

  public: virtual Int add(Char const *name, SharedPtr<IdentifiableObject> const &val) = 0;

  public: using SharedContainer::set;

  public: virtual void set(Int index, Char const *name, SharedPtr<IdentifiableObject> const &val) = 0;

  public: virtual const SbStr& getName(Int index) const = 0;

}; // class

} } // namespace

#endif
