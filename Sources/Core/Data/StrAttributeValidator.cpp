/**
 * @file Core/Data/StrAttributeValidator.cpp
 * Contains the implementation of class Core::Data::StrAttributeValidator.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Member Functions

Bool StrAttributeValidator::validate(IdentifiableObject *r) const
{
  if (r == 0) return false;
  AttributesHolder *ah = ii_cast<AttributesHolder>(r);
  if (ah == 0) return false;
  IdentifiableObject *val = ah->getAttribute(this->attributeName.c_str());
  if (val == 0) return false;
  if (val->isA<String>()) {
    return static_cast<String*>(val)->getStr() == this->attributeValue;
  } else if (val->isA<Integer>()) {
    Word num = static_cast<Integer*>(val)->getUnsigned();
    if (ID_GENERATOR->isDefined(num)) return ID_GENERATOR->getDesc(num) == this->attributeValue;
    else return false;
  } else {
    return false;
  }
}


Bool StrAttributeValidator::compare(Validator const *v) const
{
  const StrAttributeValidator *sav = io_cast<StrAttributeValidator>(v);
  if (sav != 0 && this->getName() == sav->getName() && this->getValue() == sav->getValue()) return true;
  else return false;
}

} } // namespace
