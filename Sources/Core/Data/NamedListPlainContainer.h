/**
 * @file Core/Data/NamedListPlainContainer.h
 * Contains the header of interface Data::NamedListPlainContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_NAMEDLISTPLAINCONTAINER_H
#define DATA_NAMEDLISTPLAINCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class NamedListPlainContainer : public virtual ListPlainContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(NamedListPlainContainer, ListPlainContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: using ListPlainContainer::add;

  public: virtual Int add(Char const *name, IdentifiableObject *val) = 0;

  public: using PlainContainer::set;

  public: virtual void set(Int index, Char const *name, IdentifiableObject *val) = 0;

  public: virtual const SbStr& getName(Int index) const = 0;

}; // class

} } // namespace

#endif
