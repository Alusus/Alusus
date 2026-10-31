/**
 * @file Spp/Ast/Helper.cpp
 * Contains the implementation of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
    &this->isCastableTo,
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
    &this->getNodeType,
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
  this->isCastableTo = &Helper::_isCastableTo;
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
  this->getNodeType = &Helper::_getNodeType;
  this->resolveNodePath = &Helper::_resolveNodePath;
  this->getFunctionName = &Helper::_getFunctionName;
  this->getNeededIntSize = &Helper::_getNeededIntSize;
  this->getNeededWordSize = &Helper::_getNeededWordSize;
  this->getVariableDomain = &Helper::_getVariableDomain;
  this->validateUseStatement = &Helper::_validateUseStatement;
}


//==============================================================================
// Main Functions

Bool Helper::_isAstReference(TiObject *self, Core::Ast::Node *obj)
{
  return
    obj->isDerivedFrom<Core::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isVariable(TiObject *self, Core::Ast::Node *obj)
{
  // TODO: Remove this in favor of obj->isDerivedFrom<Variable> once the Variable class is fully utilized.
  return
    obj->isDerivedFrom<Spp::Ast::Variable>() ||
    obj->isDerivedFrom<Core::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isInMemVariable(TiObject *self, Core::Ast::Node *obj)
{
  return
    (obj->isDerivedFrom<Spp::Ast::Variable>() && !static_cast<Spp::Ast::Variable*>(obj)->isValueOnly()) ||
    // TODO: Remove the following once the Variable class is fully utilized.
    obj->isDerivedFrom<Core::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Ast::Identifier>() ||
    obj->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
    obj->isDerivedFrom<Spp::Ast::TypeOp>();
}


Bool Helper::_isValueOnlyVariable(TiObject *self, Core::Ast::Node *obj)
{
  return
    obj->isDerivedFrom<Spp::Ast::Variable>() &&
    static_cast<Spp::Ast::Variable*>(obj)->isValueOnly();
}


TypeMatchStatus Helper::_lookupCustomCaster(
  TiObject *self, Type *srcType, Type *targetType, Function *&caster
) {
  PREPARE_SELF(helper, Helper);

  auto srcRefType = ti_cast<ReferenceType>(srcType);
  if (srcRefType != 0) {
    auto srcContentType = ti_cast<DataType>(srcRefType->getContentType(helper));
    if (srcContentType != 0 && srcContentType->getBody() != 0) {
      PlainList<Spp::Ast::Type> argTypes({ srcRefType });
      static Core::Ast::Identifier ref({{S("value"), TiStr(S("~cast"))}});
      TypeMatchStatus retMatch;
      helper->getSeeker()->extForeach(&ref, srcContentType->getBody().get(),
        [=, &retMatch, &caster, &argTypes] (TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
        {
          if (action != Core::Ast::Seeker::Action::TARGET_MATCH) return Core::Ast::Seeker::Verb::MOVE;
          auto func = ti_cast<Function>(obj);
          if (func != 0) {
            auto funcType = func->getType().get();
            auto match = funcType->matchCall(&argTypes, helper);
            if (match == TypeMatchStatus::EXACT) {
              auto retType = funcType->traceRetType(helper);
              if (retType->getInitializationMethod(helper) != Ast::TypeInitMethod::NONE) {
                retType = helper->getReferenceTypeFor(retType, Ast::ReferenceMode::IMPLICIT);
              }
              Function *innerCaster;
              auto rmatch = helper->matchTargetType(retType, targetType, innerCaster);
              if ((rmatch>=TypeMatchStatus::CUSTOM_CASTER || rmatch==TypeMatchStatus::AGGREGATION) && rmatch>retMatch) {
                retMatch = rmatch;
                caster = func;
                if (rmatch == TypeMatchStatus::EXACT) return Core::Ast::Seeker::Verb::STOP;
              }
            }
          }
          return Core::Ast::Seeker::Verb::MOVE;
        },
        Core::Ast::Seeker::Flags::SKIP_OWNERS | Core::Ast::Seeker::Flags::SKIP_USES
      );
      return retMatch;
    }
  }
  return TypeMatchStatus::NONE;
}


Type* Helper::__traceType(TiObject *self, Core::Ast::Node *ref, Bool skipErrors)
{
  PREPARE_SELF(helper, Helper);

  SharedPtr<Core::Notices::Notice> notice;
  Core::Ast::Node *foundObj = 0;
  Spp::Ast::Type *type = 0;
  if (ref->isDerivedFrom<Spp::Ast::TypeOp>()) {
    auto typeOp = static_cast<Spp::Ast::TypeOp*>(ref);
    if (typeOp->getType() != 0) return typeOp->getType();
    auto operand = typeOp->getOperand().ti_cast_get<Core::Ast::Node>();
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
    Core::Ast::List list;
    SharedPtr<Core::Ast::Node> result;
    if (tpl->matchInstance(&list, helper, result, notice)) {
      type = result.ti_cast_get<Spp::Ast::Type>();
    }
  } else if (ref->isDerivedFrom<Core::Ast::Passage>()) {
    auto passage = static_cast<Core::Ast::Passage*>(ref);
    return helper->_traceType(passage->get(), skipErrors);
  } else if (ref->isDerivedFrom<Spp::Ast::Variable>()) {
    auto var = static_cast<Spp::Ast::Variable*>(ref);
    if (var->getType() != 0) return var->getType();
    if (!helper->resolveVarProps(var, skipErrors)) return 0;
    return var->getType();
  } else if (ref->isDerivedFrom<Spp::Ast::ArgPack>()) {
    // With arg packs we should already have the type set during the generation of the function signature.
    return getAstType(ref);
  } else if (ref->isDerivedFrom<Core::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Ast::Bracket*>(ref);
    if (bracket->getType() == Core::Ast::BracketType::ROUND) {
      return helper->_traceType(bracket->getOperand().get(), skipErrors);
    }
  } else if (helper->isAstReference(ref)) {
    type = tryGetAstType(ref);
    if (type != 0) return type;
    auto typeRef = static_cast<Core::Ast::Node*>(ref);
    auto owner = typeRef->getOwner();
    auto paramPass = ti_cast<Core::Ast::ParamPass>(ref);
    if (paramPass != 0 && paramPass->getType() == Core::Ast::BracketType::ROUND) {
      typeRef = paramPass->getOperand().ti_cast_get<Core::Ast::Node>();
      if (typeRef == 0) throw EXCEPTION(GenericException, S("Invalid type reference."));
      return helper->_traceType(typeRef, skipErrors);
    }
    helper->getSeeker()->extForeach(typeRef, owner,
      [=, &foundObj, &type, &notice](
        TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &seekNotice
      )->Core::Ast::Seeker::Verb
      {
        if (action == Core::Ast::Seeker::Action::ERROR) {
          notice = seekNotice;
          ASSERT(notice != 0);
          return Core::Ast::Seeker::Verb::MOVE;
        }
        if (action != Core::Ast::Seeker::Action::TARGET_MATCH) {
          return Core::Ast::Seeker::Verb::MOVE;
        }

        // Unbox if we have a box.
        auto passage = ti_cast<Core::Ast::Passage>(obj);
        if (passage != 0) foundObj = passage->get();
        else foundObj = obj;

        // Do we have a type?
        type = ti_cast<Spp::Ast::Type>(foundObj);
        if (type != 0) {
          return Core::Ast::Seeker::Verb::STOP;
        }

        // If we have a template then try to get a default instance.
        auto tpl = ti_cast<Spp::Ast::Template>(foundObj);
        if (tpl != 0) {
          Core::Ast::List list;
          list.setSourceLocation(Core::Ast::findSourceLocation(ref));
          SharedPtr<Core::Ast::Node> result;
          if (tpl->matchInstance(&list, helper, result, notice)) {
            type = result.ti_cast_get<Spp::Ast::Type>();
            if (type != 0) {
              return Core::Ast::Seeker::Verb::STOP;
            }
          }
        }

        return Core::Ast::Seeker::Verb::MOVE;
      }, 0
    );
    if (type != 0) setAstType(ref, type);
  }

  if (type == 0 && helper->rootManager->getNoticeStore() != 0 && !skipErrors) {
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    } else if (foundObj == 0) {
      helper->rootManager->getNoticeStore()->add(
        newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Ast::findSourceLocation(ref))
      );
    } else {
      helper->rootManager->getNoticeStore()->add(
        newSrdObj<Spp::Notices::IdentifierIsNotTypeNotice>(Core::Ast::findSourceLocation(ref))
      );
    }
  }

  return type;
}


Bool Helper::resolveVarProps(Variable *var, Bool skipErrors)
{
  if (var->getTypeRef() == 0) {
    // Deduction reports its own errors.
    return this->deduceVarType(var);
  } else {
    auto paramPass = var->getTypeRef().ti_cast<Core::Ast::ParamPass>();
    if (paramPass != 0 && paramPass->getType() == Core::Ast::BracketType::ROUND) {
      if (var->getInitArgs() != 0) {
        // Constructor args in the type ref, as in `var name: type(args) = value`, can't be combined with init args.
        if (!skipErrors) {
          this->getNoticeStore()->add(
            newSrdObj<Spp::Notices::VarCtorArgsWithValueNotice>(Core::Ast::findSourceLocation(var))
          );
          var->setTypeDeductionState(VarTypeDeductionState::FAILED);
        }
        return 0;
      }
      var->setInitArgs(paramPass->getParam());
      var->setTypeRef(paramPass->getOperand());
    }
    auto type = this->_traceType(var->getTypeRef().get(), skipErrors);
    var->setType(type);
    return type != 0;
  }
}


Bool Helper::deduceVarType(Variable *var)
{
  if (var->getTypeDeductionState() == VarTypeDeductionState::FAILED) return 0;
  if (var->getTypeDeductionState() == VarTypeDeductionState::DEDUCING) {
    this->getNoticeStore()->add(
      newSrdObj<Spp::Notices::CircularVarTypeDeductionNotice>(Core::Ast::findSourceLocation(var))
    );
    return false;
  }

  // Find the expression to deduce the type from.
  if (var->getInitArgs() == 0) {
    throw EXCEPTION(
      GenericException, S("Variable definition is missing both the type reference and the initial value.")
    );
  }
  Core::Ast::Node *astExpr;
  auto list = var->getInitArgs().ti_cast_get<Core::Ast::List>();
  if (list == 0) {
    astExpr = var->getInitArgs().get();
  } else if (list->getCount() == 1) {
    astExpr = list->getElement(0);
  } else {
    this->getNoticeStore()->add(
      newSrdObj<Spp::Notices::MultiArgVarTypeDeductionNotice>(Core::Ast::findSourceLocation(var))
    );
    var->setTypeDeductionState(VarTypeDeductionState::FAILED);
    return false;
  }

  // Compute the type of the expression. With an unresolved init mode the expression is either a value, which is
  // passed to the constructor, or the var's type. Either way the var is initialized by binding.
  Bool initModeUnresolved = var->getInitMode() == VarInitMode::UNRESOLVED;
  if (initModeUnresolved) var->setInitMode(VarInitMode::BIND);
  var->setTypeDeductionState(VarTypeDeductionState::DEDUCING);
  Core::Ast::Node *result;
  Bool resultIsValue;
  Type *type = this->deduceIntLiteralType(astExpr);
  if (type == 0) {
    if (this->expressionComputation->computeResultType(astExpr, result, resultIsValue)) {
      if (resultIsValue) {
        type = static_cast<Type*>(result);
        // Implicit references, which we get when reading a variable, are always dropped so the var gets a copy of the
        // value. Explicit references (e.g. returned by functions) are kept when passed to the constructor, which binds
        // the reference, but dropped when assigned, since assigning to a reference writes to the referenced value
        // rather than binding the reference.
        auto refType = ti_cast<ReferenceType>(type);
        if (refType != 0 && refType->isImplicit()) type = refType->getContentType(this);
        if (var->getInitMode() == VarInitMode::COPY) type = this->tryGetDeepReferenceContentType(type);
        if (this->isVoid(type)) type = 0;
      } else if (result != 0 && initModeUnresolved) {
        // The expression is the type of the var rather than its initial value (e.g. `def x: var Int`).
        type = this->traceType(result, true);
        if (type != 0) {
          // Clear the init args before setting the type ref, since clearing them disowns the node.
          auto typeRef = var->getInitArgs();
          var->setInitArgs(SharedPtr<Core::Ast::Node>());
          var->setTypeRef(typeRef);
        }
      }
      if (type == 0) {
        this->getNoticeStore()->add(
          newSrdObj<Spp::Notices::VarTypeDeductionFailedNotice>(Core::Ast::findSourceLocation(astExpr))
        );
      }
    }
  }
  var->setType(type);
  var->setTypeDeductionState(type == 0 ? VarTypeDeductionState::FAILED : VarTypeDeductionState::NONE);
  return type != 0;
}


Type* Helper::deduceIntLiteralType(Core::Ast::Node *astExpr)
{
  // Integer literals are given the smallest type that fits their value, which is good for implicit casting but not
  // as the type of a variable, so we give integer literals that have no explicit size at least 32 bits.
  while (astExpr->isDerivedFrom<Core::Ast::PrefixOperator>()) {
    auto prefixOp = static_cast<Core::Ast::PrefixOperator*>(astExpr);
    if (prefixOp->getType() != S("-") && prefixOp->getType() != S("+")) return 0;
    astExpr = prefixOp->getOperand().get();
  }
  auto literal = ti_cast<Core::Ast::IntegerLiteral>(astExpr);
  if (literal == 0) return 0;
  Str value = literal->getValue().get();
  LongWord _value;
  Int size;
  Bool signedNum;
  Bool typeRequested;
  this->parseIntLiteral(value, _value, size, signedNum, typeRequested);
  if (typeRequested) return 0;
  if (size < 32) size = 32;
  return signedNum ? static_cast<Type*>(this->getIntType(size)) : static_cast<Type*>(this->getWordType(size));
}


void Helper::parseIntLiteral(Char const *src, LongWord &value, Int &size, Bool &signedNum, Bool &typeRequested)
{
  // TODO: Consider non-English letters prefixes and postfixes in the literal.
  // TODO: Implement unsigned integers.

  // Parse the given value.
  Int base = 10;
  if (compareStr(src, S("0b"), 2) == 0 || compareStr(src, S("0B"), 2) == 0) {
    base = 2;
    src += 2;
  } else if (compareStr(src, S("0ن"), 3) == 0) {
    base = 2;
    src += 3;
  } else if (compareStr(src, S("0o"), 2) == 0 || compareStr(src, S("0O"), 2) == 0) {
    base = 8;
    src += 2;
  } else if (compareStr(src, S("0م"), 3) == 0) {
    base = 8;
    src += 3;
  } else if (
    compareStr(src, S("0h"), 2) == 0 || compareStr(src, S("0H"), 2) == 0 ||
    compareStr(src, S("0x"), 2) == 0 || compareStr(src, S("0X"), 2) == 0
  ) {
    base = 16;
    src += 2;
  }
  value = 0;
  while (
    (*src >= C('0') && *src <= C('9')) ||
    (*src >= C('a') && *src <= C('f')) ||
    (*src >= C('A') && *src <= C('F'))
  ) {
    Int digit = 0;
    if (*src >= C('0') && *src <= C('9')) digit = *src - C('0');
    else if (*src >= C('a') && *src <= C('f')) digit = *src - C('a') + 10;
    else if (*src >= C('A') && *src <= C('F')) digit = *src - C('A') + 10;
    ASSERT(digit < base);
    value *= base;
    value += digit;
    ++src;
  }

  // Is it a signed number?
  signedNum = true;
  if (*src == C('u') || *src == C('U')) {
    signedNum = false;
    ++src;
  } else if (compareStr(src, S("ط"), 2) == 0) {
    signedNum = false;
    src += 2;
  }

  // Determine integer size.
  typeRequested = false;
  size = 0;
  if (*src == C('i') || *src == C('I')) {
    typeRequested = true;
    ++src;
    if (getStrLen(src) > 0) size = std::stoi(src);
  } else if (compareStr(src, S("ص"), 2) == 0) {
    typeRequested = true;
    src += 2;
    if (getStrLen(src) > 0) size = std::stoi(src);
  }
  if (size == 0) {
    if (value == 0 || value == 1) {
      // Give special treatment to 0 and 1 and consider it unsigned.
      signedNum = false;
    }
    size = signedNum ?
      this->getNeededIntSize((LongInt)value) :
      this->getNeededWordSize(value);
  }
}


Core::Ast::Node* Helper::getVarInitArgs(Core::Ast::Node *astVar, Core::Ast::Node *&astTypeRef)
{
  astTypeRef = astVar;
  auto var = ti_cast<Variable>(astVar);
  if (var != 0) {
    this->resolveVarPropsIfNeeded(var);
    return var->getInitArgs().get();
  }
  // TODO: Remove the following once the Variable class is fully utilized as the
  //       exclusive way to define variables.
  auto paramPass = ti_cast<Core::Ast::ParamPass>(astVar);
  if (paramPass != 0 && paramPass->getType() == Core::Ast::BracketType::ROUND) {
    astTypeRef = paramPass->getOperand().get();
    return paramPass->getParam().get();
  }
  return 0;
}


Variable* Helper::addThisDefinition(
  Block *body, Char const *thisName, Type *astThisType, Bool skipInjection, Int &index
) {
  Core::Ast::Definition *def = 0;
  // Do we already have the definition?
  for (index = 0; index < body->getCount(); ++index) {
    auto tempDef = body->get(index).ti_cast_get<Core::Ast::Definition>();
    if (
      tempDef != 0 &&
      tempDef->getName() == thisName &&
      tempDef->getMetadata(S("__autovar")) != 0
    ) {
      def = tempDef;
      break;
    }
  }
  // We don't have the definition yet, so we'll create it.
  if (def == 0) {
    auto newDef = Core::Ast::Definition::create({
      {S("sourceLocation"), body->getSourceLocation()},
      {S("name"), TiStr(thisName)}
    });
    newDef->setMetadata(S("__autovar"), Core::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("1")) }}));
    if (!skipInjection) {
      newDef->setMetadata(S("injection"), Core::Ast::IntegerLiteral::create({{ S("value"), TiStr(S("1")) }}));
    }
    body->insert(0, newDef);
    index = 0;
    def = newDef.get();
  }
  // Now we can set the target of the new or existing definition. We reuse the existing var, if any, because the
  // definition may have been added earlier, in which case the var could already be referenced.
  auto var = def->getTarget().ti_cast_get<Variable>();
  if (var == 0) {
    def->setTarget(Variable::create({
      {S("valueOnly"), TiBool(true)},
      {S("sourceLocation"), body->getSourceLocation()}
    }, {
      {S("type"), astThisType}
    }));
    var = def->getTarget().ti_cast_get<Variable>();
  } else {
    var->setType(astThisType);
  }
  return var;
}


Bool Helper::_isVoid(TiObject *self, Core::Ast::Node const *ref)
{
  PREPARE_SELF(helper, Helper);

  if (ref == 0) return true;
  auto type = helper->traceType(const_cast<Core::Ast::Node*>(ref));
  if (type == 0) return false;
  return type->isA<VoidType>();
}


Bool Helper::_isCastableTo(
  TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Bool implicit
) {
  PREPARE_SELF(helper, Helper);
  Function *caster;
  if (implicit) {
    return helper->matchTargetType(srcTypeRef, targetTypeRef, caster) >= TypeMatchStatus::CUSTOM_CASTER;
  } else {
    return helper->matchTargetType(srcTypeRef, targetTypeRef, caster) >= TypeMatchStatus::EXPLICIT_CAST;
  }
}


TypeMatchStatus Helper::_matchTargetType(
  TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Function *&caster
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
  auto matchType = srcType->matchTargetType(targetType, helper);
  if (negativeDeref && (matchType == TypeMatchStatus::AGGREGATION || matchType >= TypeMatchStatus::REF_AGGREGATION)) {
    --matchType.derefs;
    if (matchType == TypeMatchStatus::AGGREGATION) matchType.value = TypeMatchStatus::REF_AGGREGATION;
    return matchType;
  } else if (!negativeDeref && matchType >= TypeMatchStatus::IMPLICIT_CAST) {
    return matchType;
  } else {
    if (!srcType->isDerivedFrom<ReferenceType>()) {
      auto srcReferenceType = helper->getReferenceTypeFor(srcType, Ast::ReferenceMode::TEMP_EXPLICIT);
      auto customMatchType = helper->lookupCustomCaster(srcReferenceType, targetType, caster);
      if (customMatchType >= TypeMatchStatus::IMPLICIT_CAST) {
        return TypeMatchStatus(customMatchType, TypeMatchStatus::CUSTOM_CASTER, -1);
      }
    }
    Int deref = 0;
    while (srcType->isDerivedFrom<ReferenceType>() && static_cast<ReferenceType*>(srcType)->isAutoDeref()) {
      auto customMatchType = helper->lookupCustomCaster(srcType, targetType, caster);
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


Bool Helper::_isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType)
{
  PREPARE_SELF(helper, Helper);

  auto referenceType = ti_cast<ReferenceType>(refType);
  if (referenceType == 0) return false;
  return referenceType->getContentType(helper)->isEqual(contentType, helper);
}


ReferenceType* Helper::_getReferenceTypeFor(TiObject *self, Core::Ast::Node *type, ReferenceMode const &mode)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getReferenceTemplate(mode);

  SharedPtr<Core::Ast::Node> result;
  SharedPtr<Core::Notices::Notice> notice;
  if (tpl->matchInstance(type, helper, result, notice)) {
    auto refType = result.ti_cast_get<ReferenceType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for reference type is invalid."));
    }
    return refType;
  } else {
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


PointerType* Helper::_getPointerTypeFor(TiObject *self, Core::Ast::Node *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getPointerTemplate();

  SharedPtr<Core::Ast::Node> result;
  SharedPtr<Core::Notices::Notice> notice;
  if (tpl->matchInstance(type, helper, result, notice)) {
    auto refType = result.ti_cast_get<PointerType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for pointer type is invalid."));
    }
    return refType;
  } else {
    if (notice != 0) {
      helper->rootManager->getNoticeStore()->add(notice);
    }
    return 0;
  }
}


ArrayType* Helper::_getArrayTypeFor(TiObject *self, Core::Ast::Node *type)
{
  PREPARE_SELF(helper, Helper);

  auto tpl = helper->getArrayTemplate();

  SharedPtr<Core::Ast::Node> result;
  SharedPtr<Core::Notices::Notice> notice;
  if (tpl->matchInstance(type, helper, result, notice)) {
    auto refType = result.ti_cast_get<ArrayType>();
    if (refType == 0) {
      throw EXCEPTION(GenericException, S("Template for array type is invalid."));
    }
    return refType;
  } else {
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


Type* Helper::_getValueTypeFor(TiObject *self, Core::Ast::Node *typeRef)
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
    auto typeRef = helper->rootManager->parseExpression(S("Null")).s_cast<Core::Ast::Identifier>();
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
      .s_cast<Core::Ast::ParamPass>();
    helper->charArrayTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->charArrayTypeRef
      ->getParam().ti_cast_get<Core::Ast::List>()
      ->get(1).ti_cast_get<Core::Ast::IntegerLiteral>();
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
      .s_cast<Core::Ast::ParamPass>();
    helper->integerTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->integerTypeRef->getParam().ti_cast_get<Core::Ast::IntegerLiteral>();
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
      .s_cast<Core::Ast::ParamPass>();
    helper->wordTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->wordTypeRef->getParam().ti_cast_get<Core::Ast::IntegerLiteral>();
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
      .s_cast<Core::Ast::ParamPass>();
    helper->floatTypeRef->setOwner(helper->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = helper->floatTypeRef->getParam().ti_cast_get<Core::Ast::IntegerLiteral>();
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
    auto typeRef = helper->rootManager->parseExpression(S("Void")).s_cast<Core::Ast::Identifier>();
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


UserType* Helper::_getNodeType(TiObject *self)
{
  PREPARE_SELF(helper, Helper);
  if (helper->nodeType == 0) {
    auto typeRef = helper->rootManager->parseExpression(S("Core.Ast.Node"));
    typeRef->setOwner(helper->rootManager->getRootScope().get());

    helper->nodeType = ti_cast<UserType>(helper->getSeeker()->tryGet(
      typeRef.get(), helper->rootManager->getRootScope().get()
    ));
  }
  return helper->nodeType;
}


Str Helper::_resolveNodePath(TiObject *self, Core::Ast::Node const *node)
{
  PREPARE_SELF(helper, Helper);
  return helper->nodePathResolver->doResolve(node, helper);
}


Str const& Helper::_getFunctionName(TiObject *self, Function *astFunc)
{
  static LongInt anonymousFuncIndex = 0;
  if (astFunc->getName().getStr() == S("")) {
    if (ti_cast<Core::Ast::Definition>(astFunc->getOwner()) == 0) {
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


DefinitionDomain Helper::_getVariableDomain(TiObject *self, Core::Ast::Node const *obj)
{
  // Find the definition.
  auto def = ti_cast<Core::Ast::Definition const>(obj);
  if (def == 0) {
    def = ti_cast<Core::Ast::Definition const>(obj->getOwner());
    if (def == 0) {
      // This could be a function arg.
      if (
        obj->getOwner() != 0 && obj->getOwner()->getOwner() != 0 &&
        obj->getOwner()->getOwner()->isDerivedFrom<FunctionType>()
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
        owner->getOwner()->isDerivedFrom<UseInOp>() || owner->getOwner()->isDerivedFrom<Core::Ast::LinkOperator>()
      ) {
        return DefinitionDomain::FUNCTION;
      }
    }
    owner = owner->getOwner();
  }
  return DefinitionDomain::GLOBAL;
}


Bool Helper::doesModifierExistOnDef(Core::Ast::Definition const *def, Char const *name)
{
  auto modifiers = def->getModifiers().get();
  if (modifiers != 0) {
    for (Int i = 0; i < modifiers->getElementCount(); ++i) {
      auto identifier = ti_cast<Core::Ast::Identifier>(modifiers->getElement(i));
      if (identifier != 0 && identifier->getValue() == name) return true;
    }
  }
  return false;
}


Bool Helper::_validateUseStatement(TiObject *self, Core::Ast::Bridge *bridge)
{
  PREPARE_SELF(helper, Helper);
  VALIDATE_NOT_NULL(bridge);
  if (bridge->getTarget() == 0) {
    throw EXCEPTION(InvalidArgumentException, S("bridge"), S("Use statement has a null target."));
  }
  Bool found = false;
  helper->getSeeker()->foreach(bridge->getTarget().get(), bridge->getOwner(),
    [=, &found] (TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
    {
      if (action != Core::Ast::Seeker::Action::TARGET_MATCH) return Core::Ast::Seeker::Verb::MOVE;
      if (ti_cast<Ast::Module>(obj) != 0) {
        found = true;
        return Core::Ast::Seeker::Verb::STOP;
      } else {
        return Core::Ast::Seeker::Verb::MOVE;
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

  Core::Ast::Identifier identifier;
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

  Core::Ast::Identifier identifier;
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

  Core::Ast::Identifier identifier;
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
