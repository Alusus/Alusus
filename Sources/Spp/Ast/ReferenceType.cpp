/**
 * @file Spp/Ast/ReferenceType.cpp
 * Contains the implementation of class Spp::Ast::ReferenceType.
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

Type* ReferenceType::getContentType(Helper *helper) const
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
    throw EXCEPTION(GenericException, S("Invalid reference content type found."));
  }
  return type;
}


TypeMatchStatus ReferenceType::matchTargetType(
  Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts
) const
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
    auto status = thisContentType->matchTargetType(targetContentType, helper, ec, opts);
    if (status == TypeMatchStatus::AGGREGATION && status.derefs == 0) return TypeMatchStatus::REF_AGGREGATION;
    else if (status >= TypeMatchStatus::REF_AGGREGATION) return status;
  }

  if (opts & TypeMatchOptions::SKIP_DEREF) return TypeMatchStatus::NONE;

  auto matchStatus = thisContentType->matchTargetType(type, helper, ec, opts);
  ++matchStatus.derefs;
  return matchStatus;
}

} // namespace
