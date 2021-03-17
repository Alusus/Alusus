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
    &this->lookupCallee_var,
    &this->lookupCallee_funcPtr,
    &this->lookupCallee_customOp,
    &this->lookupCallee_builtInOp,
    &this->lookupReferenceTarget
  });
}


void CalleeTracer::initBindings()
{
  this->lookupCallee = &CalleeTracer::_lookupCallee;
  this->lookupCallee_routing = &CalleeTracer::_lookupCallee_routing;
  this->lookupCallee_function = &CalleeTracer::_lookupCallee_function;
  this->lookupCallee_type = &CalleeTracer::_lookupCallee_type;
  this->lookupCallee_template = &CalleeTracer::_lookupCallee_template;
  this->lookupCallee_var = &CalleeTracer::_lookupCallee_var;
  this->lookupCallee_funcPtr = &CalleeTracer::_lookupCallee_funcPtr;
  this->lookupCallee_customOp = &CalleeTracer::_lookupCallee_customOp;
  this->lookupCallee_builtInOp = &CalleeTracer::_lookupCallee_builtInOp;
  this->lookupReferenceTarget = &CalleeTracer::_lookupReferenceTarget;
}


//==============================================================================
// Main Functions

void CalleeTracer::_lookupCallee(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

  if (request.ref != 0) {
    Core::Data::Node *prevNode = 0;
    tracer->getSeeker()->foreach(request.ref, request.target,
      [=, &request, &result, &prevNode]
        (TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
        {
          if (ntc != 0) {
            if (result.isNew()) result.notice = getSharedPtr(ntc);
            return Core::Data::Seeker::Verb::MOVE;
          }
          if (obj == 0) return Core::Data::Seeker::Verb::MOVE;

          // Unbox if we have a box.
          auto box = ti_cast<TioWeakBox>(obj);
          if (box != 0) obj = box->get().get();

          if (
            result.stack.getLength() > 0 && result.stack(result.stack.getLength() - 1) == obj &&
            result.thisIndex < 0
          ) {
            return Core::Data::Seeker::Verb::MOVE;
          }

          // There is no need to continue searching if we already have a match and we are jumping to another level.
          auto node = ti_cast<Core::Data::Node>(obj);
          if (node != 0 && prevNode != 0) {
            // If we encounter the same node again it means we moved up and encountered a use statement that took us back
            // into the same module. In this case we can stop looking since we have moved up and we already have a match.
            if (node == prevNode) return Core::Data::Seeker::Verb::STOP;

            // If it's not the same node, then let's check the owners of the two nodes.
            Core::Data::Node *owner = Core::Data::findOwner<Core::Data::Ast::Scope>(node);
            // It's possible that the previous match was for a function arg, so we have to account for that since a
            // function arg would have the same owner scope as a function sitting at the same module.
            Core::Data::Node *prevOwner = Core::Data::findOwner<Spp::Ast::FunctionType>(prevNode);
            if (prevOwner == 0) prevOwner = Core::Data::findOwner<Core::Data::Ast::Scope>(prevNode);
            if (owner != prevOwner) return Core::Data::Seeker::Verb::STOP;
          }

          CalleeLookupRequest innerRequest = request;
          CalleeLookupResult innerResult;
          innerRequest.target = obj;
          tracer->lookupCallee_routing(innerRequest, innerResult);
          if (innerResult.isSuccessful()) prevNode = node;
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
        },
        request.searchTargetOwners ? 0 : SeekerExtension::Flags::SKIP_OWNERS_AND_USES
    );

    // Try the injections if we don't have an exact match yet.
    if (result.matchStatus < TypeMatchStatus::EXACT) {
      auto helper = tracer->helper;
      auto dataType = ti_cast<DataType>(request.target);
      if (dataType != 0) {
        auto block = dataType->getBody().get();
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
      } else {
        if (ti_cast<UserType>(request.target) != 0) {
          tracer->lookupCallee_customOp(request, result);
          if (result.isSuccessful()) return;
        }
        tracer->lookupCallee_builtInOp(request, result);
      }
    } else {
      tracer->lookupCallee_type(request, result);
    }
  } else if (request.target != 0 && request.target->isDerivedFrom<Template>()) {
    tracer->lookupCallee_template(request, result);
  } else if (request.target != 0 && helper->isAstReference(request.target)) {
    tracer->lookupCallee_var(request, result);
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

  if (request.op != "()") {
    // Only allow this check if the funciton has the proper modifier.
    // TODO: Check function modifiers rather than generic def modifiers.
    auto def = func->findOwner<Core::Data::Ast::Definition>();
    if (def == 0) return;
    if (!helper->doesModifierExistOnDef(def, S("assign"))) return;
  }

  Bool useThis = !func->getType()->isBindDisabled() && request.thisType != 0;

  result.matchStatus = func->getType()->matchCall(useThis ? &extTypes : request.argTypes, helper, request.ec);
  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
    result.stack.add(func);
    result.thisIndex = useThis ? -1 : -2;
    result.type = func->getType().get();
  } else {
    result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
  }
}


void CalleeTracer::_lookupCallee_type(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  auto type = static_cast<Type*>(request.target);

  if (request.op == S("()")) {
    // This is a constructor.
    Function *customCaster;
    static Core::Data::Ast::Identifier ref({ {S("value"), TiStr(S("~init"))} });
    CalleeLookupRequest innerRequest = request;
    innerRequest.ref = &ref;
    innerRequest.target = type;
    innerRequest.searchTargetOwners = false;
    innerRequest.thisType = helper->getReferenceTypeFor(type, Ast::ReferenceMode::IMPLICIT);
    tracer->lookupCallee(innerRequest, result);
    if (result.isSuccessful()) {
      // A constructor match was found.
      result.pushStack(type);
      result.thisIndex = -2;
      result.type = type;
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
      result.type = type;
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
      result.type = type;
    } else {
      // If the ref symbol we provided here was not found we'll provide a better error message to the user since
      // the user didn't manually provide this symbol.
      if (result.notice != 0 && result.notice->isA<Spp::Notices::UnknownSymbolNotice>()) {
        result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
      }
    }
  } else {
    result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
  }
}


void CalleeTracer::_lookupCallee_template(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

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


void CalleeTracer::_lookupCallee_var(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  auto def = Core::Data::findOwner<Core::Data::Ast::Definition>(ti_cast<Core::Data::Node>(request.target));
  if (def == 0) {
    throw EXCEPTION(GenericException, S("Unexpected error while looking for var definition."));
  }
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

  Bool noBindDef = helper->isNoBindDef(def);

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


void CalleeTracer::_lookupCallee_funcPtr(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);
  auto helper = tracer->helper;

  ContainerExtender<TiObject, 1, 0> extTypes(request.argTypes);
  extTypes.setPreItem(0, request.thisType);

  auto funcType = helper->tryGetPointerContentType<FunctionType>(request.target);

  // TODO: Verify op against modifier.
  Bool useThis = !funcType->isBindDisabled() && request.thisType != 0;

  result.matchStatus = funcType->matchCall(useThis ? &extTypes : request.argTypes, helper, request.ec);
  if (result.matchStatus >= TypeMatchStatus::CUSTOM_CASTER) {
    result.notice.reset();
    result.stack.clear();
    result.thisIndex = useThis ? -1 : -2;
    result.type = funcType;
  } else {
    result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
  }
}


void CalleeTracer::_lookupCallee_customOp(TiObject *self, CalleeLookupRequest &request, CalleeLookupResult &result)
{
  PREPARE_SELF(tracer, CalleeTracer);

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

  if (request.op == S("()")) {
    // Type must be an array for () operator.
    if (type->isDerivedFrom<ArrayType>()) {
      // We have an array.
      if (
        request.argTypes != 0 && request.argTypes->getElementCount() == 1 &&
        helper->isImplicitlyCastableTo(request.argTypes->getElement(0), helper->getArchIntType(), request.ec)
      ) {
        result.matchStatus = TypeMatchStatus::EXACT;
        result.type = type;
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
        result.type = type;
        result.thisIndex = -2;
        result.stack.clear();
        result.notice.reset();
      } else {
        if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::ArgsMismatchNotice>();
      }
    } else {
      if (result.notice == 0) result.notice = newSrdObj<Spp::Notices::InvalidOperationNotice>();
    }
  } else {
    // Accept any other operator and let the caller handle the built in type checking.
    result.matchStatus = TypeMatchStatus::EXACT;
    result.type = type;
    result.thisIndex = -2;
    result.stack.clear();
    result.notice.reset();
  }
}


// TODO:
Bool CalleeTracer::_lookupReferenceTarget(
  TiObject *self, TiObject *astNode, Core::Data::Ast::Identifier *ref, Bool searchOwners, PlainList<TiObject> &stack
) {
  PREPARE_SELF(tracer, CalleeTracer);

  Word currentStackSize = stack.getCount();
  Bool symbolFound = false;
  tracer->getSeeker()->foreach(ref, astNode,
    [=, &symbolFound, &stack] (TiObject *obj, Core::Notices::Notice*)->Core::Data::Seeker::Verb
    {
      symbolFound = true;
      stack.add(obj);
      return Core::Data::Seeker::Verb::STOP;
    },
    searchOwners ? 0 : SeekerExtension::Flags::SKIP_OWNERS_AND_USES
  );

  if (!symbolFound) {
    Block *block = 0;
    auto dataType = ti_cast<DataType>(astNode);
    if (dataType != 0) block = dataType->getBody().get();
    else block = Core::Data::findOwner<Block>(ti_cast<Core::Data::Node>(astNode));

    if (block != 0) {
      for (Int i = 0; i < block->getInjectionCount(); ++i) {
        auto def = ti_cast<Core::Data::Ast::Definition>(block->getInjection(i));
        if (def == 0 || def->getTarget() == 0) continue;
        if (!tracer->helper->isAstReference(def->getTarget().get())) continue;
        auto objType = ti_cast<Type>(tracer->getSeeker()->tryGet(def->getTarget().get(), block));
        if (objType == 0) continue;
        objType = tracer->helper->tryGetDeepReferenceContentType(objType);
        stack.add(def->getTarget().get());
        if (tracer->lookupReferenceTarget(objType, ref, false, stack)) {
          symbolFound = true;
          break;
        } else {
          while (stack.getCount() > currentStackSize) stack.remove(currentStackSize);
        }
      }
    }
  }

  return symbolFound;
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
      // Multiple callee match.
      result.notice = newSrdObj<Spp::Notices::MultipleCalleeMatchNotice>();
    }
  } else if (result.matchStatus == TypeMatchStatus::NONE && result.notice == 0) {
    // It was a failed match and we don't have a previous failed match, so we'll take this match.
    result = newResult;
  }
}

} // namespace
