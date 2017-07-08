/**
 * @file Spp/Ast/ArrayType.cpp
 * Contains the implementation of class Spp::Ast::ArrayType.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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

Type* ArrayType::getContentType(Core::Standard::RootManager *rootManager) const
{
  if (this->contentTypeRef == 0) {
    this->contentTypeRef = rootManager->parseExpression(STR("type"));
  }
  auto typeBox = ti_cast<Core::Basic::TioSharedBox>(
    rootManager->getSeeker()->doGet(this->contentTypeRef.get(), this->getOwner())
  );
  if (typeBox == 0) return 0;
  auto type = typeBox->get().ti_cast_get<Spp::Ast::Type>();
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("Invalid array content type found."));
  }
  return type;
}


Word ArrayType::getSize(Core::Standard::RootManager *rootManager) const
{
  if (this->sizeRef == 0) {
    this->sizeRef = rootManager->parseExpression(STR("size"));
  }
  auto sizeBox = ti_cast<Core::Basic::TioSharedBox>(
    rootManager->getSeeker()->doGet(this->sizeRef.get(), this->getOwner())
  );
  if (sizeBox == 0) {
    throw EXCEPTION(GenericException, STR("Could not find size value."));
  }
  auto size = sizeBox->get().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
  if (size == 0) {
    throw EXCEPTION(GenericException, STR("Invalid size value found."));
  }
  return std::stoi(size->getValue().get());
}

} } // namespace
