/**
 * @file Spp/Ast/CalleeTracer.cpp
 * Contains the implementation of class Spp::Ast::CalleeTracer.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Initialization Functions

void CalleeTracer::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->lookupCallee,
    &this->lookupCallee_routing,
    &this->lookupCallee_function,
    &this->lookupCallee_type,
    &this->lookupCallee_template,
    &this->lookupCallee_scope,
    &this->lookupCallee_argPack,
    &this->lookupCallee_var,
    &this->lookupCallee_literal,
    &this->lookupCallee_funcPtr,
    &this->lookupCallee_customOp,
    &this->lookupCallee_builtInOp
  });
}


void CalleeTracer::initBindings()
{
  this->lookupCallee = &CalleeTracer::_lookupCallee;
  this->lookupCallee_routing = &CalleeTracer::_lookupCallee_routing;
  this->lookupCallee_function = &CalleeTracer::_lookupCallee_function;
  this->lookupCallee_type = &CalleeTracer::_lookupCallee_type;
  this->lookupCallee_template = &CalleeTracer::_lookupCallee_template;
  this->lookupCallee_scope = &CalleeTracer::_lookupCallee_scope;
  this->lookupCallee_argPack = &CalleeTracer::_lookupCallee_argPack;
  this->lookupCallee_var = &CalleeTracer::_lookupCallee_var;
  this->lookupCallee_literal = &CalleeTracer::_lookupCallee_literal;
  this->lookupCallee_funcPtr = &CalleeTracer::_lookupCallee_funcPtr;
  this->lookupCallee_customOp = &CalleeTracer::_lookupCallee_customOp;
  this->lookupCallee_builtInOp = &CalleeTracer::_lookupCallee_builtInOp;
}


//==============================================================================
// Main Functions

void CalleeTracer::_lookupCallee(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.ref != 0) {
    auto target = request.target;
    Int tracingAlias = 0;
    Bool searchingFunctionOwners = false;
    auto callback = [=, &request, &result, &target, &tracingAlias, &searchingFunctionOwners]
      (TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
      {
        if (action == Core::Data::Seeker::Action::ERROR) {
          if (result.isNew()) result.notice = getSharedPtr(ti_cast<Core::Notices::Notice>(obj));
          ASSERT(result.notice != 0);
          return Core::Data::Seeker::Verb::MOVE;
        } else if (action == Core::Data::Seeker::Action::OWNER_SCOPE) {
          if (tracingAlias == 0 || result.isNameMatched()) return Core::Data::Seeker::Verb::SKIP_GROUP;
          else return Core::Data::Seeker::Verb::MOVE;
        } else if (action == Core::Data::Seeker::Action::USE_SCOPES_START) {
          if (tracingAlias == 0 || result.isNameMatched()) return Core::Data::Seeker::Verb::SKIP;
          else return Core::Data::Seeker::Verb::MOVE;
        } else if (action == Core::Data::Seeker::Action::ALIAS_TRACE_START) {
          ++tracingAlias;
          return Core::Data::Seeker::Verb::MOVE;
        } else if (action == Core::Data::Seeker::Action::ALIAS_TRACE_END) {
          --tracingAlias;
          return Core::Data::Seeker::Verb::MOVE;
        }

        if (tracer->helper->isVariable(obj)) {
          auto domain = tracer->helper->getVariableDomain(obj);
          
          // If we are going out of the current function then we should only be able to access global variables.
          if (domain == Ast::DefinitionDomain::FUNCTION && searchingFunctionOwners) {
            result.notice = newSrdObj<Spp::Notices::AccessingLocalVarInOtherFuncNotice>(
              Core::Data::Ast::findSourceLocation(request.astNode)
            );
            return Core::Data::Seeker::Verb::STOP;
          }

          // If we are not requesting directly accessible members and instead looking for object or scope members and
          // we encounter local variables or values (like the auto created this) then we should skip that. This is to
          // avoid cases where an auto created local variable (for example: this) is unintentionally picked up while
          // looking through object members.
          if (domain == DefinitionDomain::FUNCTION && request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE) {
            return Core::Data::Seeker::Verb::MOVE;
          }
        }

        if (action != Core::Data::Seeker::Action::TARGET_MATCH || obj == 0) return Core::Data::Seeker::Verb::MOVE;

        // Unbox if we have a box.
        if (obj->isA<TioWeakBox>()) {
          obj = static_cast<TioWeakBox*>(obj)->get().get();
        } else if (obj->isA<TioSharedBox>()) {
          obj = static_cast<TioSharedBox*>(obj)->get().get();
        }

        if (
          result.stack.getLength() > 0 && result.stack(result.stack.getLength() - 1).obj == obj
        ) {
          return Core::Data::Seeker::Verb::MOVE;
        }

        auto node = ti_cast<Core::Data::Node>(obj);
        if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
          if (node == 0 || node->findOwner<Type>() != target) {
            // The found member was probably an alias to a non member.
            result.notice = newSrdObj<Spp::Notices::InvalidTypeMemberNotice>(
              Core::Data::Ast::findSourceLocation(request.astNode)
            );
            return Core::Data::Seeker::Verb::MOVE;
          }
        }

        CalleeLookupRequest innerRequest = request;
        CalleeLookupResult innerResult;
        innerRequest.target = obj;
        tracer->lookupCallee_routing(innerRequest, innerResult);
        CalleeTracer::selectBetterResult(innerResult, result);
        if (
          result.notice != 0 &&
          result.notice->isDerivedFrom<Spp::Notices::MultipleCalleeMatchNotice>() &&
          result.matchStatus == TypeMatchStatus::EXACT
        ) {
          // There is no need to continue searching if we found multiple exact matches.
          return Core::Data::Seeker::Verb::STOP;
        }

        return Core::Data::Seeker::Verb::MOVE;
      };

    while (target != 0) {
      // Search the requested scope.
      tracingAlias = 0;
      tracer->getSeeker()->foreach(request.ref, target, callback, 0);

      // Try the injections if we don't have an exact match yet.
      if (!request.skipInjections && result.matchStatus < TypeMatchStatus::EXACT) {
        Block *block = 0;
        auto helper = tracer->helper;
        // Special handling for Type targets, but it's only needed if we are at the top of the stack to avoid re-testing
        // the same block again.
        auto dataType = target == request.target ? ti_cast<DataType>(target) : 0;
        if (dataType != 0) {
          block = dataType->getBody().get();
        } else {
          block = ti_cast<Block>(target);
        }
        if (block != 0) {
          for (Int i = 0; i < block->getInjectionCount(); ++i) {
            auto def = ti_cast<Core::Data::Ast::Definition>(block->getInjection(i));
            if (def == 0 || def->getTarget() == 0) continue;
            if (!helper->isVariable(def->getTarget().get())) continue;
            auto domain = helper->getVariableDomain(def);

            // If we are not requesting directly accessible members and instead looking for object or scope members and
            // we encounter local variables or values (like the auto created this) then we should skip that. This is to
            // avoid cases where an auto created local variable (for example: this) is unintentionally picked up while
            // looking through object members.
            if (domain == DefinitionDomain::FUNCTION && request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE) continue;

            // If we are going out of the current function then we should ignore local variables of outer functions.
            if (domain == DefinitionDomain::FUNCTION && searchingFunctionOwners) continue;

            Bool isMember = domain == DefinitionDomain::OBJECT;
            if ((isMember && request.thisType == 0) || (!isMember && request.thisType != 0)) continue;
            auto objType = helper->traceType(def->getTarget().get());
            if (objType == 0) continue;
            auto refType = helper->getReferenceTypeFor(objType, Ast::ReferenceMode::IMPLICIT);
            objType = helper->tryGetDeepReferenceContentType(objType);
            Bool noBindDef = helper->isNoBindDef(def);
            CalleeLookupRequest innerRequest = request;
            innerRequest.target = objType;
            innerRequest.mode = CalleeLookupMode::OBJECT_MEMBER;
            if (!noBindDef) innerRequest.thisType = refType;
            CalleeLookupResult innerResult;
            tracer->lookupCallee(innerRequest, innerResult);
            if (innerResult.isSuccessful()) {
              if (!noBindDef) innerResult.pushThisMarker();
              innerResult.pushObject(def->getTarget().get());
              ++innerResult.injectionLevel;
            }
            CalleeTracer::selectBetterResult(innerResult, result);
            // No need to keep looking if we already found an exact match.
            if (result.matchStatus == TypeMatchStatus::EXACT) break;
          }
        }
      }

      // Try scope bridges if we haven't found any name match.
      if (!result.isNameMatched() && request.mode == CalleeLookupMode::DIRECTLY_ACCESSIBLE) {
        Core::Data::Ast::Scope *scope = 0;
        // Special handling for Type targets, but it's only needed if we are at the top of the stack to avoid re-testing
        // the same scope again.
        auto dataType = target == request.target ? ti_cast<DataType>(target) : 0;
        if (dataType != 0) {
          scope = dataType->getBody().get();
        } else {
          scope = ti_cast<Core::Data::Ast::Scope>(target);
        }
        if (scope != 0) {
          for (Int i = 0; i < scope->getBridgeCount(); ++i) {
            auto bridgeRef = scope->getBridge(i);
            // If the thing we are looking for is actually this bridgeRef, then we need to skip, otherwise we end up in
            // an infinite loop.
            if (bridgeRef->getTarget() == request.ref) continue;
            auto bridgeTarget = tracer->getSeeker()->tryGet(
              bridgeRef->getTarget().get(), scope,
              Core::Data::Seeker::Flags::SKIP_USES | Core::Data::Seeker::Flags::SKIP_USES_FOR_ALIASES
            );
            if (bridgeTarget == 0) continue;

            tracingAlias = 0;
            auto verb = tracer->getSeeker()->foreach(request.ref, bridgeTarget, callback, 0);
            if (!Core::Data::Seeker::isMove(verb)) break;
          }
        }
      }

      if (request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE || result.isNameMatched()) break;

      if (ti_cast<Ast::Function>(target) != 0) searchingFunctionOwners = true;

      target = ti_cast<Core::Data::Node>(target)->getOwner();
      if (ti_cast<Ast::DataType>(target) != 0) {
        // Skip the type object itself.
        target = static_cast<Core::Data::Node*>(target)->getOwner();
      }
    }
  } else {
    tracer->lookupCallee_routing(request, result);
  }

  // Did we have a matched callee?
  if (!result.isSuccessful()) {
    // If we have no notice it means the symbol was not found in the first place.
    if (result.notice == 0) {
      // If no ref symbol was provided it means we are applying an operation on a reuslt rather than searching by
      // symbol, so in this case using InvalidOperationNotice is more appropriate than UnknownSymbolNotice.
      if (request.ref == 0) {
        result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      } else {
        result.notice = newSrdObj<Spp::Notices::UnknownSymbolNotice>();
      }
    }

    if (result.notice != 0) result.notice->setSourceLocation(Core::Data::Ast::findSourceLocation(
      request.ref != 0 ? request.ref : request.astNode
    ));
  }
}


void CalleeTracer::_lookupCallee_routing(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  if (request.templateParam != 0 && ti_cast<Template>(request.target) == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  if (request.target != 0 && request.target->isDerivedFrom<Ast::Function>()) {
    tracer->lookupCallee_function(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Type>()) {
    if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
      auto funcType = helper->tryGetPointerContentType<FunctionType>(request.target);
      if (funcType != 0) {
        tracer->lookupCallee_funcPtr(request, result);
      } else if (ti_cast<UserType>(request.target) != 0) {
        tracer->lookupCallee_customOp(request, result);
      }
      if (result.isSuccessful()) return;
      tracer->lookupCallee_builtInOp(request, result);
    } else {
      tracer->lookupCallee_type(request, result);
    }
  } else if (request.target != 0 && request.target->isDerivedFrom<Template>()) {
    tracer->lookupCallee_template(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Core::Data::Ast::Scope>()) {
    tracer->lookupCallee_scope(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<ArgPack>()) {
    tracer->lookupCallee_argPack(request, result);
  } else if (request.target != 0 && helper->isVariable(request.target)) {
    tracer->lookupCallee_var(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    tracer->lookupCallee_literal(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    tracer->lookupCallee_literal(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    tracer->lookupCallee_literal(request, result);
  } else {
    // Invalid
    if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  }
}


void CalleeTracer::_lookupCallee_function(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  ContainerExtender<TiObject, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  auto func = static_cast<Ast::Function*>(request.target);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  // TODO: Should we skip if mode is not OBJECT_MEMBER and the function is a member function?

  if (!func->getType()->isMember() && request.thisType != 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidGlobalDefAccessNotice>(
      Core::Data::Ast::findSourceLocation(request.target)
    );
    return;
  }

  Bool useThis = request.thisType != 0;

  // Only allow this check if the funciton has the proper modifier.
  Char const *defOp = 0;
  auto def = func->findOwner<Core::Data::Ast::Definition>();
  if (def != 0) {
    defOp = findOperationModifier(def);
  }
  if (defOp != 0 && SBSTR(defOp) == S("")) {
    PlainList<TiObject> argTypes;
    if (useThis) argTypes.add(request.thisType);
    auto matchStatus = func->getType()->matchCall(&argTypes, helper);
    if (matchStatus < TypeMatchStatus::CUSTOM_CASTER) {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    }

    if (request.op == S("")) {
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.pushThisMarker();
      result.pushFunctionCall();
      result.pushObject(request.target);
    } else if (CalleeTracer::isAssignOp(request.op)) {
      // This is a read property, so we shouldn't allow assign operations on it.
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    } else {
      auto retType = helper->tryGetDeepReferenceContentType(func->getType()->traceRetType(helper));
      CalleeLookupRequest innerRequest = request;
      innerRequest.mode = CalleeLookupMode::OBJECT_MEMBER;
      innerRequest.target = retType;
      if (retType->isDerivedFrom<UserType>()) {
        innerRequest.thisType = helper->getReferenceTypeFor(retType, ReferenceMode::IMPLICIT);
      }
      tracer->lookupCallee_routing(innerRequest, result);

      if (result.isSuccessful()) {
        if (retType->isDerivedFrom<UserType>()) result.pushThisMarker();
        result.pushFunctionCall();
        result.pushObject(request.target);
      }
    }
    return;
  } else if (request.op == S("") && request.mode != CalleeLookupMode::OBJECT_MEMBER) {
    // We are just trying to get a reference to the function itself.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.pushObject(func);
    return;
  } else if (request.op != (defOp == 0 ? S("()") : defOp)) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  result.matchStatus = func->getType()->matchCall(useThis ? &extTypes : request.argTypes, helper);
  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
    result.pushObject(func);
  } else {
    result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
  }
}


void CalleeTracer::_lookupCallee_type(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  auto type = static_cast<Type*>(request.target);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  } else if (request.op == S("()")) {
    // Look for a () operator on the Type itself.
    static Core::Data::Ast::Identifier ref;
    CalleeLookupRequest innerRequest = request;
    innerRequest.ref = &ref;
    innerRequest.mode = CalleeLookupMode::SCOPE_MEMBER;
    ref.setValue(S("()"));
    innerRequest.op = S("()");
    innerRequest.target = type;
    innerRequest.thisType = 0;
    tracer->lookupCallee(innerRequest, result);
    if (result.isSuccessful()) {
      return;
    }

    // Look for a constructor.
    ref.setValue(S("~init"));
    innerRequest.op = S("~init");
    innerRequest.target = type;
    innerRequest.thisType = helper->getReferenceTypeFor(type, Ast::ReferenceMode::IMPLICIT);
    innerRequest.mode = CalleeLookupMode::OBJECT_MEMBER;
    innerRequest.skipInjections = true;
    tracer->lookupCallee(innerRequest, result);
    if (result.isSuccessful()) {
      result.pushObject(type);
      return;
    }

    Function *customCaster;
    if (
      request.argTypes->getElementCount() == 0 &&
      (type->getInitializationMethod(helper) & TypeInitMethod::USER) == 0
    ) {
      // No user-defined constructor is defined and no params is provided, so we can skip the matching.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.pushObject(type);
    } else if (
      (type->getInitializationMethod(helper) & TypeInitMethod::USER) == 0 &&
      request.argTypes->getElementCount() == 1 &&
      helper->matchTargetType(request.argTypes->getElement(0), type, customCaster) >= TypeMatchStatus::CUSTOM_CASTER
    ) {
      // A type with no custom init but can be initialized from the given arg.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.pushObject(type);
    } else {
      // If the ref symbol we provided here was not found we'll provide a better error message to the user since
      // the user didn't manually provide this symbol.
      if (result.notice != 0 && result.notice->isA<Spp::Notices::UnknownSymbolNotice>()) {
        result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      }
    }
  } else if (request.op == S("")) {
    // We are just trying to get a reference to the type itself.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.pushObject(type);
  } else {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  }
}


void CalleeTracer::_lookupCallee_template(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.op == S("[]")) {
    // We want the template itself.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.pushObject(request.target);
    return;
  }

  auto tpl = static_cast<Ast::Template*>(request.target);
  Core::Data::Ast::List list;
  TiObject *tplParam = request.templateParam != 0 ? request.templateParam : &list;
  TioSharedPtr instance;
  if (!tpl->matchInstance(tplParam, tracer->helper, instance)) {
    result.notice = instance.s_cast<Core::Notices::Notice>();
  } else {
    CalleeLookupRequest innerRequest = request;
    innerRequest.templateParam = 0;
    innerRequest.target = instance.get();
    tracer->lookupCallee_routing(innerRequest, result);
  }
}


void CalleeTracer::_lookupCallee_scope(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  if (request.op != S("") || request.mode == CalleeLookupMode::OBJECT_MEMBER) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  } else {
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.pushObject(request.target);
  }
}


void CalleeTracer::_lookupCallee_argPack(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.op != S("")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  result.matchStatus = TypeMatchStatus::EXACT;
  result.notice.reset();
  result.stack.clear();
  result.pushObject(request.target);
}


void CalleeTracer::_lookupCallee_var(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  auto def = ti_cast<Core::Data::Ast::Definition>(static_cast<Core::Data::Node*>(request.target)->getOwner());

  auto objType = ti_cast<Type>(helper->traceType(request.target));
  if (objType == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(request.target));
    return;
  }
  Bool targetIsRef = false;
  if (objType->isDerivedFrom<ReferenceType>()) {
    targetIsRef = true;
    objType = helper->tryGetDeepReferenceContentType(objType);
  } else if (helper->isInMemVariable(request.target)) {
    // If this is an in-mem variable (i.e. not a value) then it will always be a reference.
    targetIsRef = true;
  }
  if (objType == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(request.target));
    return;
  }

  if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
    if (helper->getVariableDomain(request.target) != Ast::DefinitionDomain::OBJECT) {
      result.notice = newSrdObj<Spp::Notices::InvalidGlobalDefAccessNotice>(
        Core::Data::Ast::findSourceLocation(request.target)
      );
      return;
    }
  } else {
    if (helper->getVariableDomain(request.target) == Ast::DefinitionDomain::OBJECT) {
      result.notice = newSrdObj<Spp::Notices::InvalidObjectMemberAccessNotice>(
        Core::Data::Ast::findSourceLocation(request.target)
      );
      return;
    }
  }

  Bool noBindDef = def != 0 && helper->isNoBindDef(def);

  CalleeLookupRequest innerRequest = request;
  innerRequest.varTargetOp = def == 0 ? 0 : Ast::findOperationModifier(def);
  innerRequest.mode = CalleeLookupMode::OBJECT_MEMBER;
  innerRequest.target = objType;
  if (!noBindDef && objType->isDerivedFrom<UserType>() && targetIsRef) {
    innerRequest.thisType = helper->getReferenceTypeFor(objType, ReferenceMode::IMPLICIT);
  }
  tracer->lookupCallee_routing(innerRequest, result);

  if (result.isSuccessful()) {
    if (!noBindDef && objType->isDerivedFrom<UserType>()) result.pushThisMarker();
    result.pushObject(request.target);
  }
}


void CalleeTracer::_lookupCallee_literal(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  if (request.op != S("()")) {
    // Accept any other operator and let the caller handle the built in type checking.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.stack.clear();
    if (request.op != S("")) {
      result.pushObject(getDummyBuiltInOpFunction());
      result.pushThisMarker();
    }
    result.pushObject(request.target);
    result.notice.reset();
  }
}


void CalleeTracer::_lookupCallee_funcPtr(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  // TODO: Should we skip if mode is not OBJECT_MEMBER and the function is a member function?

  ContainerExtender<TiObject, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  auto funcType = helper->tryGetPointerContentType<FunctionType>(request.target);
  Bool useThis = funcType->isMember() && request.thisType != 0;

  if (request.varTargetOp != 0 && SBSTR(request.varTargetOp) == S("")) {
    PlainList<TiObject> argTypes;
    if (useThis) argTypes.add(request.thisType);
    auto matchStatus = funcType->matchCall(&argTypes, helper);
    if (matchStatus < TypeMatchStatus::CUSTOM_CASTER) {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    }

    if (request.op == S("")) {
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.pushThisMarker();
      result.pushFunctionCall();
    } else if (CalleeTracer::isAssignOp(request.op)) {
      // This is a read property, so we shouldn't allow assign operations on it.
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    } else {
      auto retType = helper->tryGetDeepReferenceContentType(funcType->traceRetType(helper));
      CalleeLookupRequest innerRequest = request;
      innerRequest.mode = CalleeLookupMode::OBJECT_MEMBER;
      innerRequest.target = retType;
      innerRequest.varTargetOp = 0;
      if (retType->isDerivedFrom<UserType>()) {
        innerRequest.thisType = helper->getReferenceTypeFor(retType, ReferenceMode::IMPLICIT);
      }
      tracer->lookupCallee_routing(innerRequest, result);

      if (result.isSuccessful()) {
        if (retType->isDerivedFrom<UserType>()) result.pushThisMarker();
        result.pushFunctionCall();
      }
    }
    return;
  } else if (request.op == S("") && request.varTargetOp == 0) {
    // We are just trying to get a reference to the function ptr itself.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    return;
  } else if (request.op != (request.varTargetOp == 0 ? S("()") : request.varTargetOp)) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  result.matchStatus = funcType->matchCall(useThis ? &extTypes : request.argTypes, helper);

  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
  } else {
    result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
  }
}


void CalleeTracer::_lookupCallee_customOp(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  Core::Data::Ast::Identifier ref({ {S("value"), TiStr(request.op)} });
  CalleeLookupRequest innerRequest = request;
  innerRequest.ref = &ref;
  tracer->lookupCallee(innerRequest, result);
  if (!result.isSuccessful()) {
    // If the ref symbol we provided here was not found we'll provide a better error message to the user since
    // the user didn't manually provide this symbol.
    if (result.notice != 0 && result.notice->isA<Spp::Notices::UnknownSymbolNotice>()) {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
  }
}


void CalleeTracer::_lookupCallee_builtInOp(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  auto type = static_cast<Type*>(request.target);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  } else if (request.op == S("()")) {
    // Type must be an array for () operator.
    if (type->isDerivedFrom<ArrayType>()) {
      // We have an array.
      if (
        request.argTypes != 0 && request.argTypes->getElementCount() == 1 &&
        helper->isCastableTo(request.argTypes->getElement(0), helper->getArchIntType(), true)
      ) {
        result.matchStatus = TypeMatchStatus::EXACT;
        result.stack.clear();
        result.notice.reset();
      } else {
        if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
        return;
      }
    } else {
      if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    }
  } else if (request.op == S("=")) {
    // Type must be castable to the target type for assignment operators.
    if (request.argTypes != 0 && request.argTypes->getElementCount() == 1) {
      // Type must also have no custom initialization.
      if (type->getInitializationMethod(helper) == Ast::TypeInitMethod::NONE) {
        Function *caster;
        auto ms = helper->matchTargetType(request.argTypes->getElement(0), type, caster);
        if (ms >= TypeMatchStatus::CUSTOM_CASTER) {
          result.matchStatus = ms;
          result.stack.clear();
          result.notice.reset();
          result.pushObject(getDummyBuiltInOpFunction());
          result.pushThisMarker();
        } else {
          if (result.notice == 0 || result.notice->isA<Spp::Notices::InvalidOperationNotice>()) {
            result.notice = newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>();
          }
          return;
        }
      } else {
        if (result.notice == 0 || result.notice->isA<Spp::Notices::InvalidOperationNotice>()) {
          result.notice = newSrdObj<Spp::Notices::TypeMissingAssignOpNotice>();
        }
        return;
      }
    } else {
      if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    }
  } else if (request.varTargetOp == 0) {
    // Accept any other operator and let the caller handle the built in type checking.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.stack.clear();
    result.notice.reset();
    if (request.op != S("")) {
      result.pushObject(getDummyBuiltInOpFunction());
      result.pushThisMarker();
    }
  } else {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }
}


//==============================================================================
// Helper Functions

void CalleeTracer::selectBetterResult(CalleeLookupResult const &newResult, CalleeLookupResult &result)
{
  if (newResult.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    // It was a successful match.
    if (newResult.matchStatus > result.matchStatus) {
      // Better result is found.
      result = newResult;
    } else if (newResult.matchStatus == result.matchStatus) {
      if (newResult.injectionLevel < result.injectionLevel) {
        result = newResult;
      } else if (newResult.injectionLevel == result.injectionLevel) {
        // If the results are identical it means we have reached the same target twice, which can happen if the user has
        // an alias at a parent scope with the same name pointing to the same element. In this case we ignore the second
        // match without raising an error.
        Int diffPoint = -1;
        for (Int i = 0; i < newResult.stack.getLength(); ++i) {
          if (newResult.stack(i) != result.stack(i)) {
            diffPoint = i;
            break;
          }
        }
        if (diffPoint != -1) {
          // Multiple callee match.
          result.notice = newSrdObj<Spp::Notices::MultipleCalleeMatchNotice>(
            Core::Data::Ast::findSourceLocation(result.stack(diffPoint).obj),
            Core::Data::Ast::findSourceLocation(newResult.stack(diffPoint).obj)
          );
        } else if (newResult.stack.getLength() != result.stack.getLength()) {
          // Multiple callee match.
          result.notice = newSrdObj<Spp::Notices::MultipleCalleeMatchNotice>(
            Core::Data::Ast::findSourceLocation(result.stack(result.stack.getLength() - 1).obj),
            Core::Data::Ast::findSourceLocation(newResult.stack(newResult.stack.getLength() - 1).obj)
          );
        }
      }
    }
  } else if (!result.isNameMatched()) {
    // It was a failed match and we don't have a previous failed match, so we'll take this match.
    result = newResult;
  }
}


Bool CalleeTracer::isAssignOp(Str const &op)
{
  Char const *assignOps[] = {
    S("="), S("+="), S("-="), S("*="), S("/="), S("%="), S("&="), S("|="), S("$="), S("<<="), S(">>=")
  };
  for (Int i = 0; i < sizeof(assignOps) / sizeof(assignOps[0]); ++i) {
    if (op == assignOps[i]) return true;
  }
  return false;
}

} // namespace
