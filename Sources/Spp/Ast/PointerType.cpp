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
    this->contentTypeRef = helper->getRootManager()->parseExpression(STR("type"));
  }
  auto typeBox = ti_cast<TioWeakBox>(
    helper->getSeeker()->doGet(this->contentTypeRef.get(), this->getOwner())
  );
  if (typeBox == 0) return 0;
  auto type = typeBox->get().ti_cast_get<Spp::Ast::Type>();
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("Invalid pointer content type found."));
  }
  return type;
}


Bool PointerType::isImplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (this == type) return true;

  auto pointerType = ti_cast<PointerType>(type);
  if (pointerType == 0) return false;
  else {
    Type const *contentType = pointerType->getContentType(helper);
    if (contentType == 0) return true;
    if (helper->isVoid(contentType)) return true;
    else {
      Type const *thisContentType = this->getContentType(helper);
      if (thisContentType == 0) return false;
      else return thisContentType == contentType;
      // TODO: Allow for contents of same size and no-op casting.
    }
  }
}


Bool PointerType::isExplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (this == type) return true;

  if (type->isDerivedFrom<PointerType>()) return true;

  if (type->isDerivedFrom<IntegerType>()) {
    auto integerType = static_cast<IntegerType const*>(type);
    if (integerType->getBitCount(helper) == ec->getPointerBitCount()) return true;
  }

  return false;
}

} } // namespace
