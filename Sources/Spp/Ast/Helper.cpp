/**
 * @file Spp/Ast/Helper.cpp
 * Contains the implementation of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
  Basic::initBindingCaches(this, {
    &this->isAstReference,
    &this->lookupCallee,
    &this->lookupCallee_iteration,
    &this->traceType,
    &this->isVoid,
    &this->isImplicitlyCastableTo,
    &this->isReferenceTypeFor,
    &this->getReferenceTypeFor,
    &this->getPointerTypeFor,
    &this->getValueTypeFor,
    &this->getNullType,
    &this->getBoolType,
    &this->getCharType,
    &this->getCharArrayType,
    &this->getInt64Type,
    &this->getIntType,
    &this->getWord64Type,
    &this->getWordType,
    &this->getFloatType,
    &this->getVoidType,
    &this->resolveNodePath,
    &this->getFunctionName,
    &this->getNeededIntSize,
    &this->getNeededWordSize,
    &this->getDefinitionDomain
  });
}


void Helper::initBindings()
{
  this->isAstReference = &Helper::_isAstReference;
  this->lookupCallee = &Helper::_lookupCallee;
  this->lookupCallee_iteration = &Helper::_lookupCallee_iteration;
  this->traceType = &Helper::_traceType;
  this->isVoid = &Helper::_isVoid;
  this->isImplicitlyCastableTo = &Helper::_isImplicitlyCastableTo;
  this->isReferenceTypeFor = &Helper::_isReferenceTypeFor;
  this->getReferenceTypeFor = &Helper::_getReferenceTypeFor;
  this->getPointerTypeFor = &Helper::_getPointerTypeFor;
  this->getValueTypeFor = &Helper::_getValueTypeFor;
  this->getNullType = &Helper::_getNullType;
  this->getBoolType = &Helper::_getBoolType;
  this->getCharType = &Helper::_getCharType;
  this->getCharArrayType = &Helper::_getCharArrayType;
  this->getInt64Type = &Helper::_getInt64Type;
  this->getIntType = &Helper::_getIntType;
  this->getWord64Type = &Helper::_getWord64Type;
  this->getWordType = &Helper::_getWordType;
  this->getFloatType = &Helper::_getFloatType;
  this->getVoidType = &Helper::_getVoidType;
  this->resolveNodePath = &Helper::_resolveNodePath;
  this->getFunctionName = &Helper::_getFunctionName;
  this->getNeededIntSize = &Helper::_getNeededIntSize;
  this->getNeededWordSize = &Helper::_getNeededWordSize;
  this->getDefinitionDomain = &Helper::_getDefinitionDomain;
}


//==============================================================================
// Main Functions

Bool Helper::_isAstReference(TiObject *self, TiObject *obj)
{
  return
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>();
}


Bool Helper::lookupCalleeByName(
  Char const *name, SharedPtr<Core::Data::SourceLocation> const &sl, Core::Data::Node *astNode, Bool searchOwners,
  Containing<TiObject> *types, ExecutionContext const *ec,
  TiObject *&callee, Type *&calleeType, SharedPtr<Core::Notices::Notice> &notice
) {
  Core::Data::Ast::Identifier identifier;
  identifier.setValue(name);
  identifier.setSourceLocation(sl);
  return this->lookupCallee(&identifier, astNode, searchOwners, types, ec, callee, calleeType, notice);
}


Bool Helper::_lookupCallee(
  TiObject *self, TiObject *ref, Core::Data::Node *astNode, Bool searchOwners,
  Containing<TiObject> *types, ExecutionContext const *ec,
  TiObject *&callee, Type *&calleeType, SharedPtr<Core::Notices::Notice> &notice
) {
  PREPARE_SELF(helper, Helper);

  callee = 0;
  calleeType = 0;
  TypeMatchStatus matchStatus = TypeMatchStatus::NONE;
  helper->getSeeker()->foreach(ref, astNode,
    [=, &callee, &calleeType, &matchStatus, &notice]
      (TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          if (notice == 0 && callee == 0) notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        return helper->lookupCallee_iteration(obj, types, ec, matchStatus, notice, callee, calleeType);
      },
      searchOwners ? 0 : SeekerExtension::Flags::SKIP_OWNERS_AND_USES
  );
  // Did we have a matched callee?
  if (callee != 0) {
    return true;
  } else {
    if (notice != 0) {
      if (notice->getSourceLocation() == 0) notice->setSourceLocation(Core::Data::Ast::findSourceLocation(ref));
    } else {
      notice = std::make_shared<Spp::Notices::UnknownSymbolNotice>(Core::Data::Ast::findSourceLocation(ref));
    }
    return false;
  }
}


Core::Data::Seeker::Verb Helper::_lookupCallee_iteration(
  TiObject *self, TiObject *obj, Containing<TiObject> *types, ExecutionContext const *ec,
  TypeMatchStatus &matchStatus, SharedPtr<Core::Notices::Notice> &notice,
  TiObject *&callee, Type *&calleeType
) {
  PREPARE_SELF(helper, Helper);

  if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
    // Match functions.
    auto f = static_cast<Ast::Function*>(obj);
    TypeMatchStatus ms;
    ms = f->getType()->matchCall(types, helper, ec);
    if (ms == TypeMatchStatus::NONE && matchStatus == TypeMatchStatus::NONE) {
      notice = std::make_shared<Spp::Notices::ArgsMismatchNotice>();
    } else if (ms >= TypeMatchStatus::IMPLICIT_CAST && matchStatus == TypeMatchStatus::NONE) {
      notice.reset();
      callee = f;
      calleeType = f->getType().get();
      matchStatus = ms;
    } else if (ms == matchStatus) {
      if (Core::Data::findOwner<Block>(static_cast<Core::Data::Node*>(callee)) == Core::Data::findOwner<Block>(f)) {
        callee = 0;
        notice = std::make_shared<Spp::Notices::MultipleCalleeMatchNotice>();
      }
      // If this was an exact match there is no point in searching more.
      if (ms == TypeMatchStatus::EXACT) return Core::Data::Seeker::Verb::STOP;
    } else if (ms > matchStatus) {
      notice.reset();
      callee  = f;
      calleeType = f->getType().get();
      matchStatus = ms;
    }
    return Core::Data::Seeker::Verb::MOVE;
  } else if (obj != 0 && helper->isAstReference(obj)) {
    // Match variables
    if (matchStatus >= TypeMatchStatus::IMPLICIT_CAST) {
      if (
        Core::Data::findOwner<Block>(static_cast<Core::Data::Node*>(callee)) ==
        Core::Data::findOwner<Block>(static_cast<Core::Data::Node*>(obj))
      ) {
        notice = std::make_shared<Spp::Notices::MultipleCalleeMatchNotice>();
        callee = 0;
        return Core::Data::Seeker::Verb::STOP;
      }
    }
    auto objType = ti_cast<Type>(helper->getSeeker()->tryGet(obj, obj));
    if (objType == 0) {
      notice = std::make_shared<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(obj));
    } else if (objType->isDerivedFrom<ArrayType>()) {
      if (
        types != 0 && types->getElementCount() == 1 &&
        helper->isImplicitlyCastableTo(types->getElement(0), helper->getInt64Type(), ec)
      ) {
        notice.reset();
        callee = obj;
        calleeType = objType;
        matchStatus = TypeMatchStatus::EXACT;
      } else {
        notice = std::make_shared<Spp::Notices::ArgsMismatchNotice>();
      }
    } else {
      auto funcType = helper->tryGetPointerContentType<FunctionType>(objType);
      if (funcType != 0) {
        // We have a function pointer.
        auto ms = funcType->matchCall(types, helper, ec);
        if (funcType->matchCall(types, helper, ec) >= TypeMatchStatus::IMPLICIT_CAST) {
          notice.reset();
          callee = obj;
          calleeType = objType;
          matchStatus = ms;
        } else {
          notice = std::make_shared<Spp::Notices::ArgsMismatchNotice>();
        }
      } else {
        notice = std::make_shared<Spp::Notices::InvalidOperationNotice>();
      }
    }
    return Core::Data::Seeker::Verb::STOP;
  } else {
    // Invalid
    if (matchStatus >= TypeMatchStatus::IMPLICIT_CAST) {
      if (
        Core::Data::findOwner<Block>(static_cast<Core::Data::Node*>(callee)) ==
        Core::Data::findOwner<Block>(static_cast<Core::Data::Node*>(obj))
      ) {
        notice = std::make_shared<Spp::Notices::MultipleCalleeMatchNotice>();
        callee = 0;
        return Core::Data::Seeker::Verb::STOP;
      }
    }
    notice = std::make_shared<Spp::Notices::InvalidOperationNotice>();
    callee = 0;
    return Core::Data::Seeker::Verb::STOP;
  }
}


Type* Helper::_traceType(TiObject *self, TiObject *ref)
{
  PREPARE_SELF(helper, Helper);

  SharedPtr<Core::Notices::Notice> notice;
  Spp::Ast::Type *type = 0;
  if (ref->isDerivedFrom<Spp::Ast::Type>()) {
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
  } else if (helper->isAstReference(ref)) {
    auto *refNode = ti_cast<Core::Data::Node>(ref);
    if (refNode == 0) {
      throw EXCEPTION(GenericException, S("Invalid type reference."));
    }
    helper->getSeeker()->foreach(ref, refNode->getOwner(),
      [=, &type, &notice](TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        // Do we have a type?
        type = ti_cast<Spp::Ast::Type>(obj);
        if (type != 0) {
          return Core::Data::Seeker::Verb::STOP;
        }

        // If we have a template then try to get a default instance.
        auto tpl = ti_cast<Spp::Ast::Template>(obj);
        if (tpl != 0) {
          Core::Data::Ast::List list;
          TioSharedPtr result;
          if (tpl->matchInstance(&list, helper, result)) {
            type = result.ti_cast_get<Spp::Ast::Type>();
            if (type != 0) {
              return Core::Data::Seeker::Verb::STOP;
            }
          } else {
            if (result != 0 && result->isDerivedFrom<Core::Notices::Notice>()) notice = result.s_cast<Core::Notices::Notice>();
          }
        }

        return Core::Data::Seeker::Verb::MOVE;
      }, 0
    );
  }

  if (type == 0) {
    if (notice != 0) helper->noticeStore->add(notice);
    helper->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(ref))
    );
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


Bool Helper::_isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType, ExecutionContext const *ec)
{
  PREPARE_SELF(helper, Helper);

  auto referenceType = ti_cast<ReferenceType>(refType);
  if (referenceType == 0) return false;
  return referenceType->getContentType(helper)->isEqual(contentType, helper, ec);
}


ReferenceType* Helper::_getReferenceTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getReferenceTemplate();

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
      helper->noticeStore->add(notice);
    }
    return 0;
  }
}


ReferenceType* Helper::getReferenceTypeForPointerType(PointerType *type)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, S("type"), S("Cannot be null."));
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
      throw EXCEPTION(GenericException, S("Template for pointer type is invalid."));
    }
    return refType;
  } else {
    auto notice = result.ti_cast<Core::Notices::Notice>();
    if (notice != 0) {
      helper->noticeStore->add(notice);
    }
    return 0;
  }
}


PointerType* Helper::getPointerTypeForReferenceType(ReferenceType *type)
{
  if (type == 0) {
    throw EXCEPTION(InvalidArgumentException, S("type"), S("Cannot be null."));
  }

  return this->getPointerTypeFor(type->getContentType(this));
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


Str Helper::_resolveNodePath(TiObject *self, Core::Data::Node const *node)
{
  PREPARE_SELF(helper, Helper);
  return helper->nodePathResolver->doResolve(node, helper);
}


Str const& Helper::_getFunctionName(TiObject *self, Function *astFunc)
{
  if (astFunc->getName().getStr() == S("")) {
    PREPARE_SELF(helper, Helper);
    Str name = helper->nodePathResolver->doResolve(astFunc, helper);
    astFunc->setName(name.c_str());
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


DefinitionDomain Helper::_getDefinitionDomain(TiObject *self, TiObject const *obj)
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

  if (def->getOwner() == 0) {
    throw EXCEPTION(GenericException, S("Definition is not part of any scope."));
  }

  if (def->getOwner()->isDerivedFrom<Module>()) {
    return DefinitionDomain::GLOBAL;
  } else {
    // The definition is either inside a function or a type.
    // Is it shared?
    PREPARE_SELF(helper, Helper);
    if (helper->isSharedDef(def)) return DefinitionDomain::GLOBAL;
    // it's not static, so it's either a function local or an object member.
    auto owner = def->getOwner()->getOwner();
    if (owner->isDerivedFrom<Type>()) {
      return DefinitionDomain::OBJECT;
    } else {
      return DefinitionDomain::FUNCTION;
    }
  }
}


Bool Helper::isSharedDef(Core::Data::Ast::Definition const *def)
{
  auto modifiers = def->getModifiers().get();
  if (modifiers != 0) {
    for (Int i = 0; i < modifiers->getElementCount(); ++i) {
      auto identifier = ti_cast<Core::Data::Ast::Identifier>(modifiers->getElement(i));
      if (identifier != 0 && identifier->getValue() == S("shared")) return true;
    }
  }
  return false;
}


//==============================================================================
// Main Functions

Template* Helper::getReferenceTemplate()
{
  if (this->refTemplate != 0) return this->refTemplate;

  Core::Data::Ast::Identifier identifier;
  identifier.setValue(S("ref"));
  this->refTemplate = ti_cast<Template>(rootManager->getSeeker()->doGet(
    &identifier, this->rootManager->getRootScope().get())
  );
  if (this->refTemplate == 0) {
    throw EXCEPTION(GenericException, S("Invalid object found for ref template."));
  }
  return this->refTemplate;
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

} } // namespace
