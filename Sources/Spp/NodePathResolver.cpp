/**
 * @file Spp/NodePathResolver.cpp
 * Contains the implementation of class Spp::NodePathResolver.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

void NodePathResolver::initialize()
{
  this->setFunction(this->resolve, &NodePathResolver::_resolve);
  this->setFunction(this->resolveDefinition, &NodePathResolver::_resolveDefinition);
  this->setFunction(this->resolveFunction, &NodePathResolver::_resolveFunction);
  this->setFunction(this->resolveTemplateInstance, &NodePathResolver::_resolveTemplateInstance);
}


//==============================================================================
// Helper Functions

Spp::Ast::Type* NodePathResolver::traceType(TiObject *ref)
{
  Spp::Ast::Type *result = 0;
  auto *refNode = ti_cast<Core::Data::Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  this->seeker->doForeach(refNode, refNode->getOwner(), [=, &result](TiObject *obj)->Core::Data::Seeker::SeekVerb
  {
    if (obj->isDerivedFrom<Spp::Ast::Type>()) {
      result = static_cast<Spp::Ast::Type*>(obj);
      return Core::Data::Seeker::SeekVerb::STOP;
    }
    // TODO: Recurse if the object is an Alias.
    return Core::Data::Seeker::SeekVerb::MOVE;
  });
  if (result == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  return result;
}


//==============================================================================
// Path Resolving Functions

void NodePathResolver::_resolve(Bindings *_self, Core::Data::Node const *node, StrStream &path)
{
  PREPARE_SELF(NodePathResolver);
  if (node == 0) return;
  if (node->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition const*>(node);
    self->call<void, Core::Data::Ast::Definition const*, StrStream&>(self->resolveDefinition, def, path);
  } else if (node->isDerivedFrom<Spp::Ast::Function>()) {
    auto func = static_cast<Spp::Ast::Function const*>(node);
    self->call<void, Spp::Ast::Function const*, StrStream&>(self->resolveFunction, func, path);
  } else if (node->isDerivedFrom<Spp::Ast::Block>() &&
             Core::Basic::isDerivedFrom<Spp::Ast::Template>(node->getOwner())) {
    auto block = static_cast<Spp::Ast::Block const*>(node);
    self->call<void, Spp::Ast::Block const*, StrStream&>(self->resolveTemplateInstance, block, path);
  } else {
    NodePathResolver::_resolve(_self, node->getOwner(), path);
  }
}


void NodePathResolver::_resolveDefinition(Bindings *_self, Core::Data::Ast::Definition const *def, StrStream &path)
{
  PREPARE_SELF(NodePathResolver);
  self->call<void, Core::Data::Node const*, StrStream&>(self->resolve, def->getOwner(), path);
  if (path.rdbuf()->in_avail() != 0) path << CHR('.');
  path << def->getName().get();
}


void NodePathResolver::_resolveFunction(Bindings *_self, Spp::Ast::Function const *func, StrStream &path)
{
  PREPARE_SELF(NodePathResolver);
  self->call<void, Core::Data::Node const*, StrStream&>(self->resolve, func->getOwner(), path);
  path << CHR('(');
  auto argTypes = func->getArgTypes().get();
  if (argTypes != 0) {
    for (Int i = 0; i < argTypes->getCount(); ++i) {
      if (i > 0) path << CHR(',');
      auto type = self->traceType(argTypes->get(i));
      path << self->doResolve(type);
    }
  }
  path << STR(")");
  if (func->getRetType() != 0) {
    auto type = self->traceType(func->getRetType().get());
    path << STR("->(") << self->doResolve(type) << STR(")");
  }
}


void NodePathResolver::_resolveTemplateInstance(Bindings *_self, Spp::Ast::Block const *block, StrStream &path)
{
  PREPARE_SELF(NodePathResolver);
  auto tmplt = static_cast<Spp::Ast::Template*>(block->getOwner());
  self->call<void, Core::Data::Node const*, StrStream&>(self->resolve, tmplt->getOwner(), path);
  path << CHR('[');
  auto varDefs = tmplt->getVarDefs();
  for (Int i = 0; i < varDefs->size(); ++i) {
    if (i > 0) path << CHR(',');
    auto obj = Ast::Template::getTemplateVar(block, varDefs->at(i).first.c_str());
    if (varDefs->at(i).second == Ast::Template::VarType::INTEGER) {
      auto intLiteral = ti_cast<Core::Data::Ast::IntegerLiteral>(obj);
      if (intLiteral == 0) {
        throw EXCEPTION(GenericException, STR("Invalid template argument."));
      }
      path << intLiteral->getValue().get();
    } else if (varDefs->at(i).second == Ast::Template::VarType::STRING) {
      auto strLiteral = ti_cast<Core::Data::Ast::StringLiteral>(obj);
      if (strLiteral == 0) {
        throw EXCEPTION(GenericException, STR("Invalid template argument."));
      }
      path << strLiteral->getValue().get();
    } else {
      path << self->doResolve(ti_cast<Core::Data::Node>(obj));
    }
  }
  path << CHR(']');
}

} // namespace
