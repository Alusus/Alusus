/**
 * @file Core/Data/ListContainer.h
 * Contains the header of interface Data::ListContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_LISTCONTAINER_H
#define DATA_LISTCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class ListContainer : public virtual Container
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(ListContainer, Container, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual Int add(IdentifiableObject *val) = 0;

}; // class

} } // namespace

#endif
