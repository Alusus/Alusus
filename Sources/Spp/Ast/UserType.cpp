/**
 * @file Spp/Ast/UserType.cpp
 * Contains the implementation of class Spp::Ast::UserType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

Bool UserType::merge(TiObject *src, Core::Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(src, noticeStore);
  if (src->isA<Core::Data::Ast::Scope>()) {
    auto scope = static_cast<Core::Data::Ast::Scope*>(src);
    return Core::Data::Ast::addPossiblyMergeableElements(scope, this->getBody().get(), noticeStore);
  }

  if (src->isDerivedFrom<UserType>()) {
    noticeStore->add(
      std::make_shared<Spp::Notices::TypeMergeConflictNotice>(Core::Data::Ast::findSourceLocation(src))
    );
  } else {
    noticeStore->add(
      std::make_shared<Core::Notices::IncompatibleDefMergeNotice>(Core::Data::Ast::findSourceLocation(src))
    );
  }
  return false;
}

} // namespace
