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

//==============================================================================
// Initialization Functions

void StandardSeeker::initialize()
{
  this->setRtMemberFunction("_set", &StandardSeeker::_set);
  this->setRtMemberFunction("_remove", &StandardSeeker::_remove);
  this->setRtMemberFunction("_foreach", &StandardSeeker::_foreach);

  this->setRtMemberFunction("setByIdentifier", &StandardSeeker::setByIdentifier);
  this->setRtMemberFunction("removeByIdentifier", &StandardSeeker::removeByIdentifier);
  this->setRtMemberFunction("foreachByIdentifier", &StandardSeeker::foreachByIdentifier);

  this->setRtMemberFunction("setByLinkOperator", &StandardSeeker::setByLinkOperator);
  this->setRtMemberFunction("setByLinkOperator_routing", &StandardSeeker::setByLinkOperator_routing);
  this->setRtMemberFunction("setByLinkOperator_scopeDotIdentifier",
                            &StandardSeeker::setByLinkOperator_scopeDotIdentifier);
  this->setRtMemberFunction("setByLinkOperator_mapDotIdentifier", &StandardSeeker::setByLinkOperator_mapDotIdentifier);

  // TODO:
}


//==============================================================================
// Main Seek Functions

void StandardSeeker::_set(TiObject const *ref, TiObject *target, SeekSetCallback cb)
{
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(this, "setByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(this, "setByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(GenericException, STR("Unrecognized reference type."));
  }
}


void StandardSeeker::_remove(TiObject const *ref, TiObject *target, SeekRemoveCallback cb)
{
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(this, "removeByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(this, "removeByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(GenericException, STR("Unrecognized reference type."));
  }
}


void StandardSeeker::_foreach(TiObject const *ref, TiObject *target, SeekForeachCallback cb)
{
  if (ref->isA<Ast::Identifier>()) {
    callVoidRtMethod(this, "foreachByIdentifier", static_cast<Ast::Identifier const*>(ref), target, cb);
  } else if (ref->isA<Ast::LinkOperator>()) {
    callVoidRtMethod(this, "foreachByLinkOperator", static_cast<Ast::LinkOperator const*>(ref), target, cb);
  } else {
    throw EXCEPTION(GenericException, STR("Unrecognized reference type."));
  }
}


//==============================================================================
// Identifier

void StandardSeeker::setByIdentifier(Ast::Identifier const *identifier, TiObject *data, SeekSetCallback cb)
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


void StandardSeeker::removeByIdentifier(Ast::Identifier const *identifier, TiObject *data, SeekRemoveCallback cb)
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


void StandardSeeker::foreachByIdentifier(Ast::Identifier const *identifier, TiObject *data, SeekForeachCallback cb)
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
// LinkOperator Set Operation

void StandardSeeker::setByLinkOperator(Ast::LinkOperator const *link, TiObject *data, SeekSetCallback cb)
{
  auto first = link->getFirst().get();
  this->foreach(first, data, [=](Seeker *newSeeker, TiObject *newData)->SeekVerb {
    if (newSeeker == 0) newSeeker = this;
    return callRtMethod<SeekVerb>(newSeeker, "setByLinkOperator_routing", link, newData, cb);
  });
}


StandardSeeker::SeekVerb StandardSeeker::setByLinkOperator_routing(Ast::LinkOperator const *link, TiObject *data,
                                                                   SeekSetCallback cb)
{
  if (link->getType() == STR(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Ast::Identifier>()) {
      if (data->isA<Ast::Scope>()) {
        return callRtMethod<SeekVerb>(this, "setByLinkOperator_scopeDotIdentifier",
                                      static_cast<Ast::Identifier*>(second),
                                      static_cast<Ast::Scope*>(data), cb);
      } else {
        MapContainer *map = ti_cast<MapContainer>(data);
        if (map != 0) {
          return callRtMethod<SeekVerb>(this, "setByLinkOperator_mapDotIdentifier",
                                        static_cast<Ast::Identifier*>(second), map, cb);
        } else {
          throw EXCEPTION(GenericException, STR("Unrecognized target data type."));
        }
      }
    } else {
      throw EXCEPTION(GenericException, STR("Unrecognized type for link operator's second part."));
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
  if (map == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("map"), STR("Should not be null."));
  }
  SeekVerb verb = SeekVerb::MOVE;
  auto obj = map->get(identifier->getValue().get());
  verb = cb(this, obj);
  if (isPerform(verb)) {
    map->set(identifier->getValue().get(), obj);
  }
  return verb;
}


//==============================================================================
// LinkOperator Remove Operation

// TODO:

//==============================================================================
// LinkOperator Foreach Operation

} } // namespace
