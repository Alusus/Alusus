/**
 * @file Spp/Ast/NodePathResolver.cpp
 * Contains the implementation of class Spp::Ast::NodePathResolver.
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

void NodePathResolver::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->resolve,
    &this->resolveDefinition,
    &this->resolveFunction,
    &this->resolveFunctionType,
    &this->resolveFunctionArg,
    &this->resolveTemplateInstance,
    &this->resolveOther
  });
}


void NodePathResolver::initBindings()
{
  this->resolve = &NodePathResolver::_resolve;
  this->resolveDefinition = &NodePathResolver::_resolveDefinition;
  this->resolveFunction = &NodePathResolver::_resolveFunction;
  this->resolveFunctionType = &NodePathResolver::_resolveFunctionType;
  this->resolveFunctionArg = &NodePathResolver::_resolveFunctionArg;
  this->resolveTemplateInstance = &NodePathResolver::_resolveTemplateInstance;
  this->resolveOther = &NodePathResolver::_resolveOther;
}


//==============================================================================
// Path Resolving Functions

void NodePathResolver::_resolve(TiObject *self, Core::Ast::Node const *node, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  if (node == 0) return;
  if (node->isDerivedFrom<Core::Ast::Definition>()) {
    auto def = static_cast<Core::Ast::Definition const*>(node);
    resolver->resolveDefinition(def, helper, path);
  } else if (node->isDerivedFrom<Spp::Ast::Function>()) {
    auto func = static_cast<Spp::Ast::Function const*>(node);
    resolver->resolveFunction(func, helper, path);
  } else if (node->isDerivedFrom<Spp::Ast::FunctionType>()) {
    auto funcType = static_cast<Spp::Ast::FunctionType const*>(node);
    resolver->resolveFunctionType(funcType, helper, path);
  } else if (
    node->isDerivedFrom<Core::Ast::Scope>() && Basic::isDerivedFrom<Spp::Ast::Template>(node->getOwner())
  ) {
    auto block = static_cast<Core::Ast::Scope const*>(node);
    resolver->resolveTemplateInstance(block, helper, path);
  } else {
    resolver->resolveOther(node, helper, path);
  }
}


void NodePathResolver::_resolveDefinition(
  TiObject *self, Core::Ast::Definition const *def, Helper *helper, StrStream &path
) {
  PREPARE_SELF(resolver, NodePathResolver);
  resolver->resolve(def->getOwner(), helper, path);
  if (path.tellp() > 0) path << C('.');
  path << def->getName().get();
  auto defOp = findOperationModifier(def);
  if (defOp != 0 && def->getName() != defOp) path << defOp;
}


void NodePathResolver::_resolveFunction(TiObject *self, Spp::Ast::Function const *func, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  resolver->resolve(func->getOwner(), helper, path);
  resolver->resolveFunctionType(func->getType().get(), helper, path);
}


// Resolve the path of the type of the given node. A type that can't be traced, like a reference to a missing symbol,
// has no path, so we'll use one that's unique to the node to avoid conflicts. Otherwise, for example, `func (j: Invalid)`
// would get the same path as `func ()`, and definitions using it, like the members of template instances, would
// conflict with those using `func ()` when generated, which still happens after build errors.
static void resolveTypeOf(NodePathResolver *resolver, Core::Ast::Node *node, Helper *helper, StrStream &path)
{
  auto type = helper->traceType(node, true);
  if (type == 0) path << S("?") << static_cast<void*>(node);
  else path << resolver->doResolve(type, helper);
}


void NodePathResolver::_resolveFunctionType(
  TiObject *self, Spp::Ast::FunctionType const *funcType, Helper *helper, StrStream &path
) {
  PREPARE_SELF(resolver, NodePathResolver);
  path << C('(');
  auto argTypes = funcType->getArgTypes().get();
  if (argTypes != 0) {
    for (Int i = 0; i < argTypes->getCount(); ++i) {
      if (i > 0) path << C(',');
      resolver->resolveFunctionArg(argTypes->getElement(i), helper, path);
    }
  }
  path << S(")");
  if (funcType->getRetType() != 0) {
    path << S("=>(");
    resolveTypeOf(resolver, funcType->getRetType().get(), helper, path);
    path << S(")");
  }
}


void NodePathResolver::_resolveFunctionArg(TiObject *self, Core::Ast::Node *arg, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  if (arg->isDerivedFrom<Ast::ArgPack>()) {
    auto argPack = static_cast<Ast::ArgPack const*>(arg);
    path << S("ArgPack[");
    if (argPack->getArgType() == 0) {
      path << S("any");
    } else {
      resolveTypeOf(resolver, argPack->getArgType().get(), helper, path);
    }
    path << C(',') << argPack->getMin().get() << C(',') << argPack->getMax().get() << C(']');
  } else {
    resolveTypeOf(resolver, arg, helper, path);
  }
}


void NodePathResolver::_resolveTemplateInstance(
  TiObject *self, Core::Ast::Scope const *block, Helper *helper, StrStream &path
) {
  PREPARE_SELF(resolver, NodePathResolver);
  auto tmplt = static_cast<Spp::Ast::Template*>(block->getOwner());
  resolver->resolve(tmplt->getOwner(), helper, path);
  path << C('[');
  auto varDefs = tmplt->getVarDefs();
  for (Int i = 0; i < varDefs->getCount(); ++i) {
    auto varDef = varDefs->get(i).ti_cast_get<Ast::TemplateVarDef>();
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Invalid template variable definition."));
    }
    if (i > 0) path << C(',');
    auto obj = Ast::Template::getTemplateVar(block, varDef->getName().getStr());
    if (varDef->getType() == Ast::TemplateVarType::INTEGER) {
      auto integer = ti_cast<Core::Ast::IntegerLiteral>(obj);
      if (integer == 0) {
        throw EXCEPTION(GenericException, S("Invalid template argument."));
      }
      path << integer->getValue().get();
    } else if (varDef->getType() == Ast::TemplateVarType::STRING) {
      auto str = ti_cast<Core::Ast::StringLiteral>(obj);
      if (str == 0) {
        throw EXCEPTION(GenericException, S("Invalid template argument."));
      }
      path << str->getValue().get();
    } else {
      path << resolver->doResolve(obj, helper);
    }
  }
  path << C(']');
  // Templates that cannot share instances can end up with multiple instances having the same args, so we need the
  // instance index to tell them apart.
  if (!tmplt->canShareInstances()) path << C('#') << tmplt->findInstanceIndex(block);
}


void NodePathResolver::_resolveOther(TiObject *self, Core::Ast::Node const *node, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  auto owner = node->getOwner();
  resolver->resolve(owner, helper, path);
  auto ownerContainer = ti_cast<Core::Basic::Containing<Core::Ast::Node>>(owner);
  if (
    ownerContainer != 0 &&
    !Basic::isDerivedFrom<Core::Ast::Definition>(owner) &&
    !Basic::isDerivedFrom<Spp::Ast::Function>(owner) &&
    !Basic::isDerivedFrom<Spp::Ast::Type>(owner) &&
    !Basic::isDerivedFrom<Spp::Ast::Template>(owner->getOwner())
  ) {
    for (Int i = 0; i < ownerContainer->getElementCount(); ++i) {
      if (ownerContainer->getElement(i) == node) {
        path << C('.') << i;
        break;
      }
    }
  }
}

} } // namespace
