/**
 * @file Spp/Ast/CalleeTracer.cpp
 * Contains the implementation of class Spp::Ast::CalleeTracer.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
    // The result that the search callback writes to. Searches that only accept successful matches write to a
    // temporary result, which is then committed by commitReferencedResult.
    CalleeLookupResult *currentResult = &result;
    auto callback = [=, &request, &currentResult, &target, &tracingAlias, &searchingFunctionOwners]
      (TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
      {
        auto &result = *currentResult;
        if (action == Core::Ast::Seeker::Action::ERROR) {
          if (result.isNew()) result.notice = notice;
          return Core::Ast::Seeker::Verb::MOVE;
        } else if (action == Core::Ast::Seeker::Action::OWNER_SCOPE) {
          if (tracingAlias == 0 || result.isNameMatched()) return Core::Ast::Seeker::Verb::SKIP_GROUP;
          else return Core::Ast::Seeker::Verb::MOVE;
        } else if (action == Core::Ast::Seeker::Action::USE_SCOPES_START) {
          if (tracingAlias == 0 || result.isNameMatched()) return Core::Ast::Seeker::Verb::SKIP;
          else return Core::Ast::Seeker::Verb::MOVE;
        } else if (action == Core::Ast::Seeker::Action::ALIAS_TRACE_START) {
          ++tracingAlias;
          return Core::Ast::Seeker::Verb::MOVE;
        } else if (action == Core::Ast::Seeker::Action::ALIAS_TRACE_END) {
          --tracingAlias;
          return Core::Ast::Seeker::Verb::MOVE;
        }

        if (tracer->helper->isVariable(obj)) {
          auto domain = tracer->helper->getVariableDomain(obj);
          
          // If we are going out of the current function then we should only be able to access global variables.
          if (domain == Ast::DefinitionDomain::FUNCTION && searchingFunctionOwners) {
            result.notice = newSrdObj<Spp::Notices::AccessingLocalVarInOtherFuncNotice>(
              Core::Ast::findSourceLocation(request.astNode)
            );
            return Core::Ast::Seeker::Verb::STOP;
          }

          // Local variables (including auto created values like `this`) are only accessible directly. Object and scope
          // member lookups skip them to avoid unintentionally picking up an auto created `this`.
          if (domain == DefinitionDomain::FUNCTION && request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE) {
            return Core::Ast::Seeker::Verb::MOVE;
          }
        }

        if (action != Core::Ast::Seeker::Action::TARGET_MATCH || obj == 0) return Core::Ast::Seeker::Verb::MOVE;

        // Unbox if we have a box.
        if (obj->isA<Core::Ast::Passage>()) {
          obj = static_cast<Core::Ast::Passage*>(obj)->get();
        }

        if (
          result.stack.getLength() > 0 && result.stack(result.stack.getLength() - 1).obj == obj
        ) {
          return Core::Ast::Seeker::Verb::MOVE;
        }

        if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
          if (obj == 0 || obj->findOwner<Type>() != target) {
            // The found member was probably an alias to a non member.
            result.notice = newSrdObj<Spp::Notices::InvalidTypeMemberNotice>(
              Core::Ast::findSourceLocation(request.astNode)
            );
            return Core::Ast::Seeker::Verb::MOVE;
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
          return Core::Ast::Seeker::Verb::STOP;
        }

        return Core::Ast::Seeker::Verb::MOVE;
      };

    // Any name match from within the initial function being searched is taken even if it's a failure, whereas matches
    // from referenced scopes (through injections or bridges) and from scopes outside the initial function are only
    // accepted on success. This distinction is to prevent local variables from being shadowed by a better match from
    // a referenced scope, which can be harder to spot by the programmer. Failed matches that aren't accepted are kept
    // aside and the first of them is only reported if nothing else matches the name. This allows, for example, calling
    // a type's shared functions from within its member functions despite the members injected through the auto `this`
    // of those member functions.
    // Failures that aren't due to a mismatch (accessing a local variable of an outer function, or matching multiple
    // callees) are accepted as well, since they mean the programmer most likely meant that definition, so we shouldn't
    // silently pick another one with the same name from further out.
    //
    // TODO: Is there a more generic way to decide the errors that would be deferred than hardcoding them?
    CalleeLookupResult deferredFailure;
    auto commitReferencedResult = [&result, &deferredFailure](CalleeLookupResult const &referencedResult) {
      Bool accepted = referencedResult.isSuccessful() || (
        referencedResult.notice != 0 && (
          referencedResult.notice->isDerivedFrom<Spp::Notices::AccessingLocalVarInOtherFuncNotice>() ||
          referencedResult.notice->isDerivedFrom<Spp::Notices::MultipleCalleeMatchNotice>()
        )
      );
      CalleeTracer::selectBetterResult(referencedResult, accepted ? result : deferredFailure);
    };

    while (target != 0) {
      // Search the requested scope.
      CalleeLookupResult scopeResult;
      currentResult = &scopeResult;
      tracingAlias = 0;
      tracer->getSeeker()->foreach(request.ref, target, callback, 0);

      // The scope in which to look for injections and bridges. Special handling for Type targets, but it's only needed
      // if we are at the top of the stack to avoid re-testing the same scope again.
      auto dataType = target == request.target ? ti_cast<DataType>(target) : 0;
      Core::Ast::Scope *scope = dataType != 0 ? dataType->getBody().get() : ti_cast<Core::Ast::Scope>(target);
      auto block = ti_cast<Block>(scope);

      // Function args take precedence over the injections of the function's body (e.g. the auto injected `this` of
      // member functions), so we search the args before trying those injections.
      if (scope != 0 && request.mode == CalleeLookupMode::DIRECTLY_ACCESSIBLE && !currentResult->isNameMatched()) {
        auto func = ti_cast<Ast::Function>(scope->getOwner());
        if (func != 0) {
          tracingAlias = 0;
          tracer->getSeeker()->foreach(request.ref, func, callback, 0);
        }
      }

      // If we are still within the function, and didn't move to outer scope yet then we should just
      // take the result even if it's a failure.
      if (searchingFunctionOwners) commitReferencedResult(scopeResult);
      else CalleeTracer::selectBetterResult(scopeResult, result);

      // Try the injections if we haven't found any name match in a function's scopes, or if we don't have an exact
      // match yet in a type body or an object member lookup. Type bodies need it because members found directly in the
      // body can only be accessed through the injected `this` of the auto constructor, and object member lookups need
      // it because overloads of a member can be split between the type and the types injected into it (inheritance).
      if (
        block != 0 && !request.skipInjections && (
          request.mode == CalleeLookupMode::DIRECTLY_ACCESSIBLE && ti_cast<DataType>(block->getOwner()) == 0 ?
            !result.isNameMatched() : result.matchStatus < TypeMatchStatus::EXACT
        )
      ) {
        CalleeLookupResult injectionResult;
        tracer->lookupCallee_injections(request, block, dataType, searchingFunctionOwners, injectionResult);
        commitReferencedResult(injectionResult);
      }

      // Try scope bridges if we haven't found any name match.
      if (scope != 0 && !result.isNameMatched() && request.mode == CalleeLookupMode::DIRECTLY_ACCESSIBLE) {
        CalleeLookupResult bridgeResult;
        currentResult = &bridgeResult;
        for (Int i = 0; i < scope->getBridgeCount(); ++i) {
          auto bridgeRef = scope->getBridge(i);
          // If the thing we are looking for is actually this bridgeRef, then we need to skip, otherwise we end up in
          // an infinite loop.
          if (bridgeRef->getTarget() == request.ref) continue;
          auto bridgeTarget = tracer->getSeeker()->tryGet(
            bridgeRef->getTarget().get(), scope,
            Core::Ast::Seeker::Flags::SKIP_USES | Core::Ast::Seeker::Flags::SKIP_USES_FOR_ALIASES
          );
          if (bridgeTarget == 0) continue;

          tracingAlias = 0;
          auto verb = tracer->getSeeker()->foreach(request.ref, bridgeTarget, callback, 0);
          if (!Core::Ast::Seeker::isMove(verb)) break;
        }
        commitReferencedResult(bridgeResult);
      }

      // Only directly accessible lookups go up to owner scopes. `result` only receives failures that should be
      // reported (see commitReferencedResult), so any name match in it ends the search.
      if (request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE || result.isNameMatched()) break;

      if (ti_cast<Ast::Function>(target) != 0) searchingFunctionOwners = true;

      target = target->getOwner();
      auto ownerFunc = ti_cast<Ast::Function>(target);
      if (ownerFunc != 0 && ownerFunc->getBody() == scope) {
        // The function's args were already searched along with its body, so we skip the function itself.
        searchingFunctionOwners = true;
        target = target->getOwner();
      }
      if (ti_cast<Ast::DataType>(target) != 0) {
        // Skip the type object itself.
        target = static_cast<Core::Ast::Node*>(target)->getOwner();
      }
    }

    if (!result.isNameMatched() && deferredFailure.isNameMatched()) result = deferredFailure;
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

    if (result.notice != 0) result.notice->setSourceLocation(Core::Ast::findSourceLocation(
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

  if (request.target == 0) {
    if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  // The ~pick operator only picks among named functions and variables, possibly from templates. Any other target,
  // including the result of an expression (an object member lookup on a type), has nothing to pick from.
  if (
    request.op == S("~pick") &&
    !request.target->isDerivedFrom<Ast::Function>() &&
    !request.target->isDerivedFrom<Template>() &&
    !helper->isVariable(request.target)
  ) {
    if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  if (request.target->isDerivedFrom<Ast::Function>()) {
    tracer->lookupCallee_function(request, result);
  } else if (request.target->isDerivedFrom<Type>()) {
    // In object member mode the type represents an object of that type rather than the type itself.
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
  } else if (request.target->isDerivedFrom<Template>()) {
    tracer->lookupCallee_template(request, result);
  } else if (request.target->isDerivedFrom<Core::Ast::Scope>()) {
    tracer->lookupCallee_scope(request, result);
  } else if (request.target->isDerivedFrom<ArgPack>()) {
    tracer->lookupCallee_argPack(request, result);
  } else if (helper->isVariable(request.target)) {
    tracer->lookupCallee_var(request, result);
  } else if (
    request.target->isDerivedFrom<Core::Ast::StringLiteral>() ||
    request.target->isDerivedFrom<Core::Ast::IntegerLiteral>() ||
    request.target->isDerivedFrom<Core::Ast::FloatLiteral>()
  ) {
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

  ContainerExtender<Spp::Ast::Type, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  auto func = static_cast<Ast::Function*>(request.target);

  if (request.op == S("[]")) {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    return;
  }

  if (!func->getType()->isMember() && request.thisType != 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidGlobalDefAccessNotice>(
      Core::Ast::findSourceLocation(request.target)
    );
    return;
  }

  if (request.op == S("~pick")) {
    // Picking a function is only valid on the function itself, not through an object, since member functions belong
    // to the type rather than the object (similar to ~ptr). The function's type must exactly match the given type.
    // This is checked before the function's operation modifier is considered since we want the function itself
    // regardless of what kind of handler it is.
    if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    }
    if (func->getType()->matchTargetType(request.argTypes->getElement(0), helper) != TypeMatchStatus::EXACT) {
      result.notice = newSrdObj<Spp::Notices::NoMatchingPickTargetNotice>();
      return;
    }
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    result.pushObject(func);
    return;
  }

  Bool useThis = request.thisType != 0;

  // Only allow this check if the funciton has the proper modifier.
  Char const *defOp = 0;
  auto def = func->findOwner<Core::Ast::Definition>();
  if (def != 0) {
    defOp = findOperationModifier(def);
  }
  // A no_bind member function means that the value it returns doesn't become the new `this`, instead members of that
  // value bind to the same `this` that the function itself received.
  Bool noBindDef = def != 0 && helper->isNoBindDef(def) && func->getType()->isMember();
  if (defOp != 0 && SBSTR(defOp) == S("")) {
    PlainList<Spp::Ast::Type> argTypes;
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
      if (!noBindDef) result.pushThisMarker();
      result.pushFunctionCall();
      result.pushObject(request.target);
    } else if (CalleeTracer::isAssignOp(request.op)) {
      // This is a read property, so we shouldn't allow assign operations on it.
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    } else {
      auto retType = helper->tryGetDeepReferenceContentType(func->getType()->traceRetType(helper));
      Bool bindToRet = !noBindDef && retType->isDerivedFrom<UserType>();
      CalleeLookupRequest innerRequest = request;
      innerRequest.setObjectTarget(
        retType, bindToRet ? helper->getReferenceTypeFor(retType, ReferenceMode::IMPLICIT) : request.thisType
      );
      tracer->lookupCallee_routing(innerRequest, result);

      if (result.isSuccessful()) {
        if (bindToRet) result.pushThisMarker();
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
    // A call on a type, like `MyType(...)`, can match any of the following, so we'll look for all of them and pick the
    // best match:
    // * A `()` operator on the type itself (`handler this_type(...)`), which doesn't receive `this`.
    // * A `()` operator on an object (`handler this(...)`), which receives the object as the first argument.
    // * A constructor (`~init`).
    // If the matches are at the same level then the `()` operators take precedence over the constructor.
    static Core::Ast::Identifier ref;
    CalleeLookupRequest innerRequest = request;
    innerRequest.ref = &ref;
    innerRequest.mode = CalleeLookupMode::SCOPE_MEMBER;
    ref.setValue(S("()"));
    innerRequest.op = S("()");
    innerRequest.target = type;
    innerRequest.thisType = 0;
    CalleeLookupResult opResult;
    tracer->lookupCallee(innerRequest, opResult);

    ref.setValue(S("~init"));
    innerRequest.op = S("~init");
    innerRequest.setObjectTarget(type, helper->getReferenceTypeFor(type, Ast::ReferenceMode::IMPLICIT));
    innerRequest.skipInjections = true;
    CalleeLookupResult initResult;
    tracer->lookupCallee(innerRequest, initResult);

    if (opResult.isSuccessful() && (!initResult.isSuccessful() || opResult.matchStatus >= initResult.matchStatus)) {
      result = opResult;
      return;
    } else if (initResult.isSuccessful()) {
      result = initResult;
      result.pushObject(type);
      return;
    }
    // Neither was a successful match, so keep the failure that is more informative.
    result = opResult;
    CalleeTracer::selectBetterResult(initResult, result);

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
  Core::Ast::List list;
  Core::Ast::Node *tplParam = request.templateParam != 0 ? request.templateParam : &list;
  SharedPtr<Core::Ast::Node> instance;
  SharedPtr<Core::Notices::Notice> notice;
  if (!tpl->matchInstance(tplParam, tracer->helper, instance, notice)) {
    result.notice = notice;
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

  auto def = ti_cast<Core::Ast::Definition>(static_cast<Core::Ast::Node*>(request.target)->getOwner());

  auto objType = ti_cast<Type>(helper->traceType(request.target));
  if (objType == 0) {
    result.notice = newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Ast::findSourceLocation(request.target));
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
    result.notice = newSrdObj<Spp::Notices::InvalidTypeNotice>(Core::Ast::findSourceLocation(request.target));
    return;
  }

  if (request.mode == CalleeLookupMode::OBJECT_MEMBER) {
    if (helper->getVariableDomain(request.target) != Ast::DefinitionDomain::OBJECT) {
      result.notice = newSrdObj<Spp::Notices::InvalidGlobalDefAccessNotice>(
        Core::Ast::findSourceLocation(request.target)
      );
      return;
    }
  } else {
    if (
      !request.treatObjectMembersAsGlobals && helper->getVariableDomain(request.target) == Ast::DefinitionDomain::OBJECT
    ) {
      result.notice = newSrdObj<Spp::Notices::InvalidObjectMemberAccessNotice>(
        Core::Ast::findSourceLocation(request.target)
      );
      return;
    }
  }

  Bool noBindDef = def != 0 && helper->isNoBindDef(def);

  if (request.op == S("~pick")) {
    // Picking a variable means we want the variable itself, the same as referencing it without the operator, so we
    // won't apply the operation on the variable's value. Since no casting is applied, the variable's type must exactly
    // match the given type.
    Function *caster;
    if (helper->matchTargetType(objType, request.argTypes->getElement(0), caster) != TypeMatchStatus::EXACT) {
      result.notice = newSrdObj<Spp::Notices::NoMatchingPickTargetNotice>();
      return;
    }
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    if (!noBindDef) result.pushThisMarker();
    result.pushObject(request.target);
    return;
  }

  // If the variable doesn't bind, members of its value bind to the current `this`.
  Bool bindToVar = !noBindDef && objType->isDerivedFrom<UserType>() && targetIsRef;
  CalleeLookupRequest innerRequest = request;
  innerRequest.setObjectTarget(
    objType, bindToVar ? helper->getReferenceTypeFor(objType, ReferenceMode::IMPLICIT) : request.thisType
  );
  innerRequest.varTargetDef = def;
  tracer->lookupCallee_routing(innerRequest, result);

  if (result.isSuccessful()) {
    // The variable becomes the new `this` if something that binds to it follows, or if the variable itself is the
    // final value (in which case it binds to itself), unless it's a no_bind variable.
    Bool isFinalValue = request.op == S("") && result.stack.getLength() == 0;
    if (!noBindDef && (objType->isDerivedFrom<UserType>() || isFinalValue)) result.pushThisMarker();
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

  ContainerExtender<Spp::Ast::Type, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  auto funcType = helper->tryGetPointerContentType<FunctionType>(request.target);
  Bool useThis = funcType->isMember() && request.thisType != 0;

  // The operation modifier and no_bind modifier of the variable holding this pointer, if any.
  Char const *varTargetOp = request.varTargetDef == 0 ? 0 : findOperationModifier(request.varTargetDef);
  Bool varTargetNoBind = request.varTargetDef != 0 && helper->isNoBindDef(request.varTargetDef);

  if (varTargetOp != 0 && SBSTR(varTargetOp) == S("")) {
    PlainList<Spp::Ast::Type> argTypes;
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
      if (!varTargetNoBind) result.pushThisMarker();
      result.pushFunctionCall();
    } else if (CalleeTracer::isAssignOp(request.op)) {
      // This is a read property, so we shouldn't allow assign operations on it.
      result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      return;
    } else {
      auto retType = helper->tryGetDeepReferenceContentType(funcType->traceRetType(helper));
      Bool bindToRet = !varTargetNoBind && retType->isDerivedFrom<UserType>();
      CalleeLookupRequest innerRequest = request;
      innerRequest.setObjectTarget(
        retType, bindToRet ? helper->getReferenceTypeFor(retType, ReferenceMode::IMPLICIT) : request.thisType
      );
      tracer->lookupCallee_routing(innerRequest, result);

      if (result.isSuccessful()) {
        if (bindToRet) result.pushThisMarker();
        result.pushFunctionCall();
      }
    }
    return;
  } else if (request.op == S("") && varTargetOp == 0) {
    // We are just trying to get a reference to the function ptr itself.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.notice.reset();
    result.stack.clear();
    return;
  } else if (request.op != (varTargetOp == 0 ? S("()") : varTargetOp)) {
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

  Core::Ast::Identifier ref({ {S("value"), TiStr(request.op)} });
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
  } else if (request.varTargetDef == 0 || findOperationModifier(request.varTargetDef) == 0) {
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

void CalleeTracer::lookupCallee_injections(
  CalleeLookupRequest &request, Block *block, DataType *dataType, Bool searchingFunctionOwners,
  CalleeLookupResult &result
) {
  auto helper = this->helper;
  for (Int i = 0; i < block->getInjectionCount(); ++i) {
    auto def = ti_cast<Core::Ast::Definition>(block->getInjection(i));
    if (def == 0 || def->getSubject() == 0) continue;
    auto subject = def->getSubject().get();
    Bool isVar = helper->isVariable(subject);
    auto domain = isVar ? helper->getVariableDomain(def) : DefinitionDomain::GLOBAL;
    if (domain == DefinitionDomain::FUNCTION) {
      // Local variables are only accessible directly (see the similar check in _lookupCallee).
      if (request.mode != CalleeLookupMode::DIRECTLY_ACCESSIBLE) continue;
      // If we are going out of the current function then we should ignore local variables of outer functions.
      if (searchingFunctionOwners) continue;
    }
    // Injected read properties are either functions or function pointers (as_ptr).
    Bool isReadProperty = isReadPropertyDef(def);
    Type *objType = 0;
    if (isReadProperty) {
      FunctionType *funcType = 0;
      if (subject->isDerivedFrom<Function>()) {
        funcType = static_cast<Function*>(subject)->getType().get();
      } else if (isVar) {
        auto varType = helper->traceType(subject);
        if (varType == 0) continue;
        funcType = helper->tryGetPointerContentType<FunctionType>(
          helper->tryGetDeepReferenceContentType(varType)
        );
        // A pointer held in an object can only point to a member function and vice versa.
        if (funcType != 0 && funcType->isMember() != (domain == DefinitionDomain::OBJECT)) continue;
      }
      if (funcType == 0) continue;
      // Similar to variables, member read properties can only be used if we have a `this`, while non-member
      // ones can only be used if we don't.
      PlainList<Type> argTypes;
      if (funcType->isMember()) {
        if (request.thisType == 0) continue;
        argTypes.add(request.thisType);
      } else if (request.thisType != 0) {
        continue;
      }
      if (funcType->matchCall(&argTypes, helper) < TypeMatchStatus::CUSTOM_CASTER) continue;
      objType = funcType->traceRetType(helper);
      // Members can only be looked up on user types.
      if (objType == 0 || !helper->tryGetDeepReferenceContentType(objType)->isDerivedFrom<UserType>()) continue;
    } else {
      if (!isVar) continue;
      // Object member variables can only be accessed if we have a `this` to access them through, while non-member
      // variables can only be used if we don't. This depends on the availability of `this` rather than the lookup
      // mode since an object member lookup can have no `this` (e.g. on a no_bind value).
      Bool isMember = domain == DefinitionDomain::OBJECT;
      if (isMember != (request.thisType != 0)) continue;
      objType = helper->traceType(subject);
      if (objType == 0) continue;
    }
    if (helper->tryGetDeepReferenceContentType(objType) == request.injectionChainStartTarget) {
      continue; // Prevent infinite loops in case of circular injections.
    }
    auto refType = helper->getReferenceTypeFor(objType, Ast::ReferenceMode::IMPLICIT);
    objType = helper->tryGetDeepReferenceContentType(objType);
    Bool noBindDef = helper->isNoBindDef(def);
    CalleeLookupRequest innerRequest = request;
    innerRequest.setObjectTarget(objType, noBindDef ? request.thisType : refType);
    if (innerRequest.injectionChainStartTarget == 0) {
      // Capture the first injection's content type to prevent infinite loops in case of circular injections.
      innerRequest.injectionChainStartTarget = dataType;
    }
    CalleeLookupResult innerResult;
    this->lookupCallee(innerRequest, innerResult);
    if (innerResult.isSuccessful()) {
      if (!noBindDef) innerResult.pushThisMarker();
      // Read properties are called (with the current `this` if they are members) to get the value to look up
      // the members on.
      if (isReadProperty) innerResult.pushFunctionCall();
      innerResult.pushObject(subject);
      ++innerResult.injectionLevel;
    }
    CalleeTracer::selectBetterResult(innerResult, result);
    // No need to keep looking if we already found an exact match.
    if (result.matchStatus == TypeMatchStatus::EXACT) break;
  }
}


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
            Core::Ast::findSourceLocation(result.stack(diffPoint).obj),
            Core::Ast::findSourceLocation(newResult.stack(diffPoint).obj)
          );
        } else if (newResult.stack.getLength() != result.stack.getLength()) {
          // Multiple callee match.
          result.notice = newSrdObj<Spp::Notices::MultipleCalleeMatchNotice>(
            Core::Ast::findSourceLocation(result.stack(result.stack.getLength() - 1).obj),
            Core::Ast::findSourceLocation(newResult.stack(newResult.stack.getLength() - 1).obj)
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
