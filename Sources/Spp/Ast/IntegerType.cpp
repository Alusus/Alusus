/**
 * @file Spp/Ast/IntegerType.cpp
 * Contains the implementation of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
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

Word IntegerType::getBitCount(Helper *helper) const
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
  return std::stol(bitCountText->getValue().get());
}


TypeMatchStatus IntegerType::matchTargetType(Type const *type, Helper *helper, TypeMatchOptions opts) const
{
  if (this == type) return TypeMatchStatus::EXACT;
  else {
    if (type->isDerivedFrom<IntegerType>()) {
      auto integerType = static_cast<IntegerType const*>(type);
      auto thisBitCount = this->getBitCount(helper);
      auto targetBitCount = integerType->getBitCount(helper);

      Bool targetIsLarger;
      if (targetBitCount == 0 && thisBitCount == 0) targetIsLarger = false;
      else if (targetBitCount == 0 && thisBitCount <= 32) targetIsLarger = true;
      else targetIsLarger = targetBitCount > thisBitCount;

      if (thisBitCount == targetBitCount && this->isSigned() == integerType->isSigned()) {
        return TypeMatchStatus::EXACT;
      } else if (targetBitCount == 1 && thisBitCount == 1) {
        return TypeMatchStatus::PROMOTION;
      } else if (integerType->isSigned() == this->isSigned() && targetIsLarger) {
        return TypeMatchStatus::PROMOTION;
      } else if (integerType->isSigned() && targetIsLarger) {
        return TypeMatchStatus::PROMOTION;
      } else if (thisBitCount != 1 && targetBitCount == 1) {
        return TypeMatchStatus::NONE;
      } else {
        return TypeMatchStatus::IMPLICIT_CAST;
      }
    } else if (type->isDerivedFrom<FloatType>()) {
      return TypeMatchStatus::IMPLICIT_CAST;
    } else if (type->isDerivedFrom<PointerType>()) {
      if (this->isNullLiteral()) {
        return TypeMatchStatus::IMPLICIT_CAST;
      } else if (this->getBitCount(helper) == 0) {
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
  auto thisBitCount = this->getBitCount(helper);
  auto targetBitCount = integerType->getBitCount(helper);
  return thisBitCount == targetBitCount && this->isSigned() == integerType->isSigned();
}

} // namespace
