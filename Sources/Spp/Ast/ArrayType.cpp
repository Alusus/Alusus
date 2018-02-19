/**
 * @file Spp/Ast/ArrayType.cpp
 * Contains the implementation of class Spp::Ast::ArrayType.
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

Type* ArrayType::getContentType(Helper *helper) const
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
    throw EXCEPTION(GenericException, STR("Invalid array content type found."));
  }
  return type;
}


Word ArrayType::getSize(Helper *helper) const
{
  if (this->sizeRef == 0) {
    this->sizeRef = helper->getRootManager()->parseExpression(STR("size"));
  }
  auto size = ti_cast<Core::Data::Integer>(
    helper->getSeeker()->doGet(this->sizeRef.get(), this->getOwner())
  );
  if (size == 0) {
    throw EXCEPTION(GenericException, STR("Could not find size value."));
  }
  return size->get();
}

} } // namespace
