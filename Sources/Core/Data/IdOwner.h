/**
 * @file Core/Data/IdOwner.h
 * Contains the header of interface Data::IdOwner.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_ID_OWNER_H
#define DATA_ID_OWNER_H

namespace Core { namespace Data
{

// TODO: DOC

class IdOwner : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(IdOwner, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Word id;


  //============================================================================
  // Abstract Functions

  public: virtual void setId(Word id)
  {
    this->id = id;
  }

  public: virtual Word getId() const
  {
    return this->id;
  }

}; // class

} } // namespace

#endif
