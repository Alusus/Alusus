/**
 * @file Spp/Ast/NodePathResolver.cpp
 * Contains the implementation of class Spp::Ast::NodePathResolver.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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

void NodePathResolver::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->resolve,
    &this->resolveDefinition,
    &this->resolveFunction,
    &this->resolveFunctionArg,
    &this->resolveTemplateInstance
  });
}


void NodePathResolver::initBindings()
{
  this->resolve = &NodePathResolver::_resolve;
  this->resolveDefinition = &NodePathResolver::_resolveDefinition;
  this->resolveFunction = &NodePathResolver::_resolveFunction;
  this->resolveFunctionArg = &NodePathResolver::_resolveFunctionArg;
  this->resolveTemplateInstance = &NodePathResolver::_resolveTemplateInstance;
}


//==============================================================================
// Path Resolving Functions

void NodePathResolver::_resolve(TiObject *self, Core::Data::Node const *node, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  if (node == 0) return;
  if (node->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition const*>(node);
    resolver->resolveDefinition(def, helper, path);
  } else if (node->isDerivedFrom<Spp::Ast::Function>()) {
    auto func = static_cast<Spp::Ast::Function const*>(node);
    resolver->resolveFunction(func, helper, path);
  } else if (node->isDerivedFrom<Spp::Ast::Block>() &&
             Core::Basic::isDerivedFrom<Spp::Ast::Template>(node->getOwner())) {
    auto block = static_cast<Spp::Ast::Block const*>(node);
    resolver->resolveTemplateInstance(block, helper, path);
  } else {
    NodePathResolver::_resolve(self, node->getOwner(), helper, path);
  }
}


void NodePathResolver::_resolveDefinition(
  TiObject *self, Core::Data::Ast::Definition const *def, Helper *helper, StrStream &path
) {
  PREPARE_SELF(resolver, NodePathResolver);
  resolver->resolve(def->getOwner(), helper, path);
  if (path.rdbuf()->in_avail() != 0) path << CHR('.');
  path << def->getName().get();
}


void NodePathResolver::_resolveFunction(TiObject *self, Spp::Ast::Function const *func, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  resolver->resolve(func->getOwner(), helper, path);
  path << CHR('(');
  auto argTypes = func->getArgTypes().get();
  if (argTypes != 0) {
    for (Int i = 0; i < argTypes->getCount(); ++i) {
      if (i > 0) path << CHR(',');
      resolver->resolveFunctionArg(argTypes->getElement(i), helper, path);
    }
  }
  path << STR(")");
  if (func->getRetType() != 0) {
    auto type = helper->traceType(func->getRetType().get());
    path << STR("=>(") << resolver->doResolve(type, helper) << STR(")");
  }
}


void NodePathResolver::_resolveFunctionArg(TiObject *self, TiObject *arg, Helper *helper, StrStream &path)
{
  PREPARE_SELF(resolver, NodePathResolver);
  if (arg->isDerivedFrom<Ast::ArgPack>()) {
    auto argPack = static_cast<Ast::ArgPack const*>(arg);
    path << STR("ArgPack[");
    if (argPack->getArgType() == 0) {
      path << STR("any");
    } else {
      auto type = helper->traceType(argPack->getArgType().get());
      path << resolver->doResolve(type, helper);
    }
    path << CHR(',') << argPack->getMin().get() << CHR(',') << argPack->getMax().get() << CHR(']');
  } else {
    auto type = helper->traceType(arg);
    path << resolver->doResolve(type, helper);
  }
}


void NodePathResolver::_resolveTemplateInstance(
  TiObject *self, Spp::Ast::Block const *block, Helper *helper, StrStream &path
) {
  PREPARE_SELF(resolver, NodePathResolver);
  auto tmplt = static_cast<Spp::Ast::Template*>(block->getOwner());
  resolver->resolve(tmplt->getOwner(), helper, path);
  path << CHR('[');
  auto varDefs = tmplt->getVarDefs();
  for (Int i = 0; i < varDefs->size(); ++i) {
    if (i > 0) path << CHR(',');
    auto obj = Ast::Template::getTemplateVar(block, varDefs->at(i).first.c_str());
    if (varDefs->at(i).second == Ast::Template::VarType::INTEGER) {
      auto integer = ti_cast<Core::Basic::TiInt>(obj);
      if (integer == 0) {
        throw EXCEPTION(GenericException, STR("Invalid template argument."));
      }
      path << integer->get();
    } else if (varDefs->at(i).second == Ast::Template::VarType::STRING) {
      auto str = ti_cast<TiStr>(obj);
      if (str == 0) {
        throw EXCEPTION(GenericException, STR("Invalid template argument."));
      }
      path << str->get();
    } else {
      path << resolver->doResolve(ti_cast<Core::Data::Node>(obj), helper);
    }
  }
  path << CHR(']');
}

} } // namespace
