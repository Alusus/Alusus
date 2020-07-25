/**
 * @file Spp/Ast/ArrayType.cpp
 * Contains the implementation of class Spp::Ast::ArrayType.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
  if (this->contentTypeRef == 0) {
    this->contentTypeRef = helper->getRootManager()->parseExpression(S("type"));
  }
  auto typeBox = ti_cast<TioWeakBox>(
    helper->getSeeker()->doGet(this->contentTypeRef.get(), this->getOwner())
  );
  if (typeBox == 0) return 0;
  auto type = typeBox->get().ti_cast_get<Spp::Ast::Type>();
  if (type == 0) {
    throw EXCEPTION(GenericException, S("Invalid array content type found."));
  }
  return type;
}


Word ArrayType::getSize(Helper *helper) const
{
  if (this->sizeRef == 0) {
    this->sizeRef = helper->getRootManager()->parseExpression(S("size"));
  }
  auto size = ti_cast<Core::Data::Ast::IntegerLiteral>(
    helper->getSeeker()->doGet(this->sizeRef.get(), this->getOwner())
  );
  if (size == 0) {
    throw EXCEPTION(GenericException, S("Could not find size value."));
  }
  return std::stol(size->getValue().get());
}


TypeMatchStatus ArrayType::matchTargetType(
  Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts
) const
{
  if (this == type) return TypeMatchStatus::EXACT;

  auto arrayType = ti_cast<ArrayType const>(type);
  if (arrayType == 0) {
    auto thisContentType = this->getContentType(helper);
    if (thisContentType->isEqual(type, helper, ec)) return TypeMatchStatus::AGGREGATION;
    else return TypeMatchStatus::NONE;
  } else {
    auto thisContentType = this->getContentType(helper);
    auto targetContentType = arrayType->getContentType(helper);
    if (thisContentType->isEqual(targetContentType, helper, ec)) {
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

} // namespace
