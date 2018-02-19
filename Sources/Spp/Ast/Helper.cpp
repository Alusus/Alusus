/**
 * @file Spp/Ast/Helper.cpp
 * Contains the implementation of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

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
    &this->getPointerTypeFor,
    &this->getBoolType,
    &this->getCharType,
    &this->getCharPtrType,
    &this->getCharArrayType,
    &this->getInt64Type,
    &this->getIntType,
    &this->getFloatType,
    &this->getVoidType,
    &this->resolveNodePath,
    &this->getFunctionName
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
  this->getPointerTypeFor = &Helper::_getPointerTypeFor;
  this->getBoolType = &Helper::_getBoolType;
  this->getCharType = &Helper::_getCharType;
  this->getCharPtrType = &Helper::_getCharPtrType;
  this->getCharArrayType = &Helper::_getCharArrayType;
  this->getInt64Type = &Helper::_getInt64Type;
  this->getIntType = &Helper::_getIntType;
  this->getFloatType = &Helper::_getFloatType;
  this->getVoidType = &Helper::_getVoidType;
  this->resolveNodePath = &Helper::_resolveNodePath;
  this->getFunctionName = &Helper::_getFunctionName;
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
  Char const *name, Core::Data::Node *astNode, Core::Basic::Container<TiObject> *types, Spp::ExecutionContext const *ec,
  TiObject *&callee, Type *&calleeType
) {
  Core::Data::Ast::Identifier identifier;
  identifier.setValue(name);
  return this->lookupCallee(&identifier, astNode, types, ec, callee, calleeType);
}


Bool Helper::_lookupCallee(
  TiObject *self, TiObject *ref, Core::Data::Node *astNode,
  Core::Basic::Container<TiObject> *types, Spp::ExecutionContext const *ec,
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

        return helper->lookupCallee_iteration(obj, types, ec, matchStatus, matchCount, notice, callee, calleeType);
      }
  );
  // Did we have a matched callee?
  if (notice != 0 && (matchCount > 1 || callee == 0)) {
    helper->noticeStore->add(notice);
  }
  if (matchCount > 1) {
    helper->noticeStore->add(std::make_shared<MultipleCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(astNode)));
    return false;
  } else if (callee == 0) {
    if (symbolFound) {
      helper->noticeStore->add(std::make_shared<NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(astNode)));
    } else {
      helper->noticeStore->add(std::make_shared<UnknownSymbolNotice>(Core::Data::Ast::findSourceLocation(astNode)));
    }
    return false;
  }

  return true;
}


Core::Data::Seeker::Verb Helper::_lookupCallee_iteration(
  TiObject *self, TiObject *obj, Core::Basic::Container<TiObject> *types, Spp::ExecutionContext const *ec,
  CallMatchStatus &matchStatus, Int &matchCount, SharedPtr<Core::Data::Notice> &notice,
  TiObject *&callee, Type *&calleeType
) {
  PREPARE_SELF(helper, Helper);

  if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
    auto f = static_cast<Ast::Function*>(obj);
    CallMatchStatus ms;
    ms = f->matchCall(types, helper, ec);
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
        types != 0 && types->getElementCount() == 1 &&
        helper->isImplicitlyCastableTo(types->getElement(0), helper->getInt64Type(), ec)
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


Bool Helper::_isImplicitlyCastableTo(
  TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, Spp::ExecutionContext const *ec
) {
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

  return srcType->isImplicitlyCastableTo(targetType, helper, ec);
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


ReferenceType* Helper::getReferenceTypeForPointerType(PointerType *type)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("type"), STR("Cannot be null."));
  }

  return this->getReferenceTypeFor(type->getContentType(this));
}


PointerType* Helper::_getPointerTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getPointerTemplate();

  TioSharedPtr result;
  if (tpl->matchInstance(type, helper, result)) {
    auto refType = result.ti_cast_get<PointerType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, STR("Template for pointer type is invalid."));
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


PointerType* Helper::getPointerTypeForReferenceType(ReferenceType *type)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("type"), STR("Cannot be null."));
  }

  return this->getPointerTypeFor(type->getContentType(this));
}


IntegerType* Helper::_getBoolType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->boolType == 0) {
    helper->boolType = helper->getIntType(1);
  }
  return helper->boolType;
}


IntegerType* Helper::_getCharType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->charType == 0) {
    helper->charType = helper->getIntType(8);
  }
  return helper->charType;
}


PointerType* Helper::_getCharPtrType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->charPtrType == 0) {
    auto typeRef = helper->rootManager->parseExpression(STR("ptr[Int[8]]")).s_cast<Core::Data::Ast::Identifier>();
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->charPtrType = ti_cast<PointerType>(helper->getSeeker()->doGet(
      typeRef.get(), helper->rootManager->getRootScope().get()
    ));
    if (helper->charPtrType == 0) {
      throw EXCEPTION(GenericException, STR("Invalid object found for ptr[Int[8]] type."));
    }
  }
  return helper->charPtrType;
}


ArrayType* Helper::_getCharArrayType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);

  // Prepare the reference.
  if (helper->charArrayTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("array[Int[8],") << size << STR("]");
    helper->charArrayTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->charArrayTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->charArrayTypeRef
      ->getParam().ti_cast_get<Core::Data::Ast::ExpressionList>()
      ->getShared(1).ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Seek the requested type.
  auto astType = ti_cast<Ast::ArrayType>(
    helper->getSeeker()->doGet(helper->charArrayTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get char array AST type."));
  }
  return astType;
}


IntegerType* Helper::_getInt64Type(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->int64Type == 0) {
    helper->int64Type = helper->getIntType(64);
  }
  return helper->int64Type;
}


IntegerType* Helper::_getIntType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);
  // Prepare the reference.
  if (helper->integerTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("Int[") << size << STR("]");
    helper->integerTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->integerTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->integerTypeRef->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  auto astType = ti_cast<Ast::IntegerType>(
    helper->getSeeker()->doGet(helper->integerTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get integer AST type."));
  }
  return astType;
}


FloatType* Helper::_getFloatType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);
  // Prepare the reference.
  if (helper->floatTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("Float[") << size << STR("]");
    helper->floatTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->floatTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->floatTypeRef->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  auto astType = ti_cast<Ast::FloatType>(
    helper->getSeeker()->doGet(helper->floatTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get float AST type."));
  }
  return astType;
}


VoidType* Helper::_getVoidType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->voidType == 0) {
    auto typeRef = helper->rootManager->parseExpression(STR("Void")).s_cast<Core::Data::Ast::Identifier>();
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->voidType = ti_cast<VoidType>(helper->getSeeker()->doGet(
      typeRef.get(), helper->rootManager->getRootScope().get()
    ));
    if (helper->voidType == 0) {
      throw EXCEPTION(GenericException, STR("Invalid object found for void type."));
    }
  }
  return helper->voidType;
}


Str Helper::_resolveNodePath(TiObject *self, Core::Data::Node const *node)
{
  PREPARE_SELF(helper, Helper);
  return helper->nodePathResolver->doResolve(node, helper);
}


Str const& Helper::_getFunctionName(TiObject *self, Function *astFunc)
{
  if (astFunc->getName().getStr() == STR("")) {
    PREPARE_SELF(helper, Helper);
    Str name = helper->nodePathResolver->doResolve(astFunc, helper);
    // Replace special characters with _.
    Str formattedName = std::regex_replace(name, std::regex("[^a-zA-Z0-9_]"), STR("_"));
    astFunc->setName(formattedName.c_str());
  }
  return astFunc->getName().getStr();
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


Template* Helper::getPointerTemplate()
{
  if (this->ptrTemplate != 0) return this->ptrTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(STR("ptr"));
  this->ptrTemplate = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (this->ptrTemplate == 0) {
    throw EXCEPTION(GenericException, STR("Invalid object found for ptr template."));
  }
  return this->ptrTemplate;
}

} } // namespace
