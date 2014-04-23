/**
 * @file Core/Data/ListSharedContainer.h
 * Contains the header of interface Data::ListSharedContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_LISTSHAREDCONTAINER_H
#define DATA_LISTSHAREDCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class ListSharedContainer : public virtual SharedContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(ListSharedContainer, SharedContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void add(const SharedPtr<IdentifiableObject> &obj) = 0;

}; // class

} } // namespace

#endif
