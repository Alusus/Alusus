/**
 * @file Core/Data/IdHolder.h
 * Contains the header of interface Data::IdHolder.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_IDHOLDER_H
#define DATA_IDHOLDER_H

namespace Core { namespace Data
{

// TODO: DOC

class IdHolder : public AttributesHolder
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(IdHolder, AttributesHolder, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Integer id;


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

  public: virtual IdentifiableObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("id")) {
      return &this->id;
    } else {
      return 0;
    }
  }

}; // class

} } // namespace

#endif
