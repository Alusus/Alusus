/**
 * @file Spp/Ast/ReferenceType.cpp
 * Contains the implementation of class Spp::Ast::ReferenceType.
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

Type* ReferenceType::getContentType(Helper *helper) const
{
  if (this->contentTypeRef == 0) {
    this->contentTypeRef = helper->getRootManager()->parseExpression(STR("type"));
  }
  auto typeBox = ti_cast<Core::Basic::TioWeakBox>(
    helper->getSeeker()->doGet(this->contentTypeRef.get(), this->getOwner())
  );
  if (typeBox == 0) return 0;
  auto type = typeBox->get().ti_cast_get<Spp::Ast::Type>();
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("Invalid reference content type found."));
  }
  return type;
}


Bool ReferenceType::isImplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (type == this) return true;

  auto contentType = this->getContentType(helper);
  return contentType->isImplicitlyCastableTo(type, helper, ec);
}


Bool ReferenceType::isExplicitlyCastableTo(Type const *type, Helper *helper, Spp::ExecutionContext const *ec) const
{
  if (type == this) return true;

  auto contentType = this->getContentType(helper);
  return contentType->isExplicitlyCastableTo(type, helper, ec);
}

} } // namespace
