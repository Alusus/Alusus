/**
 * @file Core/Basic/ListContainer.h
 * Contains the header of interface Basic::ListContainer.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_LISTCONTAINER_H
#define CORE_BASIC_LISTCONTAINER_H

namespace Core { namespace Basic
{

template<class CTYPE> class ListContainer : public virtual Container<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(ListContainer, Container<CTYPE>, "Core.Data", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual Int addElement(CTYPE *val) = 0;

  public: virtual void insertElement(Int index, CTYPE *val) = 0;

}; // class

} } // namespace

#endif
