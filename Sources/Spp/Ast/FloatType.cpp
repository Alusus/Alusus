/**
 * @file Spp/Ast/FloatType.cpp
 * Contains the implementation of class Spp::Ast::FloatType.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
    this->bitCountRef = helper->getRootManager()->parseExpression(S("bitCount"));
  }
  auto bitCount = ti_cast<TiInt>(
    helper->getSeeker()->doGet(this->bitCountRef.get(), this->getOwner())
  );
  if (bitCount == 0) {
    throw EXCEPTION(GenericException, S("Could not find bitCount value."));
  }
  return bitCount->get();
}


TypeMatchStatus FloatType::matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const
{
  if (this == type) return TypeMatchStatus::EXACT;
  else {
    auto floatType = ti_cast<FloatType const>(type);
    if (floatType != 0) {
      auto thisBitCount = this->getBitCount(helper);
      auto targetBitCount = floatType->getBitCount(helper);
      if (thisBitCount == targetBitCount) return TypeMatchStatus::EXACT;
      else if (targetBitCount > thisBitCount) return TypeMatchStatus::PROMOTION;
      else return TypeMatchStatus::IMPLICIT_CAST;
    } else if (type->isDerivedFrom<IntegerType>()) {
      return TypeMatchStatus::IMPLICIT_CAST;
    } else {
      return TypeMatchStatus::NONE;
    }
  }
}

} } // namespace
