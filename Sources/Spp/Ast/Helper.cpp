/**
 * @file Spp/Ast/Helper.cpp
 * Contains the implementation of class Spp::Ast::Helper.
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
// Initialization Functions

void Helper::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->isVarDefinition,
    &this->lookupCallee,
    &this->lookupCallee_iteration,
    &this->traceType,
    &this->isVoid,
    &this->isImplicitlyCastableTo,
    &this->isReferenceTypeFor,
    &this->getReferenceTypeFor,
    &this->getBoolType,
    &this->getInt64Type
  });
}


void Helper::initBindings()
{
  this->isVarDefinition = &Helper::_isVarDefinition;
  this->lookupCallee = &Helper::_lookupCallee;
  this->lookupCallee_iteration = &Helper::_lookupCallee_iteration;
  this->traceType = &Helper::_traceType;
  this->isVoid = &Helper::_isVoid;
  this->isImplicitlyCastableTo = &Helper::_isImplicitlyCastableTo;
  this->isReferenceTypeFor = &Helper::_isReferenceTypeFor;
  this->getReferenceTypeFor = &Helper::_getReferenceTypeFor;
  this->getBoolType = &Helper::_getBoolType;
  this->getInt64Type = &Helper::_getInt64Type;
}


//==============================================================================
// Main Functions

Bool Helper::_isVarDefinition(TiObject *self, TiObject *obj)
{
  return
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>();
}


Bool Helper::lookupCalleeByName(
  Char const *name, Core::Data::Node *astNode, Core::Basic::Container<TiObject> *types,
  TiObject *&callee, Type *&calleeType
) {
  Core::Data::Ast::Identifier identifier;
  identifier.setValue(name);
  return this->lookupCallee(&identifier, astNode, types, callee, calleeType);
}


Bool Helper::_lookupCallee(
  TiObject *self, TiObject *ref, Core::Data::Node *astNode, Core::Basic::Container<TiObject> *types,
  TiObject *&callee, Type *&calleeType
) {
  PREPARE_SELF(helper, Helper);

  callee = 0;
  calleeType = 0;
  CallMatchStatus matchStatus = CallMatchStatus::NONE;
  Int matchCount = 0;
  SharedPtr<Core::Data::Notice> notice;
  Bool symbolFound = false;
  helper->getSeeker()->doForeach(ref, astNode->getOwner(),
    [=, &callee, &calleeType, &matchStatus, &matchCount, &notice, &symbolFound]
      (TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        symbolFound = true;

        return helper->lookupCallee_iteration(obj, types, matchStatus, matchCount, notice, callee, calleeType);
      }
  );
  // Did we have a matched callee?
  if (notice != 0 && (matchCount > 1 || callee == 0)) {
    helper->noticeStore->add(notice);
  }
  if (matchCount > 1) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    helper->noticeStore->add(std::make_shared<MultipleCalleeMatchNotice>(metadata->findSourceLocation()));
    return false;
  } else if (callee == 0) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(astNode);
    if (symbolFound) {
      helper->noticeStore->add(std::make_shared<NoCalleeMatchNotice>(metadata->findSourceLocation()));
    } else {
      helper->noticeStore->add(std::make_shared<UnknownSymbolNotice>(metadata->findSourceLocation()));
    }
    return false;
  }

  return true;
}


Core::Data::Seeker::Verb Helper::_lookupCallee_iteration(
  TiObject *self, TiObject *obj, Core::Basic::Container<TiObject> *types,
  CallMatchStatus &matchStatus, Int &matchCount, SharedPtr<Core::Data::Notice> &notice,
  TiObject *&callee, Type *&calleeType
) {
  PREPARE_SELF(helper, Helper);

  if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
    auto f = static_cast<Ast::Function*>(obj);
    CallMatchStatus ms;
    ms = f->matchCall(types, helper);
    if (ms == CallMatchStatus::NONE) {
      return Core::Data::Seeker::Verb::MOVE;
    } else if (matchStatus == CallMatchStatus::NONE) {
      callee = f;
      matchStatus = ms;
      matchCount = 1;
    } else if (ms == matchStatus) {
      matchCount++;
    } else if (ms > matchStatus) {
      callee  = f;
      matchStatus = ms;
      matchCount = 1;
    }
    return Core::Data::Seeker::Verb::MOVE;
  } else if (obj != 0 && helper->isVarDefinition(obj)) {
    auto objType = helper->getSeeker()->tryGet(obj, obj);
    if (objType == 0) {
      notice = std::make_shared<InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(obj));
    }
    if (objType->isDerivedFrom<ArrayType>()) {
      if (
        types->getElementCount() == 1 &&
        helper->isImplicitlyCastableTo(types->getElement(0), helper->getInt64Type())
      ) {
        callee = obj;
        calleeType = static_cast<Type*>(objType);
        matchCount = 1;
        matchStatus = CallMatchStatus::EXACT;
      } else {
        // TODO: Raise an error.
        matchCount = 0;
        matchStatus = CallMatchStatus::NONE;
      }
    } else {
      // TODO: Raise an error.
      matchCount = 0;
      matchStatus = CallMatchStatus::NONE;
    }
    return Core::Data::Seeker::Verb::STOP;
  }
  return Core::Data::Seeker::Verb::MOVE;
}


Type* Helper::_traceType(TiObject *self, TiObject *ref)
{
  PREPARE_SELF(helper, Helper);

  SharedPtr<Core::Data::Notice> notice;
  auto type = ti_cast<Spp::Ast::Type>(ref);
  if (type == 0) {
    auto *refNode = ti_cast<Core::Data::Node>(ref);
    if (refNode == 0) {
      throw EXCEPTION(GenericException, STR("Invalid type reference."));
    }
    helper->getSeeker()->doForeach(ref, refNode->getOwner(),
      [=, &type, &notice](TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        type = ti_cast<Spp::Ast::Type>(obj);
        if (type != 0) {
          return Core::Data::Seeker::Verb::STOP;
        }
        // TODO: Support template defaults.
        // TODO: Handle aliases.
        return Core::Data::Seeker::Verb::MOVE;
      }
    );
  }
  if (type == 0) {
    if (notice != 0) helper->noticeStore->add(notice);
    helper->noticeStore->add(std::make_shared<InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(ref)));
  }

  return type;
}


Bool Helper::_isVoid(TiObject *self, TiObject *ref)
{
  PREPARE_SELF(helper, Helper);

  if (ref == 0) return true;
  auto type = helper->traceType(ref);
  if (type == 0) return false;
  return type->isA<VoidType>();
}


Bool Helper::_isImplicitlyCastableTo(TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef)
{
  PREPARE_SELF(helper, Helper);

  auto srcType = ti_cast<Type>(srcTypeRef);
  if (srcType == 0) {
    srcType = helper->traceType(srcTypeRef);
    if (srcType == 0) return false;
  }

  auto targetType = ti_cast<Type>(targetTypeRef);
  if (targetType == 0) {
    targetType = helper->traceType(targetTypeRef);
    if (targetType == 0) return false;
  }

  return srcType->isImplicitlyCastableTo(targetType, helper);
}


Bool Helper::_isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType)
{
  PREPARE_SELF(helper, Helper);

  auto referenceType = ti_cast<ReferenceType>(refType);
  if (referenceType == 0) return false;
  return referenceType->getContentType(helper) == contentType;
}


ReferenceType* Helper::_getReferenceTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getReferenceTemplate();

  TioSharedPtr result;
  if (tpl->matchInstance(type, helper, result)) {
    auto refType = result.ti_cast_get<ReferenceType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, STR("Template for reference type is invalid."));
    }
    return refType;
  } else {
    auto notice = result.ti_cast<Core::Data::Notice>();
    if (notice != 0) {
      helper->noticeStore->add(notice);
    }
    return 0;
  }
}


IntegerType* Helper::_getBoolType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->boolType == 0) {
    helper->boolType = helper->getIntType(1);
  }
  return helper->boolType;
}


IntegerType* Helper::_getInt64Type(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->int64Type == 0) {
    helper->int64Type = helper->getIntType(64);
  }
  return helper->int64Type;
}


//==============================================================================
// Main Functions

Template* Helper::getReferenceTemplate()
{
  if (this->refTemplate != 0) return this->refTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(STR("ref"));
  this->refTemplate = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (this->refTemplate == 0) {
    throw EXCEPTION(GenericException, STR("Invalid object found for ref template."));
  }
  return this->refTemplate;
}


IntegerType* Helper::getIntType(Word bitCount)
{
  StrStream stream;
  stream << STR("Int[") << bitCount << STR("]");

  auto typeRef = this->rootManager->parseExpression(stream.str().c_str()).s_cast<Core::Data::Ast::ParamPass>();
  typeRef->setOwner(this->rootManager->getRootScope().get());

  auto type = ti_cast<IntegerType>(this->getSeeker()->doGet(typeRef.get(), this->rootManager->getRootScope().get()));
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("Invalid object found for integer type."));
  }
  return type;
}

} } // namespace
