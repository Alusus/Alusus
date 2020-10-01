/**
 * @file Spp/Ast/Module.cpp
 * Contains the implementation of class Spp::Ast::Module.
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

Bool Module::merge(TiObject *src, Core::Data::Seeker *seeker, Core::Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(src, noticeStore);
  if (src->isA<Core::Data::Ast::Scope>() || src->isA<Module>()) {
    auto scope = static_cast<Core::Data::Ast::Scope*>(src);
    return Core::Data::Ast::addPossiblyMergeableElements(scope, this, seeker, noticeStore);
  }
  noticeStore->add(
    newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(Core::Data::Ast::findSourceLocation(src))
  );
  return false;
}

} // namespace
