/**
 * @file Spp/SeekerExtension.cpp
 * Contains the implementation of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

using namespace std::placeholders;

//==============================================================================
// Initialization Functions

SeekerExtension::Overrides* SeekerExtension::extend(Core::Ast::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper)
{
  auto extension = newSrdObj<SeekerExtension>(seeker);
  seeker->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->astHelper = astHelper;
  overrides->foreachRef = seeker->foreach.set(&SeekerExtension::_foreach).get();
  overrides->extForeachRef = seeker->extForeach.set(&SeekerExtension::_extForeach).get();
  overrides->foreach_identifierLevelRef =
    seeker->foreach_identifierLevel.set(&SeekerExtension::_foreach_identifierLevel).get();
  overrides->foreach_identifierOnFuncRef =
    extension->foreach_identifierOnFunc.set(&SeekerExtension::_foreach_identifierOnFunc).get();
  overrides->foreach_identifierOnDataTypeRef =
    extension->foreach_identifierOnDataType.set(&SeekerExtension::_foreach_identifierOnDataType).get();
  overrides->foreach_linkOperatorRoutingRef =
    seeker->foreach_linkOperatorRouting.set(&SeekerExtension::_foreach_linkOperatorRouting).get();
  overrides->foreach_paramPassRef =
    extension->foreach_paramPass.set(&SeekerExtension::_foreach_paramPass).get();
  overrides->foreach_paramPassRoutingRef =
    extension->foreach_paramPassRouting.set(&SeekerExtension::_foreach_paramPassRouting).get();
  overrides->foreach_paramPassOnTemplateRef =
    extension->foreach_paramPassOnTemplate.set(&SeekerExtension::_foreach_paramPassOnTemplate).get();
  overrides->foreach_thisTypeRefRef =
    extension->foreach_thisTypeRef.set(&SeekerExtension::_foreach_thisTypeRef).get();
  overrides->foreach_comparisonRef =
    extension->foreach_comparison.set(&SeekerExtension::_foreach_comparison).get();
  overrides->foreach_comparisonLevelRef =
    extension->foreach_comparisonLevel.set(&SeekerExtension::_foreach_comparisonLevel).get();
  overrides->foreach_comparisonOnScopeRef =
    extension->foreach_comparisonOnScope.set(&SeekerExtension::_foreach_comparisonOnScope).get();
  overrides->foreach_computeComparisonRef =
    extension->foreach_computeComparison.set(&SeekerExtension::_foreach_computeComparison).get();

  return overrides;
}


void SeekerExtension::unextend(Core::Ast::Seeker *seeker, Overrides *overrides)
{
  auto extension = ti_cast<SeekerExtension>(seeker);
  seeker->foreach.reset(overrides->foreachRef);
  seeker->extForeach.reset(overrides->extForeachRef);
  seeker->foreach_identifierLevel.reset(overrides->foreach_identifierLevelRef);
  seeker->foreach_linkOperatorRouting.reset(overrides->foreach_linkOperatorRoutingRef);
  extension->astHelper = SharedPtr<Ast::Helper>::null;
  extension->foreach_identifierOnFunc.reset(overrides->foreach_identifierOnFuncRef);
  extension->foreach_identifierOnDataType.reset(overrides->foreach_identifierOnDataTypeRef);
  extension->foreach_paramPass.reset(overrides->foreach_paramPassRef);
  extension->foreach_paramPassRouting.reset(overrides->foreach_paramPassRoutingRef);
  extension->foreach_paramPassOnTemplate.reset(overrides->foreach_paramPassOnTemplateRef);
  extension->foreach_thisTypeRef.reset(overrides->foreach_thisTypeRefRef);
  extension->foreach_comparison.reset(overrides->foreach_comparisonRef);
  extension->foreach_comparisonLevel.reset(overrides->foreach_comparisonLevelRef);
  extension->foreach_comparisonOnScope.reset(overrides->foreach_comparisonOnScopeRef);
  extension->foreach_computeComparison.reset(overrides->foreach_computeComparisonRef);

  seeker->removeDynamicInterface<SeekerExtension>();
  delete overrides;
}


//==============================================================================
// Seek Functions

Core::Ast::Seeker::Verb SeekerExtension::_foreach(
  TiFunctionBase *base, TiObject *self, Core::Ast::Node const *ref, Core::Ast::Node *target,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  if (ref->isA<Core::Ast::ParamPass>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_paramPass(static_cast<Core::Ast::ParamPass const*>(ref), target, cb, flags);
  } else if (ref->isA<Spp::Ast::ThisTypeRef>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_thisTypeRef(target, cb, flags);
  } else if (ref->isA<Core::Ast::ComparisonOperator>() || ref->isA<Core::Ast::LogOperator>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_comparison(ref, target, cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Ast::Seeker);
    return seeker->foreach.useCallee(base)(ref, target, cb, flags);
  }
}


Core::Ast::Seeker::Verb SeekerExtension::_extForeach(
  TiFunctionBase *base, TiObject *self, Core::Ast::Node const *ref, Core::Ast::Node *target,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Ast::Seeker);
  Int tracingAlias = 0;
  Int tracingUse = 0;
  return seeker->foreach(ref, target,
    [flags, cb, &tracingAlias, &tracingUse](TiInt action, Core::Ast::Node *o, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb {
      if (action == Core::Ast::Seeker::Action::ALIAS_TRACE_START) {
        ++tracingAlias;
        return Core::Ast::Seeker::Verb::MOVE;
      } else if (action == Core::Ast::Seeker::Action::ALIAS_TRACE_END) {
        --tracingAlias;
        return Core::Ast::Seeker::Verb::MOVE;
      } else if (action == Core::Ast::Seeker::Action::OWNER_SCOPE) {
        if (tracingAlias == 0 && (flags & Core::Ast::Seeker::Flags::SKIP_OWNERS) != 0) {
          return Core::Ast::Seeker::Verb::SKIP_GROUP;
        } else return Core::Ast::Seeker::Verb::MOVE;
      } else if (action == Core::Ast::Seeker::Action::USE_SCOPES_START) {
        if (
          (tracingUse > 0 || (flags & Core::Ast::Seeker::Flags::SKIP_USES) != 0) &&
          (tracingAlias == 0 || (flags & Core::Ast::Seeker::Flags::SKIP_USES_FOR_ALIASES) != 0)
        ) {
          return Core::Ast::Seeker::Verb::SKIP;
        } else {
          ++tracingUse;
          return Core::Ast::Seeker::Verb::MOVE;
        }
      } else if (action == Core::Ast::Seeker::Action::USE_SCOPES_END) {
        --tracingUse;
        return Core::Ast::Seeker::Verb::MOVE;
      } else if (action == SeekerExtension::Action::CHILD_SCOPE) {
        if ((flags & SeekerExtension::Flags::SKIP_CHILDREN) != 0) return Core::Ast::Seeker::Verb::SKIP;
      } else if (
        action != Core::Ast::Seeker::Action::TARGET_MATCH &&
        action != Core::Ast::Seeker::Action::ERROR
      ) {
        return Core::Ast::Seeker::Verb::MOVE;
      }
      return cb(action, o, notice);
    },
    flags
  );
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_identifierLevel(
  TiFunctionBase *base, TiObject *self, Core::Ast::Identifier const *identifier, Core::Ast::Node *data,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  if (data->isDerivedFrom<Ast::Function>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_identifierOnFunc(identifier, static_cast<Ast::Function*>(data), cb, flags);
  } else if (data->isDerivedFrom<Ast::DataType>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_identifierOnDataType(identifier, static_cast<Ast::DataType*>(data), cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Ast::Seeker);
    return seeker->foreach_identifierLevel.useCallee(base)(identifier, data, cb, flags);
  }
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_identifierOnFunc(
  TiObject *self, Core::Ast::Identifier const *identifier, Ast::Function *function,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  auto argTypes = function->getType()->getArgTypes().get();
  if (argTypes == 0) return Core::Ast::Seeker::Verb::MOVE;
  auto index = argTypes->findIndex(identifier->getValue().get());
  if (index >= 0) {
    return cb(Core::Ast::Seeker::Action::TARGET_MATCH, argTypes->getElement(index), Core::Ast::Seeker::NoticePtr::null);
  }
  return Core::Ast::Seeker::Verb::MOVE;
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_identifierOnDataType(
  TiObject *self, Core::Ast::Identifier const *identifier, Ast::DataType *type,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Ast::Seeker);
  auto block = type->getBody().get();
  if ((block == 0) || (flags & Core::Ast::Seeker::Flags::SKIP_OWNED)) {
    return Core::Ast::Seeker::Verb::MOVE;
  } else {
    return seeker->foreach_identifierLevel(identifier, block, cb, flags);
  }
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_linkOperatorRouting(
  TiFunctionBase *base, TiObject *self, Core::Ast::LinkOperator const *link, Core::Ast::Node *data,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Ast::Seeker);
  if (link->getType() == S(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Core::Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::DataType>()) {
        auto block = static_cast<Ast::DataType*>(data)->getBody().get();
        if ((block == 0) || (flags & Core::Ast::Seeker::Flags::SKIP_OWNED)) {
          return Core::Ast::Seeker::Verb::MOVE;
        }
        data = block;
      }
    }
  }
  return seeker->foreach_linkOperatorRouting.useCallee(base)(link, data, cb, flags);
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_paramPass(
  TiObject *self, Core::Ast::ParamPass const *paramPass, Core::Ast::Node *data,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Ast::Seeker);
  PREPARE_SELF(seekerExtension, SeekerExtension);
  auto operand = paramPass->getOperand().get();
  return seeker->foreach(operand, data,
    [=](TiInt action, Core::Ast::Node *newData, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
    {
      if (action == Core::Ast::Seeker::Action::TARGET_MATCH) {
        return seekerExtension->foreach_paramPassRouting(paramPass, newData, cb, flags);
      } else return cb(action, newData, notice);
    },
    flags
  );
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_paramPassRouting(
  TiObject *self, Core::Ast::ParamPass const *paramPass, Core::Ast::Node *data,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (paramPass->getType() == Core::Ast::BracketType::SQUARE) {
    auto param = paramPass->getParam().get();
    if (data->isDerivedFrom<Ast::Template>()) {
      return seekerExtension->foreach_paramPassOnTemplate(param, static_cast<Ast::Template*>(data), cb, flags);
    } else {
      auto notice = newSrdObj<Spp::Notices::InvalidSquareBracketOperandNotice>(
        Core::Ast::findSourceLocation(paramPass)
      );
      return cb(Core::Ast::Seeker::Action::ERROR, 0, notice);
    }
  } else {
    return Core::Ast::Seeker::Verb::MOVE;
  }
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_paramPassOnTemplate(
  TiObject *self, Core::Ast::Node *param, Ast::Template *tmplt, Core::Ast::Seeker::ForeachCallback const &cb,
  Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  SharedPtr<Core::Ast::Node> result;
  SharedPtr<Core::Notices::Notice> notice;
  if (tmplt->matchInstance(param, seekerExtension->astHelper, result, notice)) {
    return cb(Core::Ast::Seeker::Action::TARGET_MATCH, result.get(), Core::Ast::Seeker::NoticePtr::null);
  } else {
    if (notice != 0) return cb(Core::Ast::Seeker::Action::ERROR, 0, notice);
    else return Core::Ast::Seeker::Verb::MOVE;
  }
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_thisTypeRef(
  TiObject *self, Core::Ast::Node *data, Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  while (data != 0) {
    if (data->isDerivedFrom<Spp::Ast::DataType>()) {
      return cb(Core::Ast::Seeker::Action::TARGET_MATCH, data, Core::Ast::Seeker::NoticePtr::null);
    }
    data = data->getOwner();
  }
  return Core::Ast::Seeker::Verb::MOVE;
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_comparison(
  TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *data, Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  Core::Ast::Seeker::Verb retVal = Core::Ast::Seeker::Verb::MOVE;
  if (data->isDerivedFrom<Core::Ast::NodeStack>()) {
    auto stack = static_cast<Core::Ast::NodeStack*>(data);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      auto data = stack->getElement(i);
      if (data == 0) continue;
      if (i != stack->getCount() - 1) {
        retVal = cb(Core::Ast::Seeker::Action::OWNER_SCOPE, data, Core::Ast::Seeker::NoticePtr::null);
        if (retVal == Core::Ast::Seeker::Verb::SKIP) continue;
        else if (retVal == Core::Ast::Seeker::Verb::SKIP_GROUP || !Core::Ast::Seeker::isMove(retVal)) break;
      }
      retVal = seekerExtension->foreach_comparisonLevel(comparison, data, cb, flags);
      if (!Core::Ast::Seeker::isMove(retVal)) return retVal;
    }
  } else {
    auto node = data;
    while (node != 0) {
      if (node != data) {
        retVal = cb(Core::Ast::Seeker::Action::OWNER_SCOPE, node, Core::Ast::Seeker::NoticePtr::null);
        if (retVal == Core::Ast::Seeker::Verb::SKIP) {
          node = node->getOwner();
          continue;
        } else if (retVal == Core::Ast::Seeker::Verb::SKIP_GROUP || !Core::Ast::Seeker::isMove(retVal)) break;
      }
      retVal = seekerExtension->foreach_comparisonLevel(comparison, node, cb, flags);
      if (!Core::Ast::Seeker::isMove(retVal)) return retVal;
      node = node->getOwner();
      flags |= Core::Ast::Seeker::Flags::SKIP_OWNED;
    }
  }
  return retVal;
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_comparisonLevel(
  TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *data, Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (data->isDerivedFrom<Core::Ast::Scope>()) {
    return seekerExtension->foreach_comparisonOnScope(
      comparison, static_cast<Core::Ast::Scope*>(data), cb, flags
    );
  } else if (data->isDerivedFrom<Ast::DataType>()) {
    auto dataType = static_cast<Ast::DataType*>(data);
    if (dataType->getBody() != 0) {
      return seekerExtension->foreach_comparisonOnScope(comparison, dataType->getBody().get(), cb, flags);
    }
  }
  return Core::Ast::Seeker::Verb::MOVE;
}


Core::Ast::Seeker::Verb SeekerExtension::_foreach_comparisonOnScope(
  TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Scope *scope,
  Core::Ast::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  Core::Ast::Seeker::Verb verb = Core::Ast::Seeker::Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Core::Ast::Definition>(scope->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (seekerExtension->foreach_computeComparison(comparison, obj)) {
        verb = cb(Core::Ast::Seeker::Action::TARGET_MATCH, obj, Core::Ast::Seeker::NoticePtr::null);
        if (!Core::Ast::Seeker::isMove(verb)) return verb;
      }
      verb = cb(SeekerExtension::Action::CHILD_SCOPE, obj, Core::Ast::Seeker::NoticePtr::null);
      if (verb != Core::Ast::Seeker::Verb::SKIP) {
        verb = seekerExtension->foreach_comparisonLevel(comparison, obj, cb, flags);
      }
      if (!Core::Ast::Seeker::isMove(verb)) return verb;
    }
  }

  if (scope->getBridgeCount() > 0) {
    verb = cb(Core::Ast::Seeker::Action::USE_SCOPES_START, scope, Core::Ast::Seeker::NoticePtr::null);
    if (verb == Core::Ast::Seeker::Verb::SKIP) return verb;
    PREPARE_SELF(seeker, Core::Ast::Seeker);
    for (Int i = 0; i < scope->getBridgeCount(); ++i) {
      auto bridgeRef = scope->getBridge(i);
      auto bridgeTarget = seeker->tryGet(
        bridgeRef->getTarget().get(), scope,
        Core::Ast::Seeker::Flags::SKIP_USES | Core::Ast::Seeker::Flags::SKIP_USES_FOR_ALIASES
      );
      if (bridgeTarget == 0) continue;
      verb = cb(Core::Ast::Seeker::Action::USE_SCOPE, bridgeTarget, Core::Ast::Seeker::NoticePtr::null);
      if (verb == Core::Ast::Seeker::Verb::SKIP) continue;
      else if (verb == Core::Ast::Seeker::Verb::SKIP_GROUP || !Core::Ast::Seeker::isMove(verb)) break;

      verb = seekerExtension->foreach_comparisonLevel(comparison, bridgeTarget, cb, flags);
      if (!Core::Ast::Seeker::isMove(verb)) return verb;
    }
    verb = cb(Core::Ast::Seeker::Action::USE_SCOPES_END, scope, Core::Ast::Seeker::NoticePtr::null);
  }

  return verb;
}


Bool SeekerExtension::_foreach_computeComparison(TiObject *self, Core::Ast::Node const *comparison, Core::Ast::Node *target)
{
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (comparison->isDerivedFrom<Core::Ast::LogOperator>()) {
    auto logOp = static_cast<Core::Ast::LogOperator const*>(comparison);
    if (logOp->getType() == S("&&") || logOp->getType() == S("and")) {
      if (!seekerExtension->foreach_computeComparison(logOp->getFirst().get(), target)) return false;
      return seekerExtension->foreach_computeComparison(logOp->getSecond().get(), target);
    } else if (logOp->getType() == S("||") || logOp->getType() == S("or")) {
      if (seekerExtension->foreach_computeComparison(logOp->getFirst().get(), target)) return true;
      return seekerExtension->foreach_computeComparison(logOp->getSecond().get(), target);
    }
  } else if (comparison->isDerivedFrom<Core::Ast::ComparisonOperator>()) {
    auto compareOp = static_cast<Core::Ast::ComparisonOperator const*>(comparison);
    if (compareOp->getType() != S("==")) return false;
    auto prop = compareOp->getFirst().ti_cast_get<Core::Ast::Identifier const>();
    if (prop == 0) return false;
    if (prop->getValue() == S("elementType") || prop->getValue() == S("نوع_العنصر")) {
      // Check the type.
      auto type = compareOp->getSecond().ti_cast_get<Core::Ast::Text const>();
      if (type->getValue() == S("type") || type->getValue() == S("صنف")) {
        return target->isDerivedFrom<Ast::DataType>() || (
          target->isDerivedFrom<Ast::Template>() &&
          static_cast<Ast::Template*>(target)->getBody()->isDerivedFrom<Ast::DataType>()
        );
      } else if (type->getValue() == S("function") || type->getValue() == S("func") || type->getValue() == S("دالة")) {
        return target->isDerivedFrom<Ast::Function>();
      } else if (type->getValue() == S("module") || type->getValue() == S("وحدة")) {
        return target->isDerivedFrom<Ast::Module>();
      } else if (type->getValue() == S("var") || type->getValue() == S("متغير")) {
        return
          target->isDerivedFrom<Core::Ast::ParamPass>() ||
          target->isDerivedFrom<Core::Ast::LinkOperator>() ||
          target->isDerivedFrom<Core::Ast::Identifier>() ||
          target->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
          target->isDerivedFrom<Spp::Ast::TypeOp>();
      }
    } else if (prop->getValue() == S("modifier") || prop->getValue() == S("مبدل")) {
      // Check the modifiers.
      auto def = ti_cast<Core::Ast::Definition>(target->getOwner());
      if (def == 0 || def->getModifiers() == 0) return false;
      auto modifierKwd = compareOp->getSecond().ti_cast_get<Core::Ast::Text const>();
      if (modifierKwd == 0) return false;
      for (Int i = 0; i < def->getModifiers()->getElementCount(); ++i) {
        auto modifier = def->getModifiers()->getElement(i);
        auto identifier = ti_cast<Core::Ast::Identifier>(modifier);
        if (identifier == 0) {
          auto paramPass = ti_cast<Core::Ast::ParamPass>(modifier);
          if (paramPass == 0) continue;
          identifier = paramPass->getOperand().ti_cast_get<Core::Ast::Identifier>();
          if (identifier == 0) continue;
        }
        if (identifier->getValue() == modifierKwd->getValue()) return true;
      }
    } else if (prop->getValue() == S("metadata") || prop->getValue() == S("بيانات_وصفية")) {
      // Check the metadata. We don't care about the actual value, only whether the
      // metadata with this name is set.
      auto def = ti_cast<Core::Ast::Definition>(target->getOwner());
      if (def == 0) return false;
      auto metadataName = compareOp->getSecond().ti_cast_get<Core::Ast::Text const>();
      if (metadataName == 0) return false;
      return def->getMetadata(metadataName->getValue()) != 0;
    }
  }
  return false;
}

} // namespace
