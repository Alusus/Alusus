/**
 * @file Spp/Ast/FloatType.cpp
 * Contains the implementation of class Spp::Ast::FloatType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

Word FloatType::getBitCount(Helper *helper) const
{
  if (this->bitCountRef == 0) {
    this->bitCountRef = helper->getRootManager()->parseExpression(STR("bitCount"));
  }
  auto bitCount = ti_cast<TiInt>(
    helper->getSeeker()->doGet(this->bitCountRef.get(), this->getOwner())
  );
  if (bitCount == 0) {
    throw EXCEPTION(GenericException, STR("Could not find bitCount value."));
  }
  return bitCount->get();
}


Bool FloatType::isImplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (this == type) return true;

  auto floatType = ti_cast<FloatType>(type);
  if (floatType != 0 && floatType->getBitCount(helper) >= this->getBitCount(helper)) return true;
  else return false;
}


Bool FloatType::isExplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (this == type) return true;

  if (type->isDerivedFrom<IntegerType>() || type->isDerivedFrom<FloatType>()) return true;
  else return false;
}

} } // namespace
