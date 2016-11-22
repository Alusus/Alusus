/**
 * @file Core/Standard/StandardSeeker.cpp
 * Contains the implementation of class Core::Standard::StandardSeeker.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

using namespace Data;
using SeekVerb = Data::Seeker::SeekVerb;

//==============================================================================
// Initialization Functions

void StandardSeeker::initialize()
{
  // Main seek functions
  this->setRtFunction("_set", &StandardSeeker::_set);
  this->setRtFunction("_remove", &StandardSeeker::_remove);
  this->setRtFunction("_foreach", &StandardSeeker::_foreach);

  // Identifier seek functions
  this->setRtMemberFunction("setByIdentifier", &StandardSeeker::setByIdentifier);
  this->setRtMemberFunction("setByIdentifier_SharedRepository", &StandardSeeker::setByIdentifier_SharedRepository);
  this->setRtMemberFunction("setByIdentifier_Ast", &StandardSeeker::setByIdentifier_Ast);
  this->setRtMemberFunction("removeByIdentifier", &StandardSeeker::removeByIdentifier);
  this->setRtMemberFunction("removeByIdentifier_SharedRepository", &StandardSeeker::removeByIdentifier_SharedRepository);
  this->setRtMemberFunction("removeByIdentifier_Ast", &StandardSeeker::removeByIdentifier_Ast);
  this->setRtMemberFunction("foreachByIdentifier", &StandardSeeker::foreachByIdentifier);
  this->setRtMemberFunction("foreachByIdentifier_SharedRepository",
                            &StandardSeeker::foreachByIdentifier_SharedRepository);
  this->setRtMemberFunction("foreachByIdentifier_Ast", &StandardSeeker::foreachByIdentifier_Ast);

  // LinkOperator seek functions
  this->setRtMemberFunction("setByLinkOperator", &StandardSeeker::setByLinkOperator);
  this->setRtMemberFunction("setByLinkOperator_routing", &StandardSeeker::setByLinkOperator_routing);
  this->setRtMemberFunction("setByLinkOperator_scopeDotIdentifier",
                            &StandardSeeker::setByLinkOperator_scopeDotIdentifier);
  this->setRtMemberFunction("setByLinkOperator_mapDotIdentifier", &StandardSeeker::setByLinkOperator_mapDotIdentifier);
  this->setRtMemberFunction("removeByLinkOperator", &StandardSeeker::removeByLinkOperator);
  this->setRtMemberFunction("removeByLinkOperator_routing", &StandardSeeker::removeByLinkOperator_routing);
  this->setRtMemberFunction("removeByLinkOperator_scopeDotIdentifier",
                            &StandardSeeker::removeByLinkOperator_scopeDotIdentifier);
  this->setRtMemberFunction("removeByLinkOperator_mapDotIdentifier",
                            &StandardSeeker::removeByLinkOperator_mapDotIdentifier);

  this->setRtMemberFunction("foreachByLinkOperator", &StandardSeeker::foreachByLinkOperator);
  this->setRtMemberFunction("foreachByLinkOperator_routing", &StandardSeeker::foreachByLinkOperator_routing);
  this->setRtMemberFunction("foreachByLinkOperator_scopeDotIdentifier",
                            &StandardSeeker::foreachByLinkOperator_scopeDotIdentifier);
  this->setRtMemberFunction("foreachByLinkOperator_mapDotIdentifier",
                            &StandardSeeker::foreachByLinkOperator_mapDotIdentifier);
}


//==============================================================================
// Main Seek Functions

void StandardSeeker::_set(Seeker *self, TiObject const *ref, TiObject *target, SeekSetCallback cb)
{
  auto standardSelf = static_cast<StandardSeeker*>(self);
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(standardSelf, "setByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(standardSelf, "setByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void StandardSeeker::_remove(Seeker *self, TiObject const *ref, TiObject *target, SeekRemoveCallback cb)
{
  auto standardSelf = static_cast<StandardSeeker*>(self);
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(standardSelf, "removeByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(standardSelf, "removeByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


void StandardSeeker::_foreach(Seeker *self, TiObject const *ref, TiObject *target, SeekForeachCallback cb)
{
  auto standardSelf = static_cast<StandardSeeker*>(self);
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(standardSelf, "foreachByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(standardSelf, "foreachByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Unrecognized reference type."));
  }
}


//==============================================================================
// Identifier set

void StandardSeeker::setByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb)
{
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    callVoidRtMethod(this, "setByIdentifier_SharedRepository", identifier, static_cast<Data::SharedRepository*>(data),
                     cb);
  } else {
    callVoidRtMethod(this, "setByIdentifier_Ast", identifier, data, cb);
  }
}


void StandardSeeker::setByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
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
          verb = cb(this, obj);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(this, obj);
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


void StandardSeeker::setByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb)
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
          verb = cb(this, obj);
          if (isPerform(verb)) {
            def->setTarget(getSharedPtr(obj));
          }
          if (!isMove(verb)) break;
        }
      }
      if (isMove(verb)) {
        TiObject *obj = 0;
        verb = cb(this, obj);
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

void StandardSeeker::removeByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback cb)
{
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    callVoidRtMethod(this, "removeByIdentifier_SharedRepository", identifier,
                     static_cast<Data::SharedRepository*>(data), cb);
  } else {
    callVoidRtMethod(this, "removeByIdentifier_Ast", identifier, data, cb);
  }
}


void StandardSeeker::removeByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
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
          verb = cb(this, obj);
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


void StandardSeeker::removeByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data,
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
          verb = cb(this, obj);
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

void StandardSeeker::foreachByIdentifier(Data::Ast::Identifier const *identifier, TiObject *data, SeekForeachCallback cb)
{
  if (data->isDerivedFrom<Data::SharedRepository>()) {
    callVoidRtMethod(this, "foreachByIdentifier_SharedRepository", identifier,
                     static_cast<Data::SharedRepository*>(data), cb);
  } else {
    callVoidRtMethod(this, "foreachByIdentifier_Ast", identifier, data, cb);
  }
}


void StandardSeeker::foreachByIdentifier_SharedRepository(Data::Ast::Identifier const *identifier,
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
          verb = cb(this, obj);
          if (!isMove(verb)) break;
        }
      }
      if (!isMove(verb)) break;
    }
  }
}


void StandardSeeker::foreachByIdentifier_Ast(Data::Ast::Identifier const *identifier, TiObject *data,
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
          verb = cb(this, obj);
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

void StandardSeeker::setByLinkOperator(Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb)
{
  auto first = link->getFirst().get();
  this->foreach(first, data,
    [=](Seeker *newSeeker, TiObject *newData)->SeekVerb
    {
      if (newSeeker == 0) newSeeker = this;
      return callRtMethod<SeekVerb>(static_cast<StandardSeeker*>(newSeeker), "setByLinkOperator_routing",
                                    link, newData, cb);
    }
  );
}


StandardSeeker::SeekVerb StandardSeeker::setByLinkOperator_routing(Ast::LinkOperator const *link, TiObject *data,
                                                                   SeekSetCallback cb)
{
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return callRtMethod<SeekVerb>(this, "setByLinkOperator_scopeDotIdentifier",
                                      static_cast<Ast::Identifier*>(second),
                                      static_cast<Ast::Scope*>(data), cb);
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return callRtMethod<SeekVerb>(this, "setByLinkOperator_mapDotIdentifier",
                                        static_cast<Ast::Identifier*>(second), map, cb);
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


StandardSeeker::SeekVerb StandardSeeker::setByLinkOperator_scopeDotIdentifier(Ast::Identifier const *identifier,
                                                                              Ast::Scope *scope, SeekSetCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(this, obj);
      if (isPerform(verb)) {
        def->setTarget(getSharedPtr(obj));
      }
      if (!isMove(verb)) break;
    }
  }
  if (isMove(verb)) {
    TiObject *obj = 0;
    verb = cb(this, obj);
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


StandardSeeker::SeekVerb StandardSeeker::setByLinkOperator_mapDotIdentifier(Ast::Identifier const *identifier,
                                                                            MapContainer *map, SeekSetCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  auto obj = map->get(identifier->getValue().get());
  verb = cb(this, obj);
  if (isPerform(verb)) {
    map->set(identifier->getValue().get(), obj);
  }
  return verb;
}


//==============================================================================
// LinkOperator remove

void StandardSeeker::removeByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekRemoveCallback cb)
{
  auto first = link->getFirst().get();
  this->foreach(first, data,
    [=](Seeker *newSeeker, TiObject *newData)->SeekVerb
    {
      if (newSeeker == 0) newSeeker = this;
      return callRtMethod<SeekVerb>(static_cast<StandardSeeker*>(newSeeker), "removeByLinkOperator_routing",
                                    link, newData, cb);
    }
  );
}


SeekVerb StandardSeeker::removeByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data,
                                                      SeekRemoveCallback cb)
{
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return callRtMethod<SeekVerb>(this, "removeByLinkOperator_scopeDotIdentifier",
                                      static_cast<Ast::Identifier*>(second),
                                      static_cast<Ast::Scope*>(data), cb);
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return callRtMethod<SeekVerb>(this, "removeByLinkOperator_mapDotIdentifier",
                                        static_cast<Ast::Identifier*>(second), map, cb);
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


SeekVerb StandardSeeker::removeByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier const *identifier,
                                                                 Data::Ast::Scope *scope, SeekRemoveCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(this, obj);
      if (isPerform(verb)) {
        scope->remove(i);
        --i;
      }
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


SeekVerb StandardSeeker::removeByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier,
                                                               Data::MapContainer *map, SeekRemoveCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  auto index = map->findIndex(identifier->getValue().get());
  if (index != -1) {
    auto obj = map->get(index);
    verb = cb(this, obj);
    if (isPerform(verb)) {
      map->remove(index);
    }
  }
  return verb;
}


//==============================================================================
// LinkOperator foreach

void StandardSeeker::foreachByLinkOperator(Data::Ast::LinkOperator const *link, TiObject *data, SeekForeachCallback cb)
{
  auto first = link->getFirst().get();
  this->foreach(first, data,
    [=](Seeker *newSeeker, TiObject *newData)->SeekVerb
    {
      if (newSeeker == 0) newSeeker = this;
      return callRtMethod<SeekVerb>(static_cast<StandardSeeker*>(newSeeker), "foreachByLinkOperator_routing", link,
                                    newData, cb);
    }
  );
}


SeekVerb StandardSeeker::foreachByLinkOperator_routing(Data::Ast::LinkOperator const *link, TiObject *data,
                                                       SeekForeachCallback cb)
{
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isDerivedFrom<Ast::Scope>()) {
        return callRtMethod<SeekVerb>(this, "foreachByLinkOperator_scopeDotIdentifier",
                                      static_cast<Ast::Identifier*>(second),
                                      static_cast<Ast::Scope*>(data), cb);
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return callRtMethod<SeekVerb>(this, "foreachByLinkOperator_mapDotIdentifier",
                                        static_cast<Ast::Identifier*>(second), map, cb);
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


SeekVerb StandardSeeker::foreachByLinkOperator_scopeDotIdentifier(Data::Ast::Identifier *identifier,
                                                                  Data::Ast::Scope *scope, SeekForeachCallback cb)
{
  SeekVerb verb = SeekVerb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(scope->get(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(this, obj);
      if (!isMove(verb)) break;
    }
  }
  return verb;
}


SeekVerb StandardSeeker::foreachByLinkOperator_mapDotIdentifier(Data::Ast::Identifier const *identifier,
                                                                Data::MapContainer *map, SeekForeachCallback cb)
{
  auto obj = map->get(identifier->getValue().get());
  return cb(this, obj);
}

} } // namespace
