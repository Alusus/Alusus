/**
 * @file Core/Data/Seeker.cpp
 * Contains the implementation of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Initialization

void Seeker::initialize()
{
  // Main seek functions
  this->setFunction(this->set, &Seeker::_set);
  this->setFunction(this->remove, &Seeker::_remove);
  this->setFunction(this->foreach, &Seeker::_foreach);

  // Identifier seek functions
  this->setFunction(this->setByIdentifier, &Seeker::_setByIdentifier);
  this->setFunction(this->setByIdentifier_sharedRepository, &Seeker::_setByIdentifier_sharedRepository);
  this->setFunction(this->setByIdentifier_ast, &Seeker::_setByIdentifier_ast);
  this->setFunction(this->removeByIdentifier, &Seeker::_removeByIdentifier);
  this->setFunction(this->removeByIdentifier_sharedRepository, &Seeker::_removeByIdentifier_sharedRepository);
  this->setFunction(this->removeByIdentifier_ast, &Seeker::_removeByIdentifier_ast);
  this->setFunction(this->foreachByIdentifier, &Seeker::_foreachByIdentifier);
  this->setFunction(this->foreachByIdentifier_sharedRepository, &Seeker::_foreachByIdentifier_sharedRepository);
  this->setFunction(this->foreachByIdentifier_ast, &Seeker::_foreachByIdentifier_ast);

  // LinkOperator seek functions
  this->setFunction(this->setByLinkOperator, &Seeker::_setByLinkOperator);
  this->setFunction(this->setByLinkOperator_routing, &Seeker::_setByLinkOperator_routing);
  this->setFunction(this->setByLinkOperator_scopeDotIdentifier, &Seeker::_setByLinkOperator_scopeDotIdentifier);
  this->setFunction(this->setByLinkOperator_mapDotIdentifier, &Seeker::_setByLinkOperator_mapDotIdentifier);
  this->setFunction(this->removeByLinkOperator, &Seeker::_removeByLinkOperator);
  this->setFunction(this->removeByLinkOperator_routing, &Seeker::_removeByLinkOperator_routing);
  this->setFunction(this->removeByLinkOperator_scopeDotIdentifier, &Seeker::_removeByLinkOperator_scopeDotIdentifier);
  this->setFunction(this->removeByLinkOperator_mapDotIdentifier, &Seeker::_removeByLinkOperator_mapDotIdentifier);
  this->setFunction(this->foreachByLinkOperator, &Seeker::_foreachByLinkOperator);
  this->setFunction(this->foreachByLinkOperator_routing, &Seeker::_foreachByLinkOperator_routing);
  this->setFunction(this->foreachByLinkOperator_scopeDotIdentifier, &Seeker::_foreachByLinkOperator_scopeDotIdentifier);
  this->setFunction(this->foreachByLinkOperator_mapDotIdentifier, &Seeker::_foreachByLinkOperator_mapDotIdentifier);
}


//==============================================================================
// Helper Functions

Bool Seeker::trySet(TiObject const *ref, TiObject *target, TiObject *val)
{
  Bool result = false;
  this->doSet(ref, target, [=,&result](TiObject *&obj)->SeekVerb {
    obj = val;
    result = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return result;
}


Bool Seeker::tryRemove(TiObject const *ref, TiObject *target)
{
  Bool ret = false;
  this->doRemove(ref, target, [&ret](TiObject *o)->SeekVerb {
    ret = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return ret;
}


Bool Seeker::tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal)
{
  Bool ret = false;
  this->doForeach(ref, target, [&ret,&retVal](TiObject *o)->SeekVerb {
    retVal = o;
    ret = true;
    return SeekVerb::STOP;
  });
  return ret;
}


//==============================================================================
// Main Seek Functions

void Seeker::_set(Bindings *_self, TiObject const *ref, TiObject *target, SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  if (ref->isA<Ast::Identifier>()) {
    self->call<void>(self->setByIdentifier, static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    self->call<void>(self->setByLinkOperator, static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void Seeker::_remove(Bindings *_self, TiObject const *ref, TiObject *target, SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  if (ref->isA<Ast::Identifier>()) {
    self->call<void>(self->removeByIdentifier, static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    self->call<void>(self->removeByLinkOperator, static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void Seeker::_foreach(Bindings *_self, TiObject const *ref, TiObject *target, SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  if (ref->isA<Ast::Identifier>()) {
    self->call<void>(self->foreachByIdentifier, static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    self->call<void>(self->foreachByLinkOperator, static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


//==============================================================================
// Identifier set

void Seeker::_setByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                              SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    self->call<void>(self->setByIdentifier_sharedRepository, identifier, static_cast<Data::SharedRepository*>(data), cb);
  } else {
    self->call<void>(self->setByIdentifier_ast, identifier, data, cb);
  }
}


void Seeker::_setByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                               Data::SharedRepository *repo, SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = repo->getLevelCount() - 1; i >= 0; --i) {
    auto node = repo->getLevelData(i).ti_cast_get<Node>();
    if (node != 0 && node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(obj);
        if (isPerform(verb)) {
          // Add a new definition.
          auto def = Data::Ast::Definition::create();
          def->setName(identifier->getValue());
          def->setTarget(getSharedPtr(obj));
          scope->add(def);
        }
      }
      if (!isMove(verb)) break;
    }
  }
}


void Seeker::_setByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                  SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  auto node = ti_cast<Node>(data);
  SeekVerb verb = SeekVerb::MOVE;
  while (node != 0) {
    if (node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(obj);
        if (isPerform(verb)) {
          // Add a new definition.
          auto def = Data::Ast::Definition::create();
          def->setName(identifier->getValue());
          def->setTarget(getSharedPtr(obj));
          scope->add(def);
        }
      }
      if (!isMove(verb)) break;
    }
    node = node->getOwner();
  }
}


//==============================================================================
// Identifier remove

void Seeker::_removeByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                 SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    self->call<void>(self->removeByIdentifier_sharedRepository,
      identifier, static_cast<Data::SharedRepository*>(data), cb
    );
  } else {
    self->call<void>(self->removeByIdentifier_ast, identifier, data, cb);
  }
}


void Seeker::_removeByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                  Data::SharedRepository *repo, SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = repo->getLevelCount() - 1; i >= 0; --i) {
    auto node = repo->getLevelData(i).ti_cast_get<Node>();
    if (node != 0 && node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (isPerform(verb)) {
            scope->remove(i);
            --i;
          }
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
  }
}


void Seeker::_removeByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                     SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  auto node = ti_cast<Node>(data);
  SeekVerb verb = SeekVerb::MOVE;
  while (node != 0) {
    if (node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (isPerform(verb)) {
            scope->remove(i);
            --i;
          }
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
    node = node->getOwner();
  }
}


//==============================================================================
// Identifier foreach

void Seeker::_foreachByIdentifier(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                  SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    self->call<void>(self->foreachByIdentifier_sharedRepository,
      identifier, static_cast<Data::SharedRepository*>(data), cb
    );
  } else {
    self->call<void>(self->foreachByIdentifier_ast, identifier, data, cb);
  }
}


void Seeker::_foreachByIdentifier_sharedRepository(Bindings *_self, Data::Ast::Identifier const *identifier,
                                                   Data::SharedRepository *repo, SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = repo->getLevelCount() - 1; i >= 0; --i) {
    auto node = repo->getLevelData(i).ti_cast_get<Node>();
    if (node != 0 && node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
  }
}


void Seeker::_foreachByIdentifier_ast(Bindings *_self, Data::Ast::Identifier const *identifier, TiObject *data,
                                      SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  auto node = ti_cast<Node>(data);
  SeekVerb verb = SeekVerb::MOVE;
  while (node != 0) {
    if (node->isDerivedFrom<Ast::Scope>()) {
      // Look into the scope for the given identifier.
      auto scope = static_cast<Ast::Scope*>(node);
      for (Int i = 0; i < scope->getCount(); ++i) {
        auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
        if (def != 0 && def->getName() == identifier->getValue()) {
          auto obj = def->getTarget().get();
          verb = cb(obj);
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
    node = node->getOwner();
  }
}


//==============================================================================
// LinkOperator set

void Seeker::_setByLinkOperator(Bindings *_self, Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  auto first = link->getFirst().get();
  self->doForeach(first, data,
    [=](TiObject *newData)->SeekVerb
    {
      return self->call<Seeker::SeekVerb>(self->setByLinkOperator_routing, link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_setByLinkOperator_routing(Bindings *_self, Ast::LinkOperator const *link, TiObject *data,
                                                    SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return self->call<Seeker::SeekVerb>(self->setByLinkOperator_scopeDotIdentifier,
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return self->call<Seeker::SeekVerb>(self->setByLinkOperator_mapDotIdentifier,
            static_cast<Ast::Identifier*>(second), map, cb
          );
        } else {
          throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Unrecognized target data type."));
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unknown link operator type."), link->getType());
  }
}


Seeker::SeekVerb Seeker::_setByLinkOperator_scopeDotIdentifier(Bindings *_self, Ast::Identifier const *identifier,
                                                               Ast::Scope *scope, SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj);
      if (isPerform(verb)) {
        def->setTarget(getSharedPtr(obj));
      }
      if (!isMove(verb)) break;
    }
  }
  if (isMove(verb)) {
    TiObject *obj = 0;
    verb = cb(obj);
    if (isPerform(verb)) {
      // Add a new definition.
      auto def = Data::Ast::Definition::create();
      def->setName(identifier->getValue());
      def->setTarget(getSharedPtr(obj));
      scope->add(def);
    }
  }
  return verb;
}


Seeker::SeekVerb Seeker::_setByLinkOperator_mapDotIdentifier(Bindings *_self, Ast::Identifier const *identifier,
                                                             MapContainer *map, SeekSetCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  auto obj = map->get(identifier->getValue().get());
  verb = cb(obj);
  if (isPerform(verb)) {
    map->set(identifier->getValue().get(), obj);
  }
  return verb;
}


//==============================================================================
// LinkOperator remove

void Seeker::_removeByLinkOperator(Bindings *_self, Data::Ast::LinkOperator const *link, TiObject *data,
                                   SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  auto first = link->getFirst().get();
  self->doForeach(first, data,
    [=](TiObject *newData)->SeekVerb
    {
      return self->call<Seeker::SeekVerb>(self->removeByLinkOperator_routing, link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_removeByLinkOperator_routing(Bindings *_self, Data::Ast::LinkOperator const *link,
                                                       TiObject *data,
                                                       SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return self->call<Seeker::SeekVerb>(self->removeByLinkOperator_scopeDotIdentifier,
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return self->call<Seeker::SeekVerb>(self->removeByLinkOperator_mapDotIdentifier,
            static_cast<Ast::Identifier*>(second), map, cb
          );
        } else {
          throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Unrecognized target data type."));
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unknown link operator type."), link->getType());
  }
}


Seeker::SeekVerb Seeker::_removeByLinkOperator_scopeDotIdentifier(Bindings *_self,
                                                                  Data::Ast::Identifier const *identifier,
                                                                  Data::Ast::Scope *scope, SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj);
      if (isPerform(verb)) {
        scope->remove(i);
        --i;
      }
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


Seeker::SeekVerb Seeker::_removeByLinkOperator_mapDotIdentifier(Bindings *_self,
                                                                Data::Ast::Identifier const *identifier,
                                                                Data::MapContainer *map, SeekRemoveCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  auto index = map->findIndex(identifier->getValue().get());
  if (index != -1) {
    auto obj = map->get(index);
    verb = cb(obj);
    if (isPerform(verb)) {
      map->remove(index);
    }
  }
  return verb;
}


//==============================================================================
// LinkOperator foreach

void Seeker::_foreachByLinkOperator(Bindings *_self, Data::Ast::LinkOperator const *link, TiObject *data,
                                    SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  auto first = link->getFirst().get();
  self->doForeach(first, data,
    [=](TiObject *newData)->SeekVerb
    {
      return self->call<Seeker::SeekVerb>(self->foreachByLinkOperator_routing, link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_foreachByLinkOperator_routing(Bindings *_self, Data::Ast::LinkOperator const *link,
                                                        TiObject *data, SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return self->call<Seeker::SeekVerb>(self->foreachByLinkOperator_scopeDotIdentifier,
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return self->call<Seeker::SeekVerb>(self->foreachByLinkOperator_mapDotIdentifier,
            static_cast<Ast::Identifier*>(second), map, cb
          );
        } else {
          throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Unrecognized target data type."));
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("link"), STR("Unknown link operator type."), link->getType());
  }
}


Seeker::SeekVerb Seeker::_foreachByLinkOperator_scopeDotIdentifier(Bindings *_self, Data::Ast::Identifier *identifier,
                                                                   Data::Ast::Scope *scope, SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj);
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


Seeker::SeekVerb Seeker::_foreachByLinkOperator_mapDotIdentifier(Bindings *_self,
                                                                 Data::Ast::Identifier const *identifier,
                                                                 Data::MapContainer *map, SeekForeachCallback cb)
{
  PREPARE_SELF(Seeker);
  auto obj = map->get(identifier->getValue().get());
  return cb(obj);
}

} } // namespace
