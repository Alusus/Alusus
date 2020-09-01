/**
 * @file Spp/Ast/UserType.cpp
 * Contains the implementation of class Spp::Ast::UserType.
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

TypeMatchStatus UserType::matchTargetType(
  Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts
) const
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
            auto memberMatchStatus = memberType->matchTargetType(type, helper, ec, opts | TypeMatchOptions::SKIP_DEREF);
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
      newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(Core::Data::Ast::findSourceLocation(src))
    );
  }
  return false;
}


TypeInitMethod UserType::getInitializationMethod(Helper *helper, ExecutionContext const *ec) const
{
  TypeInitMethod method = TypeInitMethod::NONE;
  auto body = this->getBody().get();
  if (body != 0) {
    for (Int i = 0; i < body->getCount(); ++i) {
      auto statement = body->getElement(i);
      auto def = ti_cast<Core::Data::Ast::Definition>(statement);
      if (def != 0) {
        if (def->getTarget() != 0) {
          if (def->getTarget().ti_cast_get<Ast::Function>() != 0) {
            if (def->getName() == S("~init")) {
              method |= TypeInitMethod::USER;
              if (method == TypeInitMethod::BOTH) break;
            }
          } else if (helper->isAstReference(def->getTarget().get()) && !helper->isSharedDef(def)) {
            auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(def->getTarget().get());
            if (paramPass != 0 && paramPass->getType() == Core::Data::Ast::BracketType::ROUND) {
              // If there are args passed to the definition then it's an AUTO init method even if the object type
              // is a primary data type.
              method |= TypeInitMethod::AUTO;
              if (method == TypeInitMethod::BOTH) break;
            } else {
              auto type = helper->traceType(def->getTarget().get());
              if (type != 0 && type->getInitializationMethod(helper, ec) != TypeInitMethod::NONE) {
                method |= TypeInitMethod::AUTO;
                if (method == TypeInitMethod::BOTH) break;
              }
            }
          }
        }
      } else if (statement != 0) {
        // If we have any non-definition statement then it must be initialization code.
        method |= TypeInitMethod::AUTO;
        if (method == TypeInitMethod::BOTH) break;
      }
    }
  }
  return method;
}


TypeInitMethod UserType::getDestructionMethod(Helper *helper, ExecutionContext const *ec) const
{
  TypeInitMethod method = TypeInitMethod::NONE;
  auto body = this->getBody().get();
  if (body != 0) {
    for (Int i = 0; i < body->getCount(); ++i) {
      auto statement = body->getElement(i);
      auto def = ti_cast<Core::Data::Ast::Definition>(statement);
      if (def != 0) {
        if (def->getTarget() != 0) {
          if (def->getTarget().ti_cast_get<Ast::Function>() != 0) {
            if (def->getName() == S("~terminate")) {
              method |= TypeInitMethod::USER;
              if (method == TypeInitMethod::BOTH) break;
            }
          } else if (helper->isAstReference(def->getTarget().get()) && !helper->isSharedDef(def)) {
            auto type = helper->traceType(def->getTarget().get());
            if (type != 0 && type->getDestructionMethod(helper, ec) != TypeInitMethod::NONE) {
              method |= TypeInitMethod::AUTO;
              if (method == TypeInitMethod::BOTH) break;
            }
          }
        }
      }
    }
  }
  return method;
}

} // namespace
