/**
 * @file Core/Data/Clonable.h
 * Contains the header of interface Data::Clonable.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_CLONABLE_H
#define DATA_CLONABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Clonable : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Clonable, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual SharedPtr<IdentifiableObject> clone() const = 0;

}; // class

} } // namespace

#endif
