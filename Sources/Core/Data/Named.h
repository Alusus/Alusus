/**
 * @file Core/Data/Named.h
 * Contains the header of interface Data::Named.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_NAMED_H
#define CORE_DATA_NAMED_H

namespace Core { namespace Data
{

class Named : public AttributesHolder
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Named, AttributesHolder, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void setName(Char const *name) = 0;

  public: virtual String* getName() const = 0;

  public: virtual TiObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("name")) {
      return this->getName();
    } else {
      return 0;
    }
  }

}; // class

} } // namespace

#endif
