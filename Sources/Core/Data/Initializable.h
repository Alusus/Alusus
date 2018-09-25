/**
 * @file Core/Data/Initializable.h
 * Contains the header of interface Data::Initializable.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_INITIALIZABLE_H
#define CORE_DATA_INITIALIZABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Initializable : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Initializable, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void initialize(TiObject *context) = 0;

}; // class

} } // namespace

#endif
