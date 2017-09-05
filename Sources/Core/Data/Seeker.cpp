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

void Seeker::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->set,
    &this->remove,
    &this->foreach,
    &this->setByIdentifier,
    &this->setByIdentifier_sharedRepository,
    &this->setByIdentifier_ast,
    &this->removeByIdentifier,
    &this->removeByIdentifier_sharedRepository,
    &this->removeByIdentifier_ast,
    &this->foreachByIdentifier,
    &this->foreachByIdentifier_sharedRepository,
    &this->foreachByIdentifier_ast,
    &this->setByLinkOperator,
    &this->setByLinkOperator_routing,
    &this->setByLinkOperator_scopeDotIdentifier,
    &this->setByLinkOperator_mapDotIdentifier,
    &this->removeByLinkOperator,
    &this->removeByLinkOperator_routing,
    &this->removeByLinkOperator_scopeDotIdentifier,
    &this->removeByLinkOperator_mapDotIdentifier,
    &this->foreachByLinkOperator,
    &this->foreachByLinkOperator_routing,
    &this->foreachByLinkOperator_scopeDotIdentifier,
    &this->foreachByLinkOperator_mapDotIdentifier
  });
}


void Seeker::initBindings()
{
  // Main seek functions
  this->set = &Seeker::_set;
  this->remove = &Seeker::_remove;
  this->foreach = &Seeker::_foreach;

  // Identifier seek functions
  this->setByIdentifier = &Seeker::_setByIdentifier;
  this->setByIdentifier_sharedRepository = &Seeker::_setByIdentifier_sharedRepository;
  this->setByIdentifier_ast = &Seeker::_setByIdentifier_ast;
  this->removeByIdentifier = &Seeker::_removeByIdentifier;
  this->removeByIdentifier_sharedRepository = &Seeker::_removeByIdentifier_sharedRepository;
  this->removeByIdentifier_ast = &Seeker::_removeByIdentifier_ast;
  this->foreachByIdentifier = &Seeker::_foreachByIdentifier;
  this->foreachByIdentifier_sharedRepository = &Seeker::_foreachByIdentifier_sharedRepository;
  this->foreachByIdentifier_ast = &Seeker::_foreachByIdentifier_ast;

  // LinkOperator seek functions
  this->setByLinkOperator = &Seeker::_setByLinkOperator;
  this->setByLinkOperator_routing = &Seeker::_setByLinkOperator_routing;
  this->setByLinkOperator_scopeDotIdentifier = &Seeker::_setByLinkOperator_scopeDotIdentifier;
  this->setByLinkOperator_mapDotIdentifier = &Seeker::_setByLinkOperator_mapDotIdentifier;
  this->removeByLinkOperator = &Seeker::_removeByLinkOperator;
  this->removeByLinkOperator_routing = &Seeker::_removeByLinkOperator_routing;
  this->removeByLinkOperator_scopeDotIdentifier = &Seeker::_removeByLinkOperator_scopeDotIdentifier;
  this->removeByLinkOperator_mapDotIdentifier = &Seeker::_removeByLinkOperator_mapDotIdentifier;
  this->foreachByLinkOperator = &Seeker::_foreachByLinkOperator;
  this->foreachByLinkOperator_routing = &Seeker::_foreachByLinkOperator_routing;
  this->foreachByLinkOperator_scopeDotIdentifier = &Seeker::_foreachByLinkOperator_scopeDotIdentifier;
  this->foreachByLinkOperator_mapDotIdentifier = &Seeker::_foreachByLinkOperator_mapDotIdentifier;
}


//==============================================================================
// Helper Functions

Bool Seeker::trySet(TiObject const *ref, TiObject *target, TiObject *val)
{
  Bool result = false;
  this->doSet(ref, target, [=,&result](TiObject *&obj, Notice*)->SeekVerb {
    obj = val;
    result = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return result;
}


Bool Seeker::tryRemove(TiObject const *ref, TiObject *target)
{
  Bool ret = false;
  this->doRemove(ref, target, [&ret](TiObject *o, Notice*)->SeekVerb {
    ret = true;
    return SeekVerb::PERFORM_AND_MOVE;
  });
  return ret;
}


Bool Seeker::tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal)
{
  Bool ret = false;
  this->doForeach(ref, target, [&ret,&retVal](TiObject *o, Notice*)->SeekVerb {
    retVal = o;
    ret = true;
    return SeekVerb::STOP;
  });
  return ret;
}


//==============================================================================
// Main Seek Functions

void Seeker::_set(TiObject *self, TiObject const *ref, TiObject *target, SeekSetCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Ast::Identifier>()) {
    seeker->setByIdentifier(static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    seeker->setByLinkOperator(static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void Seeker::_remove(TiObject *self, TiObject const *ref, TiObject *target, SeekRemoveCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Ast::Identifier>()) {
    seeker->removeByIdentifier(static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    seeker->removeByLinkOperator(static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void Seeker::_foreach(TiObject *self, TiObject const *ref, TiObject *target, SeekForeachCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Ast::Identifier>()) {
    seeker->foreachByIdentifier(static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    seeker->foreachByLinkOperator(static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


//==============================================================================
// Identifier set

void Seeker::_setByIdentifier(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                              SeekSetCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    seeker->setByIdentifier_sharedRepository(identifier, static_cast<Data::SharedRepository*>(data), cb);
  } else {
    seeker->setByIdentifier_ast(identifier, data, cb);
  }
}


void Seeker::_setByIdentifier_sharedRepository(TiObject *self, Data::Ast::Identifier const *identifier,
                                               Data::SharedRepository *repo, SeekSetCallback cb)
{
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
          verb = cb(obj, 0);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(obj, 0);
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


void Seeker::_setByIdentifier_ast(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                  SeekSetCallback cb)
{
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
          verb = cb(obj, 0);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(obj, 0);
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

void Seeker::_removeByIdentifier(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                 SeekRemoveCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    seeker->removeByIdentifier_sharedRepository(identifier, static_cast<Data::SharedRepository*>(data), cb);
  } else {
    seeker->removeByIdentifier_ast(identifier, data, cb);
  }
}


void Seeker::_removeByIdentifier_sharedRepository(TiObject *self, Data::Ast::Identifier const *identifier,
                                                  Data::SharedRepository *repo, SeekRemoveCallback cb)
{
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
          verb = cb(obj, 0);
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


void Seeker::_removeByIdentifier_ast(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                     SeekRemoveCallback cb)
{
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
          verb = cb(obj, 0);
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

void Seeker::_foreachByIdentifier(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                  SeekForeachCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    seeker->foreachByIdentifier_sharedRepository(identifier, static_cast<Data::SharedRepository*>(data), cb
    );
  } else {
    seeker->foreachByIdentifier_ast(identifier, data, cb);
  }
}


void Seeker::_foreachByIdentifier_sharedRepository(TiObject *self, Data::Ast::Identifier const *identifier,
                                                   Data::SharedRepository *repo, SeekForeachCallback cb)
{
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
          verb = cb(obj, 0);
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
  }
}


void Seeker::_foreachByIdentifier_ast(TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data,
                                      SeekForeachCallback cb)
{
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
          verb = cb(obj, 0);
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

void Seeker::_setByLinkOperator(TiObject *self, Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  seeker->doForeach(first, data,
    [=](TiObject *newData, Notice*)->SeekVerb
    {
      return seeker->setByLinkOperator_routing(link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_setByLinkOperator_routing(TiObject *self, Ast::LinkOperator const *link, TiObject *data,
                                                    SeekSetCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return seeker->setByLinkOperator_scopeDotIdentifier(
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return seeker->setByLinkOperator_mapDotIdentifier(static_cast<Ast::Identifier*>(second), map, cb);
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


Seeker::SeekVerb Seeker::_setByLinkOperator_scopeDotIdentifier(TiObject *self, Ast::Identifier const *identifier,
                                                               Ast::Scope *scope, SeekSetCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj, 0);
      if (isPerform(verb)) {
        def->setTarget(getSharedPtr(obj));
      }
      if (!isMove(verb)) break;
    }
  }
  if (isMove(verb)) {
    TiObject *obj = 0;
    verb = cb(obj, 0);
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


Seeker::SeekVerb Seeker::_setByLinkOperator_mapDotIdentifier(TiObject *self, Ast::Identifier const *identifier,
                                                             MapContainer *map, SeekSetCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  auto obj = map->get(identifier->getValue().get());
  verb = cb(obj, 0);
  if (isPerform(verb)) {
    map->set(identifier->getValue().get(), obj);
  }
  return verb;
}


//==============================================================================
// LinkOperator remove

void Seeker::_removeByLinkOperator(TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data,
                                   SeekRemoveCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  seeker->doForeach(first, data,
    [=](TiObject *newData, Notice*)->SeekVerb
    {
      return seeker->removeByLinkOperator_routing(link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_removeByLinkOperator_routing(TiObject *self, Data::Ast::LinkOperator const *link,
                                                       TiObject *data,
                                                       SeekRemoveCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return seeker->removeByLinkOperator_scopeDotIdentifier(
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return seeker->removeByLinkOperator_mapDotIdentifier(static_cast<Ast::Identifier*>(second), map, cb);
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


Seeker::SeekVerb Seeker::_removeByLinkOperator_scopeDotIdentifier(TiObject *self,
                                                                  Data::Ast::Identifier const *identifier,
                                                                  Data::Ast::Scope *scope, SeekRemoveCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj, 0);
      if (isPerform(verb)) {
        scope->remove(i);
        --i;
      }
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


Seeker::SeekVerb Seeker::_removeByLinkOperator_mapDotIdentifier(TiObject *self,
                                                                Data::Ast::Identifier const *identifier,
                                                                Data::MapContainer *map, SeekRemoveCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  auto index = map->findIndex(identifier->getValue().get());
  if (index != -1) {
    auto obj = map->get(index);
    verb = cb(obj, 0);
    if (isPerform(verb)) {
      map->remove(index);
    }
  }
  return verb;
}


//==============================================================================
// LinkOperator foreach

void Seeker::_foreachByLinkOperator(TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data,
                                    SeekForeachCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  seeker->doForeach(first, data,
    [=](TiObject *newData, Notice*)->SeekVerb
    {
      return seeker->foreachByLinkOperator_routing(link, newData, cb);
    }
  );
}


Seeker::SeekVerb Seeker::_foreachByLinkOperator_routing(TiObject *self, Data::Ast::LinkOperator const *link,
                                                        TiObject *data, SeekForeachCallback cb)
{
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return seeker->foreachByLinkOperator_scopeDotIdentifier(
          static_cast<Ast::Identifier*>(second), static_cast<Ast::Scope*>(data), cb
        );
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return seeker->foreachByLinkOperator_mapDotIdentifier(static_cast<Ast::Identifier*>(second), map, cb);
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


Seeker::SeekVerb Seeker::_foreachByLinkOperator_scopeDotIdentifier(TiObject *self, Data::Ast::Identifier *identifier,
                                                                   Data::Ast::Scope *scope, SeekForeachCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(obj, 0);
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


Seeker::SeekVerb Seeker::_foreachByLinkOperator_mapDotIdentifier(TiObject *self,
                                                                 Data::Ast::Identifier const *identifier,
                                                                 Data::MapContainer *map, SeekForeachCallback cb)
{
  auto obj = map->get(identifier->getValue().get());
  return cb(obj, 0);
}

} } // namespace
