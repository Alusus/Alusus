/**
 * @file Spp/Ast/CalleeTracer.cpp
 * Contains the implementation of class Spp::Ast::CalleeTracer.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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
    auto callback = [=, &request, &result, &target, &tracingAlias]
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

        if (action != Core::Data::Seeker::Action::TARGET_MATCH || obj == 0) return Core::Data::Seeker::Verb::MOVE;

        // Unbox if we have a box.
        auto box = ti_cast<TioWeakBox>(obj);
        if (box != 0) obj = box->get().get();

        if (
          result.stack.getLength() > 0 && result.stack(result.stack.getLength() - 1) == obj &&
          result.thisIndex < 0
        ) {
          return Core::Data::Seeker::Verb::MOVE;
        }

        auto node = ti_cast<Core::Data::Node>(obj);
        if (request.targetIsObject) {
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
      if (result.matchStatus < TypeMatchStatus::EXACT) {
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
            if (!helper->isAstReference(def->getTarget().get())) continue;
            Bool isMember = helper->getVariableDomain(def) == DefinitionDomain::OBJECT;
            if ((isMember && request.thisType == 0) || (!isMember && request.thisType != 0)) continue;
            auto objType = helper->traceType(def->getTarget().get());
            if (objType == 0) continue;
            auto refType = helper->getReferenceTypeFor(objType, Ast::ReferenceMode::IMPLICIT);
            objType = helper->tryGetDeepReferenceContentType(objType);
            Bool noBindDef = helper->isNoBindDef(def);
            CalleeLookupRequest innerRequest = request;
            innerRequest.target = objType;
            innerRequest.searchTargetOwners = false;
            innerRequest.targetIsObject = true;
            if (!noBindDef) innerRequest.thisType = refType;
            CalleeLookupResult innerResult;
            tracer->lookupCallee(innerRequest, innerResult);
            if (innerResult.isSuccessful()) {
              innerResult.pushStack(def->getTarget().get());
              if (innerResult.thisIndex == -1) {
                innerResult.thisIndex = noBindDef ? -1 : 0;
              }
            }
            CalleeTracer::selectBetterResult(innerResult, result);
            // No need to keep looking if we already found an exact match.
            if (result.matchStatus == TypeMatchStatus::EXACT) break;
          }
        }
      }

      // Try scope bridges if we haven't found any name match.
      if (!result.isNameMatched() && !request.targetIsObject && request.searchTargetOwners) {
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

      if (!request.searchTargetOwners || result.isNameMatched()) break;

      target = static_cast<Core::Data::Node*>(target)->getOwner();
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

  if (request.target != 0 && request.target->isDerivedFrom<Ast::Function>()) {
    tracer->lookupCallee_function(request, result);
  } else if (request.target != 0 && request.target->isDerivedFrom<Type>()) {
    if (request.targetIsObject) {
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
  } else if (request.target != 0 && helper->isAstReference(request.target)) {
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

  // TODO: Should we skip if targetIsObject is false and the function is a member function?

  // Only allow this check if the funciton has the proper modifier.
  Char const *defOp = 0;
  auto def = func->findOwner<Core::Data::Ast::Definition>();
  if (def != 0) {
    defOp = findOperationModifier(def);
  }
  if (request.op == S("()") && defOp != 0 && request.op != defOp) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  } else if (request.op != S("()") && (defOp == 0 || request.op != defOp)) {
    if (request.op == S("") && !request.targetIsObject) {
      // We are just trying to get a reference to the function itself.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.stack.add(func);
      result.thisIndex = -2;
    } else {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
    return;
  }

  Bool useThis = !func->getType()->isBindDisabled() && request.thisType != 0;

  result.matchStatus = func->getType()->matchCall(useThis ? &extTypes : request.argTypes, helper, request.ec);
  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
    result.stack.add(func);
    result.thisIndex = useThis ? -1 : -2;
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
    // This is a constructor.
    Function *customCaster;
    static Core::Data::Ast::Identifier ref({ {S("value"), TiStr(S("~init"))} });
    CalleeLookupRequest innerRequest = request;
    innerRequest.ref = &ref;
    innerRequest.op = S("~init");
    innerRequest.target = type;
    innerRequest.searchTargetOwners = false;
    innerRequest.thisType = helper->getReferenceTypeFor(type, Ast::ReferenceMode::IMPLICIT);
    tracer->lookupCallee(innerRequest, result);
    if (result.isSuccessful()) {
      // A constructor match was found.
      result.pushStack(type);
      result.thisIndex = -2;
    } else if (
      request.argTypes->getElementCount() == 0 &&
      (type->getInitializationMethod(helper, request.ec) & TypeInitMethod::USER) == 0
    ) {
      // No user-defined constructor is defined and no params is provided, so we can skip the matching.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.stack.add(type);
      result.thisIndex = -2;
    } else if (
      (type->getInitializationMethod(helper, request.ec) & TypeInitMethod::USER) == 0 &&
      request.argTypes->getElementCount() == 1 &&
      helper->matchTargetType(request.argTypes->getElement(0), type, request.ec, customCaster) >= TypeMatchStatus::CUSTOM_CASTER
    ) {
      // A type with no custom init but can be initialized from the given arg.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.stack.add(type);
      result.thisIndex = -2;
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
    result.stack.add(type);
    result.thisIndex = -2;
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
    result.stack.add(request.target);
    result.thisIndex = -2;
    return;
  }

  // TODO: Look-up template functions as well.
  auto objType = tracer->helper->traceType(request.target);
  if (objType == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  } else {
    CalleeLookupRequest innerRequest = request;
    innerRequest.target = objType;
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

  if (request.op != S("") || request.targetIsObject) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  } else {
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.stack.add(request.target);
    result.thisIndex = -2;
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
  result.stack.add(request.target);
  result.thisIndex = -2;
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
  objType = helper->tryGetDeepReferenceContentType(objType);
  if (objType == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Data::Ast::findSourceLocation(request.target));
    return;
  }

  if (request.targetIsObject) {
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
  innerRequest.targetIsObject = true;
  innerRequest.target = objType;
  if (!noBindDef && objType->isDerivedFrom<UserType>()) {
    innerRequest.thisType = helper->getReferenceTypeFor(objType, ReferenceMode::IMPLICIT);
  }
  tracer->lookupCallee_routing(innerRequest, result);

  if (result.isSuccessful()) {
    result.pushStack(request.target);
    if (result.thisIndex == -1) {
      result.thisIndex = (noBindDef || !objType->isDerivedFrom<UserType>()) ? -1 : 0;
    }
  }
}


void CalleeTracer::_lookupCallee_literal(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  if (request.op != S("()")) {
    // Accept any other operator and let the caller handle the built in type checking.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.thisIndex = -2;
    result.stack.clear();
    result.pushStack(request.target);
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

  // TODO: Should we skip if targetIsObject is false and the function is a member function?

  ContainerExtender<TiObject, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  // Only allow this check if the funciton has the proper modifier.
  auto def = static_cast<Core::Data::Node*>(request.target)->findOwner<Core::Data::Ast::Definition>();
  auto defOp = findOperationModifier(def);

  if (request.op == S("()") && defOp != 0 && request.op != defOp) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  } else if (request.op != S("()") && (defOp == 0 || request.op != defOp)) {
    if (request.op == S("")) {
      // We are just trying to get a reference to the function itself.
      result.matchStatus = TypeMatchStatus::EXACT;
      result.notice.reset();
      result.stack.clear();
      result.thisIndex = -2;
    } else {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
    return;
  }

  auto funcType = helper->tryGetPointerContentType<FunctionType>(request.target);
  Bool useThis = !funcType->isBindDisabled() && request.thisType != 0;
  result.matchStatus = funcType->matchCall(useThis ? &extTypes : request.argTypes, helper, request.ec);

  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
    result.thisIndex = useThis ? -1 : -2;
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
  innerRequest.searchTargetOwners = false;
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

  ContainerExtender<TiObject, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

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
        helper->isImplicitlyCastableTo(request.argTypes->getElement(0), helper->getArchIntType(), request.ec)
      ) {
        result.matchStatus = TypeMatchStatus::EXACT;
        result.thisIndex = -2;
        result.stack.clear();
        result.notice.reset();
      } else {
        if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
      }
    } else {
      if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
  } else if (request.op == S("=")) {
    // Type must be castable to the target type for assignment operators.
    if (request.argTypes != 0 && request.argTypes->getElementCount() == 1) {
      Function *caster;
      auto ms = helper->matchTargetType(request.argTypes->getElement(0), type, request.ec, caster);
      if (ms >= TypeMatchStatus::CUSTOM_CASTER) {
        result.matchStatus = ms;
        result.thisIndex = -2;
        result.stack.clear();
        result.notice.reset();
      } else {
        if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::IncompatibleOperatorTypesNotice>();
      }
    } else {
      if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
  } else {
    // Accept any other operator and let the caller handle the built in type checking.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.thisIndex = -2;
    result.stack.clear();
    result.notice.reset();
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
      // If the results are identical it means we have reached the same target twice, which can happen if the user has
      // an alias at a parent scope with the same name pointing to the same element. In this case we ignore the second
      // match without raising an error.
      Bool identicalStack = true;
      if (newResult.stack.getLength() != result.stack.getLength()) identicalStack = false;
      else for (Int i = 0; i < newResult.stack.getLength(); ++i) {
        if (newResult.stack(i) != result.stack(i)) {
          identicalStack = false;
          break;
        }
      }
      if (!identicalStack) {
        // Multiple callee match.
        result.notice = newSrdObj<Spp::Notices::MultipleCalleeMatchNotice>();
      }
    }
  } else if (result.matchStatus == TypeMatchStatus::NONE && result.notice == 0) {
    // It was a failed match and we don't have a previous failed match, so we'll take this match.
    result = newResult;
  }
}

} // namespace
