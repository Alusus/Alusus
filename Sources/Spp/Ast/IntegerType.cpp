/**
 * @file Spp/Ast/IntegerType.cpp
 * Contains the implementation of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

Word IntegerType::getBitCount(Helper *helper) const
{
  if (this->bitCountRef == 0) {
    this->bitCountRef = helper->getRootManager()->parseExpression(STR("bitCount"));
  }
  auto bitCount = ti_cast<Core::Data::Integer>(
    helper->getSeeker()->doGet(this->bitCountRef.get(), this->getOwner())
  );
  if (bitCount == 0) {
    throw EXCEPTION(GenericException, STR("Could not find bitCount value."));
  }
  return bitCount->get();
}


Bool IntegerType::isImplicitlyCastableTo(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  auto integerType = ti_cast<IntegerType>(type);
  if (integerType != 0 && integerType->getBitCount(helper) >= this->getBitCount(helper)) return true;
  else return false;
}


Bool IntegerType::isExplicitlyCastableTo(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  if (type->isDerivedFrom<IntegerType>() || type->isDerivedFrom<FloatType>()) {
    return true;
  } else if (
    type->isDerivedFrom<PointerType>() &&
    helper->getExecutionContext()->getPointerBitCount() == this->getBitCount(helper)
  ) {
    return true;
  } else {
    return false;
  }
}

} } // namespace
