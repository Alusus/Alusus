/**
 * @file Spp/Ast/IntegerType.cpp
 * Contains the implementation of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Member Functions

Word IntegerType::getBitCount(Helper *helper, ExecutionContext const *ec) const
{
  if (this->isNullLiteral()) return 1;

  static TioSharedPtr bitCountRef;
  if (bitCountRef == 0) {
    bitCountRef = helper->getRootManager()->parseExpression(S("bitCount"));
  }
  auto bitCountText = ti_cast<Core::Data::Ast::IntegerLiteral>(
    helper->getSeeker()->doGet(bitCountRef.get(), this->getOwner())
  );
  if (bitCountText == 0) {
    throw EXCEPTION(GenericException, S("Could not find bitCount value."));
  }
  Word bitCount = std::stol(bitCountText->getValue().get());
  if (bitCount == 0) {
    return ec->getPointerBitCount();
  } else {
    return bitCount;
  }
}


TypeMatchStatus IntegerType::matchTargetType(
  Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts
) const
{
  if (this == type) return TypeMatchStatus::EXACT;
  else {
    if (type->isDerivedFrom<IntegerType>()) {
      auto integerType = static_cast<IntegerType const*>(type);
      auto thisBitCount = this->getBitCount(helper, ec);
      auto targetBitCount = integerType->getBitCount(helper, ec);
      if (thisBitCount == targetBitCount && this->isSigned() == integerType->isSigned()) {
        return TypeMatchStatus::EXACT;
      } else if (targetBitCount == 1 && thisBitCount == 1) {
        return TypeMatchStatus::PROMOTION;
      } else if (integerType->isSigned() == this->isSigned() && targetBitCount > thisBitCount) {
        return TypeMatchStatus::PROMOTION;
      } else if (integerType->isSigned() && targetBitCount > thisBitCount) {
        return TypeMatchStatus::PROMOTION;
      } else if (thisBitCount > 1 && targetBitCount == 1) {
        return TypeMatchStatus::NONE;
      } else {
        return TypeMatchStatus::IMPLICIT_CAST;
      }
    } else if (type->isDerivedFrom<FloatType>()) {
      return TypeMatchStatus::IMPLICIT_CAST;
    } else if (type->isDerivedFrom<PointerType>()) {
      if (this->isNullLiteral()) {
        return TypeMatchStatus::IMPLICIT_CAST;
      } else if (ec->getPointerBitCount() == this->getBitCount(helper, ec)) {
        return TypeMatchStatus::EXPLICIT_CAST;
      } else {
        return TypeMatchStatus::NONE;
      }
    } else {
      return TypeMatchStatus::NONE;
    }
  }
}


Bool IntegerType::isIdentical(Type const *type, Helper *helper) const
{
  if (this == type) return true;
  auto integerType = ti_cast<IntegerType const>(type);
  if (integerType == 0) return false;
  // We want the bitcount to match regardless of the execution context. A bit count of 0 means we'll get the bit
  // count from the execution context. In this case we want to actually compare the zero itself rather than what's
  // coming from the execution context. In order to avoid code duplication we'll simply set the execution context
  // pointer bit count to 0 so that if the type bit count is 0 we get the 0 back.
  static ExecutionContext ec(0);
  auto thisBitCount = this->getBitCount(helper, &ec);
  auto targetBitCount = integerType->getBitCount(helper, &ec);
  return thisBitCount == targetBitCount && this->isSigned() == integerType->isSigned();
}

} // namespace
