/**
 * @file Spp/Ast/UserType.cpp
 * Contains the implementation of class Spp::Ast::UserType.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

TypeMatchStatus UserType::matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const
{
  if (this == type) {
    return TypeMatchStatus::EXACT;
  } else {
    // Check if this is an aggregation, which means the first element is identical to the requested type.
    auto body = this->getBody().get();
    if (body != 0) {
      for (Int i = 0; i < body->getElementCount(); ++i) {
        auto def = ti_cast<Core::Data::Ast::Definition>(body->getElement(i));
        if (def != 0 && !helper->isSharedDef(def)) {
          auto obj = def->getTarget().get();
          if (obj != 0 && helper->isAstReference(obj)) {
            auto memberType = helper->traceType(obj);
            auto memberMatchStatus = memberType->matchTargetType(type, helper, ec);
            if (
              memberMatchStatus == TypeMatchStatus::EXACT || memberMatchStatus == TypeMatchStatus::AGGREGATION ||
              memberMatchStatus == TypeMatchStatus::REF_AGGREGATION
            ) {
              return TypeMatchStatus::AGGREGATION;
            }
          }
        }
      }
    }
    return TypeMatchStatus::NONE;
  }
}


Bool UserType::merge(TiObject *src, Core::Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(src, noticeStore);
  if (src->isA<Core::Data::Ast::Scope>()) {
    auto scope = static_cast<Core::Data::Ast::Scope*>(src);
    return Core::Data::Ast::addPossiblyMergeableElements(scope, this->getBody().get(), noticeStore);
  } else if (src->isDerivedFrom<UserType>()) {
    auto scope = static_cast<UserType*>(src)->getBody().get();
    return Core::Data::Ast::addPossiblyMergeableElements(scope, this->getBody().get(), noticeStore);
  } else {
    noticeStore->add(
      std::make_shared<Core::Notices::IncompatibleDefMergeNotice>(Core::Data::Ast::findSourceLocation(src))
    );
  }
  return false;
}


Bool UserType::hasCustomInitialization(Helper *helper, ExecutionContext const *ec) const
{
  auto body = this->getBody().get();
  if (body != 0) {
    for (Int i = 0; i < body->getCount(); ++i) {
      auto statement = body->getElement(i);
      auto def = ti_cast<Core::Data::Ast::Definition>(statement);
      if (def != 0) {
        if (def->getTarget() != 0) {
          if (def->getTarget().ti_cast_get<Ast::Function>() != 0) {
            if (def->getName() == S("~init")) return true;
          } else if (helper->isAstReference(def->getTarget().get()) && !helper->isSharedDef(def)) {
            auto type = helper->traceType(def->getTarget().get());
            if (type != 0 && type->hasCustomInitialization(helper, ec)) return true;
          }
        }
      } else if (statement != 0) {
        // If we have any non-definition statement then it must be initialization code.
        return true;
      }
    }
  }
  return false;
}


Bool UserType::hasCustomDestruction(Helper *helper, ExecutionContext const *ec) const
{
  auto body = this->getBody().get();
  if (body != 0) {
    for (Int i = 0; i < body->getCount(); ++i) {
      auto statement = body->getElement(i);
      auto def = ti_cast<Core::Data::Ast::Definition>(statement);
      if (def != 0) {
        if (def->getTarget() != 0) {
          if (def->getTarget().ti_cast_get<Ast::Function>() != 0) {
            if (def->getName() == S("~terminate")) return true;
          } else if (helper->isAstReference(def->getTarget().get()) && !helper->isSharedDef(def)) {
            auto type = helper->traceType(def->getTarget().get());
            if (type != 0 && type->hasCustomDestruction(helper, ec)) return true;
          }
        }
      }
    }
  }
  return false;
}

} // namespace
