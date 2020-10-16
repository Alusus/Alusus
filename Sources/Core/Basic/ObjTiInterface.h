/**
 * @file Core/Basic/ObjTiInterface.h
 * Contains the header of interface Data::ObjTiInterface.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_OBJTIINTERFACE_H
#define CORE_BASIC_OBJTIINTERFACE_H

namespace Core { namespace Basic
{

class ObjTiInterface : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(ObjTiInterface, TiInterface, "Core.Data", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual ObjInterfaceTypeInfo const* getMyObjInterfaceInfo() const = 0;

}; // class

} } // namespace

#endif
