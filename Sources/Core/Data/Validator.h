/**
 * @file Core/Data/Validator.h
 * Contains the header of class Core::Data::Validator.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_VALIDATOR_H
#define DATA_VALIDATOR_H

namespace Core { namespace Data
{

// TODO: DOC

class Validator : public IdentifiableObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Validator, IdentifiableObject, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Constructor

  protected: Validator()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Bool validate(IdentifiableObject *obj) const = 0;

  public: virtual Bool compare(Validator const *v) const = 0;

}; // class

} } // namespace

#endif
