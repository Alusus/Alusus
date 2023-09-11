/**
 * @file Spp/Ast/Helper.cpp
 * Contains the implementation of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Initialization Functions

void Helper::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->isAstReference,
    &this->isVariable,
    &this->isInMemVariable,
    &this->isValueOnlyVariable,
    &this->lookupCustomCaster,
    &this->_traceType,
    &this->isVoid,
    &this->isImplicitlyCastableTo,
    &this->isExplicitlyCastableTo,
    &this->matchTargetType,
    &this->isReferenceTypeFor,
    &this->getReferenceTypeFor,
    &this->getPointerTypeFor,
    &this->getArrayTypeFor,
    &this->getValueTypeFor,
    &this->getNullType,
    &this->getBoolType,
    &this->getCharType,
    &this->getCharArrayType,
    &this->getArchIntType,
    &this->getIntType,
    &this->getWord64Type,
    &this->getWordType,
    &this->getFloatType,
    &this->getVoidType,
    &this->getTiObjectType,
    &this->resolveNodePath,
    &this->getFunctionName,
    &this->getNeededIntSize,
    &this->getNeededWordSize,
    &this->getVariableDomain,
    &this->validateUseStatement
  });
}


void Helper::initBindings()
{
  this->isAstReference = &Helper::_isAstReference;
  this->isVariable = &Helper::_isVariable;
  this->isInMemVariable = &Helper::_isInMemVariable;
  this->isValueOnlyVariable = &Helper::_isValueOnlyVariable;
  this->lookupCustomCaster = &Helper::_lookupCustomCaster;
  this->_traceType = &Helper::__traceType;
  this->isVoid = &Helper::_isVoid;
  this->isImplicitlyCastableTo = &Helper::_isImplicitlyCastableTo;
  this->isExplicitlyCastableTo = &Helper::_isExplicitlyCastableTo;
  this->matchTargetType = &Helper::_matchTargetType;
  this->isReferenceTypeFor = &Helper::_isReferenceTypeFor;
  this->getReferenceTypeFor = &Helper::_getReferenceTypeFor;
  this->getPointerTypeFor = &Helper::_getPointerTypeFor;
  this->getArrayTypeFor = &Helper::_getArrayTypeFor;
  this->getValueTypeFor = &Helper::_getValueTypeFor;
  this->getNullType = &Helper::_getNullType;
  this->getBoolType = &Helper::_getBoolType;
  this->getCharType = &Helper::_getCharType;
  this->getCharArrayType = &Helper::_getCharArrayType;
  this->getArchIntType = &Helper::_getArchIntType;
  this->getIntType = &Helper::_getIntType;
  this->getWord64Type = &Helper::_getWord64Type;
  this->getWordType = &Helper::_getWordType;
  this->getFloatType = &Helper::_getFloatType;
  this->getVoidType = &Helper::_getVoidType;
  this->getTiObjectType = &Helper::_getTiObjectType;
  this->resolveNodePath = &Helper::_resolveNodePath;
  this->getFunctionName = &Helper::_getFunctionName;
  this->getNeededIntSize = &Helper::_getNeededIntSize;
  this->getNeededWordSize = &Helper::_getNeededWordSize;
  this->getVariableDomain = &Helper::_getVariableDomain;
  this->validateUseStatement = &Helper::_validateUseStatement;
}


//==============================================================================
// Main Functions

Bool Helper::_isAstReference(TiObject *self, TiObject *obj)
{
  return
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isVariable(TiObject *self, TiObject *obj)
{
  // TODO: Remove this in favor of obj->isDerivedFrom<Variable> once the Variable class is fully utilized.
  return
    obj->isDerivedFrom<Spp::Ast::Variable>() ||
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isInMemVariable(TiObject *self, TiObject *obj)
{
  return
    (obj->isDerivedFrom<Spp::Ast::Variable>() && !static_cast<Spp::Ast::Variable*>(obj)->isValueOnly()) ||
    // TODO: Remove the following once the Variable class is fully utilized.
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isValueOnlyVariable(TiObject *self, TiObject *obj)
{
  return
    obj->isDerivedFrom<Spp::Ast::Variable>() &&
    static_cast<Spp::Ast::Variable*>(obj)->isValueOnly();
}


TypeMatchStatus Helper::_lookupCustomCaster(
  TiObject *self, Type *srcType, Type *targetType, ExecutionContext const *ec, Function *&caster
) {
  PREPARE_SELF(helper, Helper);

  auto srcRefType = ti_cast<ReferenceType>(srcType);
  if (srcRefType != 0) {
    auto srcContentType = ti_cast<DataType>(srcRefType->getContentType(helper));
    if (srcContentType != 0 && srcContentType->getBody() != 0) {
      PlainList<TiObject> argTypes({ srcRefType });
      static Core::Data::Ast::Identifier ref({{S("value"), TiStr(S("~cast"))}});
      TypeMatchStatus retMatch;
      helper->getSeeker()->extForeach(&ref, srcContentType->getBody().get(),
        [=, &retMatch, &caster, &argTypes] (TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
        {
          if (action != Core::Data::Seeker::Action::TARGET_MATCH) return Core::Data::Seeker::Verb::MOVE;
          auto func = ti_cast<Function>(obj);
          if (func != 0) {
            auto funcType = func->getType().get();
            auto match = funcType->matchCall(&argTypes, helper, ec);
            if (match == TypeMatchStatus::EXACT) {
              auto retType = funcType->traceRetType(helper);
              if (retType->getInitializationMethod(helper, ec) != Ast::TypeInitMethod::NONE) {
                retType = helper->getReferenceTypeFor(retType, Ast::ReferenceMode::IMPLICIT);
              }
              Function *innerCaster;
              auto rmatch = helper->matchTargetType(retType, targetType, ec, innerCaster);
              if ((rmatch>=TypeMatchStatus::CUSTOM_CASTER || rmatch==TypeMatchStatus::AGGREGATION) && rmatch>retMatch) {
                retMatch = rmatch;
                caster = func;
                if (rmatch == TypeMatchStatus::EXACT) return Core::Data::Seeker::Verb::STOP;
              }
            }
          }
          return Core::Data::Seeker::Verb::MOVE;
        },
        Core::Data::Seeker::Flags::SKIP_OWNERS | Core::Data::Seeker::Flags::SKIP_USES
      );
      return retMatch;
    }
  }
  return TypeMatchStatus::NONE;
}


Type* Helper::__traceType(TiObject *self, TiObject *ref, Bool skipErrors)
{
  PREPARE_SELF(helper, Helper);

  SharedPtr<Core::Notices::Notice> notice;
  TiObject *foundObj = 0;
  Spp::Ast::Type *type = 0;
  if (ref->isDerivedFrom<Spp::Ast::TypeOp>()) {
    auto typeOp = static_cast<Spp::Ast::TypeOp*>(ref);
    if (typeOp->getType() != 0) return typeOp->getType();
    auto operand = typeOp->getOperand().ti_cast_get<Core::Data::Node>();
    auto typeRef = helper->getSeeker()->tryGet(operand, operand->getOwner());
    if (typeRef != 0) {
      type = helper->_traceType(typeRef, skipErrors);
      if (type != 0) typeOp->setType(type);
    }
  }
  else if (ref->isDerivedFrom<Spp::Ast::Type>()) {
    type = static_cast<Spp::Ast::Type*>(ref);
  } else if (ref->isDerivedFrom<Spp::Ast::Template>()) {
    auto tpl = static_cast<Spp::Ast::Template*>(ref);
    Core::Data::Ast::List list;
    TioSharedPtr result;
    if (tpl->matchInstance(&list, helper, result)) {
      type = result.ti_cast_get<Spp::Ast::Type>();
    } else {
      if (result != 0 && result->isDerivedFrom<Core::Notices::Notice>()) notice = result.s_cast<Core::Notices::Notice>();
    }
  } else if (ref->isDerivedFrom<Spp::Ast::Variable>()) {
    auto var = static_cast<Spp::Ast::Variable*>(ref);
    if (var->getType() != 0) return var->getType();
    if (var->getTypeRef() == 0) {
      throw EXCEPTION(GenericException, S("Variable definition is missing the type reference."));
    }
    type = helper->_traceType(var->getTypeRef().get(), skipErrors);
    var->setType(type);
  } else if (ref->isDerivedFrom<Spp::Ast::ArgPack>()) {
    // With arg packs we should already have the type set during the generation of the function signature.
    return getAstType(ref);
  } else if (ref->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Data::Ast::Bracket*>(ref);
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
      return helper->_traceType(bracket->getOperand().get(), skipErrors);
    }
  } else if (helper->isAstReference(ref)) {
    type = tryGetAstType(ref);
    if (type != 0) return type;
    auto typeRef = static_cast<Core::Data::Node*>(ref);
    auto owner = typeRef->getOwner();
    auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(ref);
    if (paramPass != 0 && paramPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      typeRef = paramPass->getOperand().ti_cast_get<Core::Data::Node>();
      if (typeRef == 0) throw EXCEPTION(GenericException, S("Invalid type reference."));
      return helper->_traceType(typeRef, skipErrors);
    }
    helper->getSeeker()->extForeach(typeRef, owner,
      [=, &foundObj, &type, &notice](TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
      {
        if (action == Core::Data::Seeker::Action::ERROR) {
          notice = getSharedPtr(ti_cast<Core::Notices::Notice>(obj));
          ASSERT(notice != 0);
          return Core::Data::Seeker::Verb::MOVE;
        }
        if (action != Core::Data::Seeker::Action::TARGET_MATCH) {
          return Core::Data::Seeker::Verb::MOVE;
        }

        // Unbox if we have a box.
        auto box = ti_cast<TioWeakBox>(obj);
        if (box != 0) foundObj = box->get().get();
        else foundObj = obj;

        // Do we have a type?
        type = ti_cast<Spp::Ast::Type>(foundObj);
        if (type != 0) {
          return Core::Data::Seeker::Verb::STOP;
        }

        // If we have a template then try to get a default instance.
        auto tpl = ti_cast<Spp::Ast::Template>(foundObj);
        if (tpl != 0) {
          Core::Data::Ast::List list;
          list.setSourceLocation(Core::Data::Ast::findSourceLocation(ref));
          TioSharedPtr result;
          if (tpl->matchInstance(&list, helper, result)) {
            type = result.ti_cast_get<Spp::Ast::Type>();
            if (type != 0) {
              return Core::Data::Seeker::Verb::STOP;
            }
          } else {
            if (result != 0 && result->isDerivedFrom<Core::Notices::Notice>()) {
              notice = result.s_cast<Core::Notices::Notice>();
            }
          }
        }

        return Core::Data::Seeker::Verb::MOVE;
      }, 0
    );
    if (type != 0) setAstType(ref, type);
  }

  if (type == 0 && helper->rootManager->getNoticeStore() != 0 && !skipErrors) {
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    } else if (foundObj == 0) {
      helper->rootManager->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(ref))
      );
    } else {
      helper->rootManager->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IdentifierIsNotTypeNotice>(Core::Data::Ast::findSourceLocation(ref))
      );
    }
  }

  return type;
}


Bool Helper::_isVoid(TiObject *self, TiObject const *ref)
{
  PREPARE_SELF(helper, Helper);

  if (ref == 0) return true;
  auto type = helper->traceType(const_cast<TiObject*>(ref));
  if (type == 0) return false;
  return type->isA<VoidType>();
}


Bool Helper::_isImplicitlyCastableTo(
  TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec
) {
  PREPARE_SELF(helper, Helper);
  Function *caster;
  return helper->matchTargetType(srcTypeRef, targetTypeRef, ec, caster) >= TypeMatchStatus::CUSTOM_CASTER;
}


Bool Helper::_isExplicitlyCastableTo(
  TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec
) {
  PREPARE_SELF(helper, Helper);
  Function *caster;
  return helper->matchTargetType(srcTypeRef, targetTypeRef, ec, caster) >= TypeMatchStatus::EXPLICIT_CAST;
}


TypeMatchStatus Helper::_matchTargetType(
  TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec, Function *&caster
) {
  PREPARE_SELF(helper, Helper);

  auto srcType = ti_cast<Type>(srcTypeRef);
  if (srcType == 0) {
    srcType = helper->traceType(srcTypeRef);
    if (srcType == 0) return TypeMatchStatus::NONE;
  }

  auto targetType = ti_cast<Type>(targetTypeRef);
  if (targetType == 0) {
    targetType = helper->traceType(targetTypeRef);
    if (targetType == 0) return TypeMatchStatus::NONE;
  }

  // If the target type is a temp_ref then we'll cast to the content type instead of the ref type, and then update
  // derefs accordingly if we found a match. This is because even if we have a value rather than a reference the
  // casting is still possible since this is a temp ref and we can create a temp var to convert values into
  // references.
  Bool negativeDeref = false;
  if (
    targetType->isDerivedFrom<ReferenceType>() &&
    static_cast<ReferenceType*>(targetType)->getMode() == Ast::ReferenceMode::TEMP_EXPLICIT
  ) {
    negativeDeref = true;
    targetType = static_cast<ReferenceType*>(targetType)->getContentType(helper);
  }
  auto matchType = srcType->matchTargetType(targetType, helper, ec);
  if (negativeDeref && (matchType == TypeMatchStatus::AGGREGATION || matchType >= TypeMatchStatus::REF_AGGREGATION)) {
    --matchType.derefs;
    if (matchType == TypeMatchStatus::AGGREGATION) matchType.value = TypeMatchStatus::REF_AGGREGATION;
    return matchType;
  } else if (!negativeDeref && matchType >= TypeMatchStatus::IMPLICIT_CAST) {
    return matchType;
  } else {
    if (!srcType->isDerivedFrom<ReferenceType>()) {
      auto srcReferenceType = helper->getReferenceTypeFor(srcType, Ast::ReferenceMode::TEMP_EXPLICIT);
      auto customMatchType = helper->lookupCustomCaster(srcReferenceType, targetType, ec, caster);
      if (customMatchType >= TypeMatchStatus::IMPLICIT_CAST) {
        return TypeMatchStatus(customMatchType, TypeMatchStatus::CUSTOM_CASTER, -1);
      }
    }
    Int deref = 0;
    while (srcType->isDerivedFrom<ReferenceType>() && static_cast<ReferenceType*>(srcType)->isAutoDeref()) {
      auto customMatchType = helper->lookupCustomCaster(srcType, targetType, ec, caster);
      if (customMatchType>=TypeMatchStatus::CUSTOM_CASTER) {
        return TypeMatchStatus(customMatchType, TypeMatchStatus::CUSTOM_CASTER, deref);
      }
      ++deref;
      srcType = static_cast<ReferenceType*>(srcType)->getContentType(helper);
    }
    if (negativeDeref) return TypeMatchStatus::NONE;
    else return matchType;
  }
}


Bool Helper::_isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType, ExecutionContext const *ec)
{
  PREPARE_SELF(helper, Helper);

  auto referenceType = ti_cast<ReferenceType>(refType);
  if (referenceType == 0) return false;
  return referenceType->getContentType(helper)->isEqual(contentType, helper, ec);
}


ReferenceType* Helper::_getReferenceTypeFor(TiObject *self, TiObject *type, ReferenceMode const &mode)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getReferenceTemplate(mode);

  TioSharedPtr result;
  if (tpl->matchInstance(type, helper, result)) {
    auto refType = result.ti_cast_get<ReferenceType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for reference type is invalid."));
    }
    return refType;
  } else {
    auto notice = result.ti_cast<Core::Notices::Notice>();
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    }
    return 0;
  }
}


ReferenceType* Helper::getReferenceTypeForPointerType(PointerType *type, ReferenceMode const &mode)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, S("type"), S("Cannot be null."));
  }

  return this->getReferenceTypeFor(type->getContentType(this), mode);
}


PointerType* Helper::_getPointerTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getPointerTemplate();

  TioSharedPtr result;
  if (tpl->matchInstance(type, helper, result)) {
    auto refType = result.ti_cast_get<PointerType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for pointer type is invalid."));
    }
    return refType;
  } else {
    auto notice = result.ti_cast<Core::Notices::Notice>();
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    }
    return 0;
  }
}


ArrayType* Helper::_getArrayTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getArrayTemplate();

  TioSharedPtr result;
  if (tpl->matchInstance(type, helper, result)) {
    auto refType = result.ti_cast_get<ArrayType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for array type is invalid."));
    }
    return refType;
  } else {
    auto notice = result.ti_cast<Core::Notices::Notice>();
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    }
    return 0;
  }
}


Type* Helper::swichInnerReferenceTypeWithPointerType(ReferenceType *type)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, S("type"), S("Cannot be null."));
  }

  auto contentType = type->getContentType(this);
  auto innerRefType = ti_cast<ReferenceType>(contentType);
  if (innerRefType != 0 && innerRefType->isAutoDeref()) {
    return this->getReferenceTypeFor(this->swichInnerReferenceTypeWithPointerType(innerRefType), type->getMode());
  } else {
    return this->getPointerTypeFor(contentType);
  }
}


Type* Helper::swichOuterPointerTypeWithReferenceType(Type *type, ReferenceMode const &mode)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, S("type"), S("Cannot be null."));
  }

  if (type->isDerivedFrom<PointerType>()) {
    auto contentType = static_cast<PointerType*>(type)->getContentType(this);
    return this->getReferenceTypeFor(contentType, mode);
  } else if (type->isDerivedFrom<ReferenceType>()) {
    auto refType = static_cast<ReferenceType*>(type);
    auto contentType = refType->getContentType(this);
    auto switchedContentType = this->swichOuterPointerTypeWithReferenceType(contentType, mode);
    if (switchedContentType == 0) return 0;
    return this->getReferenceTypeFor(switchedContentType, refType->getMode());
  } else {
    return 0;
  }
}


Type* Helper::_getValueTypeFor(TiObject *self, TiObject *typeRef)
{
  PREPARE_SELF(helper, Helper);

  Type *type = helper->traceType(typeRef);
  if (type->isDerivedFrom<ReferenceType>()) {
    return static_cast<ReferenceType*>(type)->getContentType(helper);
  } else {
    return type;
  }
}


IntegerType* Helper::_getNullType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  // Prepare the reference.
  if (helper->nullType == 0) {
    // Create a new reference.
    auto typeRef = helper->rootManager->parseExpression(S("Null")).s_cast<Core::Data::Ast::Identifier>();
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->nullType = ti_cast<Ast::IntegerType>(
      helper->getSeeker()->doGet(typeRef.get(), helper->rootManager->getRootScope().get())
    );
    if (helper->nullType == 0) {
      throw EXCEPTION(GenericException, S("Failed to get null AST type."));
    }
  }
  return helper->nullType;
}


IntegerType* Helper::_getBoolType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->boolType == 0) {
    helper->boolType = helper->getWordType(1);
  }
  return helper->boolType;
}


IntegerType* Helper::_getCharType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->charType == 0) {
    helper->charType = helper->getWordType(8);
  }
  return helper->charType;
}


ArrayType* Helper::_getCharArrayType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);

  // Prepare the reference.
  if (helper->charArrayTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << S("array[Word[8],") << size << S("]");
    helper->charArrayTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->charArrayTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->charArrayTypeRef
      ->getParam().ti_cast_get<Core::Data::Ast::List>()
      ->get(1).ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, S("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Seek the requested type.
  auto astType = ti_cast<Ast::ArrayType>(
    helper->getSeeker()->doGet(helper->charArrayTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get char array AST type."));
  }
  return astType;
}


IntegerType* Helper::_getArchIntType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->archIntType == 0) {
    helper->archIntType = helper->getIntType(0);
  }
  return helper->archIntType;
}


IntegerType* Helper::_getIntType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);
  // Prepare the reference.
  if (helper->integerTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << S("Int[") << size << S("]");
    helper->integerTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->integerTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->integerTypeRef->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, S("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  auto astType = ti_cast<Ast::IntegerType>(
    helper->getSeeker()->doGet(helper->integerTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get integer AST type."));
  }
  return astType;
}


IntegerType* Helper::_getWord64Type(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->word64Type == 0) {
    helper->word64Type = helper->getWordType(64);
  }
  return helper->word64Type;
}


IntegerType* Helper::_getWordType(TiObject *self, Word size)
{
  PREPARE_SELF(helper, Helper);
  // Prepare the reference.
  if (helper->wordTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << S("Word[") << size << S("]");
    helper->wordTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->wordTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->wordTypeRef->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, S("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  auto astType = ti_cast<Ast::IntegerType>(
    helper->getSeeker()->doGet(helper->wordTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get integer AST type."));
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
    stream << S("Float[") << size << S("]");
    helper->floatTypeRef = helper->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    helper->floatTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->floatTypeRef->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, S("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  auto astType = ti_cast<Ast::FloatType>(
    helper->getSeeker()->doGet(helper->floatTypeRef.get(), helper->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get float AST type."));
  }
  return astType;
}


VoidType* Helper::_getVoidType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->voidType == 0) {
    auto typeRef = helper->rootManager->parseExpression(S("Void")).s_cast<Core::Data::Ast::Identifier>();
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->voidType = ti_cast<VoidType>(helper->getSeeker()->doGet(
      typeRef.get(), helper->rootManager->getRootScope().get()
    ));
    if (helper->voidType == 0) {
      throw EXCEPTION(GenericException, S("Invalid object found for void type."));
    }
  }
  return helper->voidType;
}


UserType* Helper::_getTiObjectType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->tiObjectType == 0) {
    auto typeRef = helper->rootManager->parseExpression(S("Core.Basic.TiObject")).s_cast<Core::Data::Node>();
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->tiObjectType = ti_cast<UserType>(helper->getSeeker()->tryGet(
      typeRef.get(), helper->rootManager->getRootScope().get()
    ));
  }
  return helper->tiObjectType;
}


Str Helper::_resolveNodePath(TiObject *self, Core::Data::Node const *node)
{
  PREPARE_SELF(helper, Helper);
  return helper->nodePathResolver->doResolve(node, helper);
}


Str const& Helper::_getFunctionName(TiObject *self, Function *astFunc)
{
  static LongInt anonymousFuncIndex = 0;
  if (astFunc->getName().getStr() == S("")) {
    if (ti_cast<Core::Data::Ast::Definition>(astFunc->getOwner()) == 0) {
      // This is a anonymous function.
      astFunc->setName(Str("__anonymousfunc__") + (anonymousFuncIndex++));
    } else {
      PREPARE_SELF(helper, Helper);
      Str name = helper->nodePathResolver->doResolve(astFunc, helper);
      astFunc->setName(name);
    }
  }
  return astFunc->getName().getStr();
}


Word Helper::_getNeededIntSize(TiObject *self, LongInt value)
{
  if (value == 0 || value == -1) return 1;
  else {
    Word bitCount = 8;
    for (Int i = 0; i < 7; ++i) {
      value >>= 7;
      if (value == 0 || value == -1) break;
      value >>= 1;
      bitCount += 8;
    }
    if (bitCount <= 16) return bitCount;
    else if (bitCount <= 32) return 32;
    else if (bitCount <= 64) return 64;
    else return 128;
  }
}


Word Helper::_getNeededWordSize(TiObject *self, LongWord value)
{
  if (value == 0 || value == 1) return 1;
  else {
    Word bitCount = 8;
    for (Int i = 0; i < 7; ++i) {
      value >>= 8;
      if (value == 0) break;
      bitCount += 8;
    }
    if (bitCount <= 16) return bitCount;
    else if (bitCount <= 32) return 32;
    else if (bitCount <= 64) return 64;
    else return 128;
  }
}


DefinitionDomain Helper::_getVariableDomain(TiObject *self, TiObject const *obj)
{
  // Find the definition.
  auto def = ti_cast<Core::Data::Ast::Definition const>(obj);
  if (def == 0) {
    auto node = ti_cast<Core::Data::Node const>(obj);
    if (node == 0) {
      throw EXCEPTION(InvalidArgumentException, S("obj"), S("Object is null or of invalid type."), obj);
    }
    def = ti_cast<Core::Data::Ast::Definition const>(node->getOwner());
    if (def == 0) {
      // This could be a function arg.
      if (
        node->getOwner() != 0 && node->getOwner()->getOwner() != 0 &&
        node->getOwner()->getOwner()->isDerivedFrom<FunctionType>()
      ) {
        return DefinitionDomain::FUNCTION;
      } else {
        throw EXCEPTION(InvalidArgumentException, S("obj"), S("Object is not a definition."), obj);
      }
    }
  }

  // Values are always treated as local.
  auto var = def->getTarget().ti_cast_get<Ast::Variable>();
  if (var != 0 && var->isValueOnly()) return DefinitionDomain::FUNCTION;

  auto owner = def->getOwner();
  while (owner != 0) {
    if (owner->isDerivedFrom<Module>()) {
      return DefinitionDomain::GLOBAL;
    } else if (owner->isDerivedFrom<Type>()) {
      PREPARE_SELF(helper, Helper);
      return helper->isSharedDef(def) ? DefinitionDomain::GLOBAL : DefinitionDomain::OBJECT;
    } else if (owner->isDerivedFrom<Function>()) {
      PREPARE_SELF(helper, Helper);
      return helper->isSharedDef(def) ? DefinitionDomain::GLOBAL : DefinitionDomain::FUNCTION;
    } else if (owner->isDerivedFrom<PreprocessStatement>()) {
      return DefinitionDomain::FUNCTION;
    } else if (owner->isDerivedFrom<WhileStatement>()) {
      return DefinitionDomain::FUNCTION;
    } else if (owner->isDerivedFrom<ForStatement>()) {
      return DefinitionDomain::FUNCTION;
    } else if (owner->isDerivedFrom<IfStatement>()) {
      return DefinitionDomain::FUNCTION;
    } else if (owner->isDerivedFrom<Block>()) {
      if (
        owner->getOwner()->isDerivedFrom<UseInOp>() || owner->getOwner()->isDerivedFrom<Core::Data::Ast::LinkOperator>()
      ) {
        return DefinitionDomain::FUNCTION;
      }
    }
    owner = owner->getOwner();
  }
  return DefinitionDomain::GLOBAL;
}


Bool Helper::doesModifierExistOnDef(Core::Data::Ast::Definition const *def, Char const *name)
{
  auto modifiers = def->getModifiers().get();
  if (modifiers != 0) {
    for (Int i = 0; i < modifiers->getElementCount(); ++i) {
      auto identifier = ti_cast<Core::Data::Ast::Identifier>(modifiers->getElement(i));
      if (identifier != 0 && identifier->getValue() == name) return true;
    }
  }
  return false;
}


Bool Helper::_validateUseStatement(TiObject *self, Core::Data::Ast::Bridge *bridge)
{
  PREPARE_SELF(helper, Helper);
  VALIDATE_NOT_NULL(bridge);
  if (bridge->getTarget() == 0) {
    throw EXCEPTION(InvalidArgumentException, S("bridge"), S("Use statement has a null target."));
  }
  Bool found = false;
  helper->getSeeker()->foreach(bridge->getTarget().get(), bridge->getOwner(),
    [=, &found] (TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
    {
      if (action != Core::Data::Seeker::Action::TARGET_MATCH) return Core::Data::Seeker::Verb::MOVE;
      if (ti_cast<Ast::Module>(obj) != 0) {
        found = true;
        return Core::Data::Seeker::Verb::STOP;
      } else {
        return Core::Data::Seeker::Verb::MOVE;
      }
    }, 0
  );
  if (!found) {
    helper->rootManager->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidUseStatementNotice>(bridge->findSourceLocation())
    );
  }
  return found;
}


//==============================================================================
// Main Functions

Template* Helper::getReferenceTemplate(ReferenceMode const &mode)
{
  if (mode == ReferenceMode::EXPLICIT && this->refTemplate != 0) return this->refTemplate;
  else if (mode == ReferenceMode::TEMP_EXPLICIT && this->trefTemplate != 0) return this->trefTemplate;
  else if (mode == ReferenceMode::IMPLICIT && this->irefTemplate != 0) return this->irefTemplate;
  else if (mode == ReferenceMode::NO_DEREF && this->ndrefTemplate != 0) return this->ndrefTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(
    mode == ReferenceMode::EXPLICIT ? S("ref") : (
      mode == ReferenceMode::TEMP_EXPLICIT ? S("temp_ref") : (mode == ReferenceMode::IMPLICIT ? S("iref") : S("ndref"))
    )
  );
  auto tpl = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (tpl == 0) {
    throw EXCEPTION(GenericException, S("Invalid object found for ref template."));
  }
  if (mode == ReferenceMode::EXPLICIT) this->refTemplate = tpl;
  else if (mode == ReferenceMode::TEMP_EXPLICIT) this->trefTemplate = tpl;
  else if (mode == ReferenceMode::IMPLICIT) this->irefTemplate = tpl;
  else this->ndrefTemplate = tpl;
  return tpl;
}


Template* Helper::getPointerTemplate()
{
  if (this->ptrTemplate != 0) return this->ptrTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(S("ptr"));
  this->ptrTemplate = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (this->ptrTemplate == 0) {
    throw EXCEPTION(GenericException, S("Invalid object found for ptr template."));
  }
  return this->ptrTemplate;
}


Template* Helper::getArrayTemplate()
{
  if (this->arrayTemplate != 0) return this->arrayTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(S("array"));
  this->arrayTemplate = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (this->arrayTemplate == 0) {
    throw EXCEPTION(GenericException, S("Invalid object found for array template."));
  }
  return this->arrayTemplate;
}

} } // namespace
