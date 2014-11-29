/**
 * @file Core/Data/NamedListContainer.h
 * Contains the header of interface Data::NamedListContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_NAMEDLISTCONTAINER_H
#define DATA_NAMEDLISTCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class NamedListContainer : public virtual ListContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(NamedListContainer, ListContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: using ListContainer::add;

  public: virtual Int add(Char const *name, IdentifiableObject *val) = 0;

  public: using Container::set;

  public: virtual void set(Int index, Char const *name, IdentifiableObject *val) = 0;

  public: virtual const SbStr& getName(Int index) const = 0;

}; // class

} } // namespace

#endif
