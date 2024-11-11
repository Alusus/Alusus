/**
 * @file Spp/Ast/ReferenceType.cpp
 * Contains the implementation of class Spp::Ast::ReferenceType.
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

Type* ReferenceType::getContentType(Helper *helper) const
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
    throw EXCEPTION(GenericException, S("Invalid reference content type found."));
  }
  return type;
}


TypeMatchStatus ReferenceType::matchTargetType(Type const *type, Helper *helper, TypeMatchOptions opts) const
{
  if (this == type) return TypeMatchStatus::EXACT;

  Type const *thisContentType = this->getContentType(helper);
  if (thisContentType == 0) {
    throw EXCEPTION(GenericException, S("Reference type is missing the content type."));
  }

  auto referenceType = ti_cast<ReferenceType const>(type);
  if (referenceType != 0) {
    Type const *targetContentType = referenceType->getContentType(helper);
    if (targetContentType == 0) {
      throw EXCEPTION(GenericException, S("Reference type is missing the content type."));
    }
    auto status = thisContentType->matchTargetType(targetContentType, helper, opts);
    if (status == TypeMatchStatus::AGGREGATION && status.derefs == 0) return TypeMatchStatus::REF_AGGREGATION;
    else if (status >= TypeMatchStatus::REF_AGGREGATION) return status;
  }

  if (opts & TypeMatchOptions::SKIP_DEREF) return TypeMatchStatus::NONE;

  auto matchStatus = this->isAutoDeref() ?
    thisContentType->matchTargetType(type, helper, opts) :
    TypeMatchStatus::NONE;
  if (matchStatus != TypeMatchStatus::NONE) ++matchStatus.derefs;
  else if (referenceType != 0 && !this->isImplicit()) matchStatus = TypeMatchStatus::EXPLICIT_CAST;
  return matchStatus;
}


Bool ReferenceType::isIdentical(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  auto referenceType = ti_cast<ReferenceType const>(type);
  if (referenceType == 0) return false;

  if (this->getMode() != referenceType->getMode()) return false;

  Type const *thisContentType = this->getContentType(helper);
  if (thisContentType == 0) {
    throw EXCEPTION(GenericException, S("Reference type is missing the content type."));
  }

  Type const *targetContentType = referenceType->getContentType(helper);
  if (targetContentType == 0) {
    throw EXCEPTION(GenericException, S("Reference type is missing the content type."));
  }

  return thisContentType->isIdentical(targetContentType, helper);
}

} // namespace
