/**
 * @file Spp/Ast/ArrayType.cpp
 * Contains the implementation of class Spp::Ast::ArrayType.
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

Type* ArrayType::getContentType(Helper *helper) const
{
  static TioSharedPtr contentTypeRef;
  if (contentTypeRef == 0) {
    contentTypeRef = helper->getRootManager()->parseExpression(S("type"));
  }
  auto typePassage = ti_cast<Core::Data::Ast::Passage>(
    helper->getSeeker()->doGet(contentTypeRef.get(), this->getOwner())
  );
  if (typePassage == 0) return 0;
  auto type = ti_cast<Spp::Ast::Type>(typePassage->get());
  if (type == 0) {
    throw EXCEPTION(GenericException, S("Invalid array content type found."));
  }
  return type;
}


Word ArrayType::getSize(Helper *helper) const
{
  static TioSharedPtr sizeRef;
  if (sizeRef == 0) {
    sizeRef = helper->getRootManager()->parseExpression(S("size"));
  }
  auto size = ti_cast<Core::Data::Ast::IntegerLiteral>(
    helper->getSeeker()->doGet(sizeRef.get(), this->getOwner())
  );
  if (size == 0) {
    throw EXCEPTION(GenericException, S("Could not find size value."));
  }
  return std::stol(size->getValue().get());
}


TypeMatchStatus ArrayType::matchTargetType(
  Type const *type, Helper *helper, TypeMatchOptions opts
) const
{
  if (this == type) return TypeMatchStatus::EXACT;

  auto arrayType = ti_cast<ArrayType const>(type);
  if (arrayType == 0) {
    auto thisContentType = this->getContentType(helper);
    if (thisContentType->isEqual(type, helper)) return TypeMatchStatus::AGGREGATION;
    else return TypeMatchStatus::NONE;
  } else {
    auto thisContentType = this->getContentType(helper);
    auto targetContentType = arrayType->getContentType(helper);
    if (thisContentType->isEqual(targetContentType, helper)) {
      auto thisSize = this->getSize(helper);
      auto targetSize = arrayType->getSize(helper);
      return thisSize == targetSize ? TypeMatchStatus::EXACT : (
        thisSize < targetSize ? TypeMatchStatus::PROMOTION : TypeMatchStatus::AGGREGATION
      );
    } else {
      return TypeMatchStatus::NONE;
    }
  }
}


Bool ArrayType::isIdentical(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  auto arrayType = ti_cast<ArrayType const>(type);
  if (arrayType == 0) return false;

  auto thisContentType = this->getContentType(helper);
  auto targetContentType = arrayType->getContentType(helper);
  auto thisSize = this->getSize(helper);
  auto targetSize = arrayType->getSize(helper);

  return thisSize == targetSize && thisContentType->isIdentical(targetContentType, helper);
}

} // namespace
