/**
 * @file Spp/Ast/PointerType.cpp
 * Contains the implementation of class Spp::Ast::PointerType.
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

Type* PointerType::getContentType(Helper *helper) const
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
    throw EXCEPTION(GenericException, S("Invalid pointer content type found."));
  }
  return type;
}


TypeMatchStatus PointerType::matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const
{
  if (this == type) return TypeMatchStatus::EXACT;

  auto pointerType = ti_cast<PointerType const>(type);
  if (pointerType != 0) {
    Type const *targetContentType = pointerType->getContentType(helper);
    Type const *thisContentType = this->getContentType(helper);
    // We will use AGGREGATION for castings that require no automatic conversion in order to enable more implicit
    // casting in cases of pointers to pointers.
    if (targetContentType == 0 && thisContentType == 0) return TypeMatchStatus::EXACT;
    else if (targetContentType == 0 || helper->isVoid(targetContentType)) return TypeMatchStatus::AGGREGATION;
    else if (thisContentType == 0) return TypeMatchStatus::EXPLICIT_CAST;
    else {
      auto status = thisContentType->matchTargetType(targetContentType, helper, ec);
      if (status == TypeMatchStatus::EXACT) return TypeMatchStatus::EXACT;
      else if (status == TypeMatchStatus::AGGREGATION) return TypeMatchStatus::AGGREGATION;
      else return TypeMatchStatus::EXPLICIT_CAST;
    }
  }

  auto integerType = ti_cast<IntegerType const>(type);
  if (integerType != 0 && integerType->getBitCount(helper) == ec->getPointerBitCount()) {
    return TypeMatchStatus::EXPLICIT_CAST;
  }

  return TypeMatchStatus::NONE;
}

} } // namespace
