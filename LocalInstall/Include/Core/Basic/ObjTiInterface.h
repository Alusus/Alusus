/**
 * @file Core/Basic/ObjTiInterface.h
 * Contains the header of interface Data::ObjTiInterface.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  INTERFACE_INFO(ObjTiInterface, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual ObjInterfaceTypeInfo* getMyObjInterfaceInfo() const = 0;

}; // class

} } // namespace

#endif
