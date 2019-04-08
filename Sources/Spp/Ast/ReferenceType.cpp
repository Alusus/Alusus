/**
 * @file Spp/Ast/ReferenceType.cpp
 * Contains the implementation of class Spp::Ast::ReferenceType.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Member Functions

DataType* ReferenceType::getContentType(Helper *helper) const
{
  if (this->contentTypeRef == 0) {
    this->contentTypeRef = helper->getRootManager()->parseExpression(S("type"));
  }
  auto typeBox = ti_cast<TioWeakBox>(
    helper->getSeeker()->doGet(this->contentTypeRef.get(), this->getOwner())
  );
  if (typeBox == 0) return 0;
  auto type = typeBox->get().ti_cast_get<Spp::Ast::DataType>();
  if (type == 0) {
    throw EXCEPTION(GenericException, S("Invalid reference content type found."));
  }
  return type;
}


TypeMatchStatus ReferenceType::matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const
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
    auto status = thisContentType->matchTargetType(targetContentType, helper, ec);
    if (status == TypeMatchStatus::EXACT) return TypeMatchStatus::EXACT;
    else if (status == TypeMatchStatus::AGGREGATION) return TypeMatchStatus::IMPLICIT_CAST;
  }

  auto matchStatus = thisContentType->matchTargetType(type, helper, ec);
  if (matchStatus >= TypeMatchStatus::DEREFERENCE) return TypeMatchStatus::DEREFERENCE;
  else return matchStatus;
}

} // namespace
