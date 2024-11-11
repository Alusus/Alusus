/**
 * @file Spp/Ast/PointerType.cpp
 * Contains the implementation of class Spp::Ast::PointerType.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

Type* PointerType::getContentType(Helper *helper) const
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
    throw EXCEPTION(GenericException, S("Invalid pointer content type found."));
  }
  return type;
}


TypeMatchStatus PointerType::matchTargetType(Type const *type, Helper *helper, TypeMatchOptions opts) const
{
  if (this == type) return TypeMatchStatus::EXACT;

  auto pointerType = ti_cast<PointerType const>(type);
  if (pointerType != 0) {
    Type const *targetContentType = pointerType->getContentType(helper);
    Type const *thisContentType = this->getContentType(helper);
    // We will use AGGREGATION for castings that require no automatic conversion in order to enable more implicit
    // casting in cases of pointers to pointers.
    if (targetContentType == 0 && thisContentType == 0) return TypeMatchStatus::EXACT;
    else if (targetContentType == 0 || helper->isVoid(targetContentType)) return TypeMatchStatus::REF_AGGREGATION;
    else if (thisContentType == 0) return TypeMatchStatus::EXPLICIT_CAST;
    else {
      auto status = thisContentType->matchTargetType(targetContentType, helper);
      if (status == TypeMatchStatus::EXACT) return TypeMatchStatus::EXACT;
      else if (status == TypeMatchStatus::AGGREGATION || status == TypeMatchStatus::REF_AGGREGATION) {
        return TypeMatchStatus::REF_AGGREGATION;
      } else return TypeMatchStatus::EXPLICIT_CAST;
    }
  }

  auto integerType = ti_cast<IntegerType const>(type);
  if (integerType != 0 && integerType->getBitCount(helper) == 0) {
    return TypeMatchStatus::EXPLICIT_CAST;
  }

  return TypeMatchStatus::NONE;
}


Bool PointerType::isIdentical(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  auto pointerType = ti_cast<PointerType const>(type);
  if (pointerType == 0) return false;

  Type const *targetContentType = pointerType->getContentType(helper);
  Type const *thisContentType = this->getContentType(helper);
  if (thisContentType == targetContentType) return true;
  else if (thisContentType != 0 && targetContentType != 0) {
    return thisContentType->isIdentical(targetContentType, helper);
  } else {
    return false;
  }
}

} } // namespace
