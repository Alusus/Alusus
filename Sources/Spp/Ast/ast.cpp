/**
 * @file Spp/Ast/ast.cpp
 * Contains the implementations of global functions in the Ast namespace.
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

Type* traceType(TiObject *ref, Core::Data::Seeker *seeker)
{
  Type *result = 0;
  auto *refNode = ti_cast<Core::Data::Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  if (refNode->isDerivedFrom<Type>()) {
    return static_cast<Type*>(refNode);
  }
  seeker->doForeach(refNode, refNode->getOwner(),
    [=, &result](TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      if (obj->isDerivedFrom<Type>()) {
        result = static_cast<Type*>(obj);
        return Core::Data::Seeker::Verb::STOP;
      }
      // TODO: Recurse if the object is an Alias.
      return Core::Data::Seeker::Verb::MOVE;
    }
  );
  if (result == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  return result;
}


Bool isVarDefinition(TiObject *obj)
{
  return
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>();
}


Bool lookupFunction(
  Char const *name, Core::Data::Node *astNode, Core::Basic::Container<Core::Basic::TiObject> *types,
  Spp::ExecutionContext *context, Core::Standard::RootManager *rootManager,
  Core::Processing::NoticeStore *noticeStore, Ast::Function *&function
) {
  Core::Data::Ast::Identifier identifier;
  identifier.setValue(name);
  return lookupFunction(&identifier, astNode, types, context, rootManager, noticeStore, function);
}


Bool lookupFunction(
  Core::Basic::TiObject *ref, Core::Data::Node *astNode, Core::Basic::Container<Core::Basic::TiObject> *types,
  Spp::ExecutionContext *context, Core::Standard::RootManager *rootManager,
  Core::Processing::NoticeStore *noticeStore, Ast::Function *&function
) {
  function = 0;
  using CallMatchStatus = Ast::Function::CallMatchStatus;
  CallMatchStatus matchStatus = CallMatchStatus::NONE;
  Int matchCount = 0;
  SharedPtr<Core::Data::Notice> notice;
  rootManager->getSeeker()->doForeach(ref, astNode->getOwner(),
    [=, &function, &matchStatus, &matchCount, &notice]
      (TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
          auto f = static_cast<Ast::Function*>(obj);
          CallMatchStatus ms;
          ms = f->matchCall(types, context, rootManager);
          if (ms != CallMatchStatus::NONE && matchStatus == CallMatchStatus::NONE) {
            function = f;
            matchStatus = ms;
            matchCount = 1;
          } else if (ms == CallMatchStatus::CASTED && matchStatus == CallMatchStatus::CASTED) {
            matchCount++;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::CASTED) {
            function  = f;
            matchStatus = ms;
            matchCount = 1;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::EXACT) {
            matchCount++;
          }
        }
        return Core::Data::Seeker::Verb::MOVE;
      }
  );
  // Did we have a matched function to call?
  if (notice != 0 && (matchCount > 1 || function == 0)) {
    noticeStore->add(notice);
  }
  if (matchCount > 1) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    noticeStore->add(std::make_shared<MultipleCalleeMatchNotice>(metadata->findSourceLocation()));
    return false;
  } else if (function == 0) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    noticeStore->add(std::make_shared<NoCalleeMatchNotice>(metadata->findSourceLocation()));
    return false;
  }

  return true;
}

} } // namespace
