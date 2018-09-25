/**
 * @file Core/Data/Clonable.h
 * Contains the header of interface Data::Clonable.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CLONABLE_H
#define CORE_DATA_CLONABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Clonable : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Clonable, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual SharedPtr<TiObject> clone() const = 0;

}; // class

} } // namespace

#endif
