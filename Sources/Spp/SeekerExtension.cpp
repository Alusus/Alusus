/**
 * @file Spp/SeekerExtension.cpp
 * Contains the implementation of class Spp::SeekerExtension.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
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

SeekerExtension::Overrides* SeekerExtension::extend(Core::Data::Seeker *seeker, SharedPtr<Ast::Helper> const &astHelper)
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


void SeekerExtension::unextend(Core::Data::Seeker *seeker, Overrides *overrides)
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

Core::Data::Seeker::Verb SeekerExtension::_foreach(
  TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  if (ref->isA<Data::Ast::ParamPass>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_paramPass(static_cast<Data::Ast::ParamPass const*>(ref), target, cb, flags);
  } else if (ref->isA<Spp::Ast::ThisTypeRef>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_thisTypeRef(target, cb, flags);
  } else if (ref->isA<Core::Data::Ast::ComparisonOperator>() || ref->isA<Core::Data::Ast::LogOperator>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_comparison(ref, target, cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Data::Seeker);
    return seeker->foreach.useCallee(base)(ref, target, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_extForeach(
  TiFunctionBase *base, TiObject *self, TiObject const *ref, TiObject *target,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  Int tracingAlias = 0;
  Int tracingUse = 0;
  return seeker->foreach(ref, target,
    [flags, cb, &tracingAlias, &tracingUse](TiInt action, TiObject *o)->Core::Data::Seeker::Verb {
      if (action == Core::Data::Seeker::Action::ALIAS_TRACE_START) {
        ++tracingAlias;
        return Core::Data::Seeker::Verb::MOVE;
      } else if (action == Core::Data::Seeker::Action::ALIAS_TRACE_END) {
        --tracingAlias;
        return Core::Data::Seeker::Verb::MOVE;
      } else if (action == Core::Data::Seeker::Action::OWNER_SCOPE) {
        if (tracingAlias == 0 && (flags & Core::Data::Seeker::Flags::SKIP_OWNERS) != 0) {
          return Core::Data::Seeker::Verb::SKIP_GROUP;
        } else return Core::Data::Seeker::Verb::MOVE;
      } else if (action == Core::Data::Seeker::Action::USE_SCOPES_START) {
        if (
          (tracingUse > 0 || (flags & Core::Data::Seeker::Flags::SKIP_USES) != 0) &&
          (tracingAlias == 0 || (flags & Core::Data::Seeker::Flags::SKIP_USES_FOR_ALIASES) != 0)
        ) {
          return Core::Data::Seeker::Verb::SKIP;
        } else {
          ++tracingUse;
          return Core::Data::Seeker::Verb::MOVE;
        }
      } else if (action == Core::Data::Seeker::Action::USE_SCOPES_END) {
        --tracingUse;
        return Core::Data::Seeker::Verb::MOVE;
      } else if (action == SeekerExtension::Action::CHILD_SCOPE) {
        if ((flags & SeekerExtension::Flags::SKIP_CHILDREN) != 0) return Core::Data::Seeker::Verb::SKIP;
      } else if (
        action != Core::Data::Seeker::Action::TARGET_MATCH &&
        action != Core::Data::Seeker::Action::ERROR
      ) {
        return Core::Data::Seeker::Verb::MOVE;
      }
      return cb(action, o);
    },
    flags
  );
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_identifierLevel(
  TiFunctionBase *base, TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  if (data->isDerivedFrom<Ast::Function>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_identifierOnFunc(identifier, static_cast<Ast::Function*>(data), cb, flags);
  } else if (data->isDerivedFrom<Ast::DataType>()) {
    PREPARE_SELF(seekerExtension, SeekerExtension);
    return seekerExtension->foreach_identifierOnDataType(identifier, static_cast<Ast::DataType*>(data), cb, flags);
  } else {
    PREPARE_SELF(seeker, Core::Data::Seeker);
    return seeker->foreach_identifierLevel.useCallee(base)(identifier, data, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_identifierOnFunc(
  TiObject *self, Data::Ast::Identifier const *identifier, Ast::Function *function,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  auto argTypes = function->getType()->getArgTypes().get();
  if (argTypes == 0) return Core::Data::Seeker::Verb::MOVE;
  auto index = argTypes->findIndex(identifier->getValue().get());
  if (index >= 0) return cb(Core::Data::Seeker::Action::TARGET_MATCH, argTypes->getElement(index));
  return Core::Data::Seeker::Verb::MOVE;
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_identifierOnDataType(
  TiObject *self, Data::Ast::Identifier const *identifier, Ast::DataType *type,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  auto block = type->getBody().get();
  if ((block == 0) || (flags & Core::Data::Seeker::Flags::SKIP_OWNED)) {
    return Core::Data::Seeker::Verb::MOVE;
  } else {
    return seeker->foreach_identifierLevel(identifier, block, cb, flags);
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_linkOperatorRouting(
  TiFunctionBase *base, TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  if (link->getType() == S(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Core::Data::Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::DataType>()) {
        auto block = static_cast<Ast::DataType*>(data)->getBody().get();
        if ((block == 0) || (flags & Core::Data::Seeker::Flags::SKIP_OWNED)) {
          return Core::Data::Seeker::Verb::MOVE;
        }
        data = block;
      }
    }
  }
  return seeker->foreach_linkOperatorRouting.useCallee(base)(link, data, cb, flags);
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_paramPass(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Core::Data::Seeker);
  PREPARE_SELF(seekerExtension, SeekerExtension);
  auto operand = paramPass->getOperand().get();
  return seeker->foreach(operand, data,
    [=](TiInt action, TiObject *newData)->Core::Data::Seeker::Verb
    {
      if (action == Core::Data::Seeker::Action::TARGET_MATCH) {
        return seekerExtension->foreach_paramPassRouting(paramPass, newData, cb, flags);
      } else return cb(action, newData);
    },
    flags
  );
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_paramPassRouting(
  TiObject *self, Data::Ast::ParamPass const *paramPass, TiObject *data,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
    auto param = paramPass->getParam().get();
    if (data->isDerivedFrom<Ast::Template>()) {
      return seekerExtension->foreach_paramPassOnTemplate(param, static_cast<Ast::Template*>(data), cb, flags);
    } else {
      auto notice = newSrdObj<Spp::Notices::InvalidSquareBracketOperandNotice>(
        Core::Data::Ast::findSourceLocation(paramPass)
      );
      return cb(Core::Data::Seeker::Action::ERROR, notice.get());
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, S("paramPass"), S("Invalid bracket type."), paramPass->getType());
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_paramPassOnTemplate(
  TiObject *self, TiObject *param, Ast::Template *tmplt, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  TioSharedPtr result;
  if (tmplt->matchInstance(param, seekerExtension->astHelper, result)) {
    return cb(Core::Data::Seeker::Action::TARGET_MATCH, result.get());
  } else {
    auto notice = result.ti_cast_get<Core::Notices::Notice>();
    if (notice != 0) return cb(Core::Data::Seeker::Action::ERROR, notice);
    else return Core::Data::Seeker::Verb::MOVE;
  }
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_thisTypeRef(
  TiObject *self, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  auto node = ti_cast<Core::Data::Node>(data);
  while (node != 0) {
    if (node->isDerivedFrom<Spp::Ast::DataType>()) {
      return cb(Core::Data::Seeker::Action::TARGET_MATCH, node);
    }
    node = node->getOwner();
  }
  return Core::Data::Seeker::Verb::MOVE;
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_comparison(
  TiObject *self, TiObject const *comparison, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  Core::Data::Seeker::Verb retVal = Core::Data::Seeker::Verb::MOVE;
  if (data->isDerivedFrom<Core::Data::DataStack>()) {
    auto stack = static_cast<Core::Data::DataStack*>(data);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      auto data = stack->getElement(i);
      if (data == 0) continue;
      if (i != stack->getCount() - 1) {
        retVal = cb(Core::Data::Seeker::Action::OWNER_SCOPE, data);
        if (retVal == Core::Data::Seeker::Verb::SKIP) continue;
        else if (retVal == Core::Data::Seeker::Verb::SKIP_GROUP || !Core::Data::Seeker::isMove(retVal)) break;
      }
      retVal = seekerExtension->foreach_comparisonLevel(comparison, data, cb, flags);
      if (!Core::Data::Seeker::isMove(retVal)) return retVal;
    }
  } else if (data->isDerivedFrom<Core::Data::Node>()) {
    auto node = static_cast<Core::Data::Node*>(data);
    while (node != 0) {
      if (node != data) {
        retVal = cb(Core::Data::Seeker::Action::OWNER_SCOPE, node);
        if (retVal == Core::Data::Seeker::Verb::SKIP) {
          node = node->getOwner();
          continue;
        } else if (retVal == Core::Data::Seeker::Verb::SKIP_GROUP || !Core::Data::Seeker::isMove(retVal)) break;
      }
      retVal = seekerExtension->foreach_comparisonLevel(comparison, node, cb, flags);
      if (!Core::Data::Seeker::isMove(retVal)) return retVal;
      node = node->getOwner();
      flags |= Core::Data::Seeker::Flags::SKIP_OWNED;
    }
  }
  return retVal;
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_comparisonLevel(
  TiObject *self, TiObject const *comparison, TiObject *data, Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (data->isDerivedFrom<Core::Data::Ast::Scope>()) {
    return seekerExtension->foreach_comparisonOnScope(
      comparison, static_cast<Core::Data::Ast::Scope*>(data), cb, flags
    );
  } else if (data->isDerivedFrom<Ast::DataType>()) {
    auto dataType = static_cast<Ast::DataType*>(data);
    if (dataType->getBody() != 0) {
      return seekerExtension->foreach_comparisonOnScope(comparison, dataType->getBody().get(), cb, flags);
    }
  }
  return Core::Data::Seeker::Verb::MOVE;
}


Core::Data::Seeker::Verb SeekerExtension::_foreach_comparisonOnScope(
  TiObject *self, TiObject const *comparison, Core::Data::Ast::Scope *scope,
  Core::Data::Seeker::ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seekerExtension, SeekerExtension);
  Core::Data::Seeker::Verb verb = Core::Data::Seeker::Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (seekerExtension->foreach_computeComparison(comparison, obj)) {
        verb = cb(Core::Data::Seeker::Action::TARGET_MATCH, obj);
        if (!Core::Data::Seeker::isMove(verb)) return verb;
      }
      verb = cb(SeekerExtension::Action::CHILD_SCOPE, obj);
      if (verb != Core::Data::Seeker::Verb::SKIP) {
        verb = seekerExtension->foreach_comparisonLevel(comparison, obj, cb, flags);
      }
      if (!Core::Data::Seeker::isMove(verb)) return verb;
    }
  }

  if (scope->getBridgeCount() > 0) {
    verb = cb(Core::Data::Seeker::Action::USE_SCOPES_START, scope);
    if (verb == Core::Data::Seeker::Verb::SKIP) return verb;
    PREPARE_SELF(seeker, Core::Data::Seeker);
    for (Int i = 0; i < scope->getBridgeCount(); ++i) {
      auto bridgeRef = scope->getBridge(i);
      auto bridgeTarget = seeker->tryGet(
        bridgeRef->getTarget().get(), scope,
        Core::Data::Seeker::Flags::SKIP_USES | Core::Data::Seeker::Flags::SKIP_USES_FOR_ALIASES
      );
      if (bridgeTarget == 0) continue;
      verb = cb(Core::Data::Seeker::Action::USE_SCOPE, bridgeTarget);
      if (verb == Core::Data::Seeker::Verb::SKIP) continue;
      else if (verb == Core::Data::Seeker::Verb::SKIP_GROUP || !Core::Data::Seeker::isMove(verb)) break;

      verb = seekerExtension->foreach_comparisonLevel(comparison, bridgeTarget, cb, flags);
      if (!Core::Data::Seeker::isMove(verb)) return verb;
    }
    verb = cb(Core::Data::Seeker::Action::USE_SCOPES_END, scope);
  }

  return verb;
}


Bool SeekerExtension::_foreach_computeComparison(TiObject *self, TiObject const *comparison, TiObject *target)
{
  PREPARE_SELF(seekerExtension, SeekerExtension);
  if (comparison->isDerivedFrom<Core::Data::Ast::LogOperator>()) {
    auto logOp = static_cast<Core::Data::Ast::LogOperator const*>(comparison);
    if (logOp->getType() == S("&&") || logOp->getType() == S("and")) {
      if (!seekerExtension->foreach_computeComparison(logOp->getFirst().get(), target)) return false;
      return seekerExtension->foreach_computeComparison(logOp->getSecond().get(), target);
    } else if (logOp->getType() == S("||") || logOp->getType() == S("or")) {
      if (seekerExtension->foreach_computeComparison(logOp->getFirst().get(), target)) return true;
      return seekerExtension->foreach_computeComparison(logOp->getSecond().get(), target);
    }
  } else if (comparison->isDerivedFrom<Core::Data::Ast::ComparisonOperator>()) {
    auto compareOp = static_cast<Core::Data::Ast::ComparisonOperator const*>(comparison);
    if (compareOp->getType() != S("==")) return false;
    auto prop = compareOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier const>();
    if (prop == 0) return false;
    if (prop->getValue() == S("elementType") || prop->getValue() == S("نوع_العنصر")) {
      // Check the type.
      auto type = compareOp->getSecond().ti_cast_get<Core::Data::Ast::Text const>();
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
          target->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
          target->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
          target->isDerivedFrom<Core::Data::Ast::Identifier>() ||
          target->isDerivedFrom<Spp::Ast::ThisTypeRef>() ||
          target->isDerivedFrom<Spp::Ast::TypeOp>();
      }
    } else if (prop->getValue() == S("modifier") || prop->getValue() == S("مبدل")) {
      // Check the modifiers.
      auto node = ti_cast<Core::Data::Node>(target);
      if (node == 0) return false;
      auto def = ti_cast<Core::Data::Ast::Definition>(node->getOwner());
      if (def == 0 || def->getModifiers() == 0) return false;
      auto modifierKwd = compareOp->getSecond().ti_cast_get<Core::Data::Ast::Text const>();
      if (modifierKwd == 0) return false;
      for (Int i = 0; i < def->getModifiers()->getElementCount(); ++i) {
        auto modifier = def->getModifiers()->getElement(i);
        auto identifier = ti_cast<Core::Data::Ast::Identifier>(modifier);
        if (identifier == 0) {
          auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(modifier);
          if (paramPass == 0) continue;
          identifier = paramPass->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
          if (identifier == 0) continue;
        }
        if (identifier->getValue() == modifierKwd->getValue()) return true;
      }
    }
  }
  return false;
}

} // namespace
