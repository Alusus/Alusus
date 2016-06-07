/**
 * @file Core/Data/StrAttributeValidator.h
 * Contains the header of class Core::Data::StrAttributeValidator.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_STRATTRIBUTEVALIDATOR_H
#define CORE_DATA_STRATTRIBUTEVALIDATOR_H

namespace Core { namespace Data
{

// TODO: DOC

class StrAttributeValidator : public Validator
{
  //============================================================================
  // Type Info

  TYPE_INFO(StrAttributeValidator, Validator, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str attributeName;

  private: Str attributeValue;


  //============================================================================
  // Constructor

  public: StrAttributeValidator()
  {
  }

  public: StrAttributeValidator(Char const *name, Char const *value) :
    attributeName(name), attributeValue(value)
  {
  }

  public: static SharedPtr<StrAttributeValidator> create()
  {
    return std::make_shared<StrAttributeValidator>();
  }

  public: static SharedPtr<StrAttributeValidator> create(Char const *name, Char const *value)
  {
    return std::make_shared<StrAttributeValidator>(name, value);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *name)
  {
    this->attributeName = name;
  }

  public: void setName(Char const *name, Int length)
  {
    this->attributeName.assign(name, length);
  }

  public: Str const& getName() const
  {
    return this->attributeName;
  }

  public: void setValue(Char const *value)
  {
    this->attributeValue = value;
  }

  public: void setValue(Char const *value, Int length)
  {
    this->attributeValue.assign(value, length);
  }

  public: Str const& getValue() const
  {
    return this->attributeValue;
  }

  public: virtual Bool validate(TiObject *obj) const;

  public: virtual Bool compare(Validator const *v) const;

}; // class

} } // namespace

#endif
