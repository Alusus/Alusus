/**
 * @file Core/Ast/Seeker.cpp
 * Contains the implementation of class Core::Ast::Seeker.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Ast
{

//==============================================================================
// Initialization

void Seeker::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    // Set Functions
    &this->set,
    &this->set_identifier,
    &this->set_identifierLevel,
    &this->set_identifierOnScope,
    &this->set_identifierOnMap,
    &this->set_linkOperator,
    &this->set_linkOperatorRouting,
    // Remove Functions
    &this->remove,
    &this->remove_identifier,
    &this->remove_identifierLevel,
    &this->remove_identifierOnScope,
    &this->remove_identifierOnMap,
    &this->remove_linkOperator,
    &this->remove_linkOperatorRouting,
    // Foreach Functions
    &this->foreach,
    &this->foreach_identifier,
    &this->foreach_identifierLevel,
    &this->foreach_identifierAtScope,
    &this->foreach_identifierOnScope,
    &this->foreach_identifierOnMap,
    &this->foreach_linkOperator,
    &this->foreach_linkOperatorRouting,
    // Other Functions
    &this->extForeach
  });
}


void Seeker::initBindings()
{
  // Set Functions
  this->set = &Seeker::_set;
  this->set_identifier = &Seeker::_set_identifier;
  this->set_identifierLevel = &Seeker::_set_identifierLevel;
  this->set_identifierOnScope = &Seeker::_set_identifierOnScope;
  this->set_identifierOnMap = &Seeker::_set_identifierOnMap;
  this->set_linkOperator = &Seeker::_set_linkOperator;
  this->set_linkOperatorRouting = &Seeker::_set_linkOperatorRouting;

  // Remove Functions
  this->remove = &Seeker::_remove;
  this->remove_identifier = &Seeker::_remove_identifier;
  this->remove_identifierLevel = &Seeker::_remove_identifierLevel;
  this->remove_identifierOnScope = &Seeker::_remove_identifierOnScope;
  this->remove_identifierOnMap = &Seeker::_remove_identifierOnMap;
  this->remove_linkOperator = &Seeker::_remove_linkOperator;
  this->remove_linkOperatorRouting = &Seeker::_remove_linkOperatorRouting;

  // Foreach Functions
  this->foreach = &Seeker::_foreach;
  this->foreach_identifier = &Seeker::_foreach_identifier;
  this->foreach_identifierLevel = &Seeker::_foreach_identifierLevel;
  this->foreach_identifierAtScope = &Seeker::_foreach_identifierAtScope;
  this->foreach_identifierOnScope = &Seeker::_foreach_identifierOnScope;
  this->foreach_identifierOnMap = &Seeker::_foreach_identifierOnMap;
  this->foreach_linkOperator = &Seeker::_foreach_linkOperator;
  this->foreach_linkOperatorRouting = &Seeker::_foreach_linkOperatorRouting;

  // Other Functions
  this->extForeach = &Seeker::_extForeach;
}


//==============================================================================
// Helper Functions

Bool Seeker::trySet(Node const *ref, Node *target, Node *val, Word flags)
{
  Bool result = false;
  Int tracingAlias = 0;
  Int tracingUse = 0;
  this->set(ref, target, [=,&result, &tracingAlias, &tracingUse](TiInt action, Node *&obj, Seeker::NoticePtr const &notice)->Verb {
    if (action == Action::ALIAS_TRACE_START) {
      ++tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::ALIAS_TRACE_END) {
      --tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::OWNER_SCOPE) {
      if (tracingAlias == 0 && (flags & Flags::SKIP_OWNERS) != 0) return Verb::SKIP_GROUP;
      else return Verb::MOVE;
    } else if (action == Action::USE_SCOPES_START) {
      if (
        (tracingUse > 0 || (flags & Flags::SKIP_USES) != 0) &&
        (tracingAlias == 0 || (flags & Flags::SKIP_USES_FOR_ALIASES) != 0)
      ) {
        return Verb::SKIP;
      } else {
        ++tracingUse;
        return Verb::MOVE;
      }
    } else if (action == Action::USE_SCOPES_END) {
      --tracingUse;
      return Verb::MOVE;
    } else if (action != Action::TARGET_MATCH) {
      return Verb::MOVE;
    }
    obj = val;
    result = true;
    return Verb::PERFORM_AND_MOVE;
  }, flags);
  return result;
}


Bool Seeker::tryRemove(Node const *ref, Node *target, Word flags)
{
  Bool ret = false;
  Int tracingAlias = 0;
  Int tracingUse = 0;
  this->remove(ref, target, [flags, &ret, &tracingAlias, &tracingUse](TiInt action, Node *o, Seeker::NoticePtr const &notice)->Verb {
    if (action == Action::ALIAS_TRACE_START) {
      ++tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::ALIAS_TRACE_END) {
      --tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::OWNER_SCOPE) {
      if (tracingAlias == 0 && (flags & Flags::SKIP_OWNERS) != 0) return Verb::SKIP_GROUP;
      else return Verb::MOVE;
    } else if (action == Action::USE_SCOPES_START) {
      if (
        (tracingUse > 0 || (flags & Flags::SKIP_USES) != 0) &&
        (tracingAlias == 0 || (flags & Flags::SKIP_USES_FOR_ALIASES) != 0)
      ) {
        return Verb::SKIP;
      } else {
        ++tracingUse;
        return Verb::MOVE;
      }
    } else if (action == Action::USE_SCOPES_END) {
      --tracingUse;
      return Verb::MOVE;
    } else if (action != Action::TARGET_MATCH) {
      return Verb::MOVE;
    }
    ret = true;
    return Verb::PERFORM_AND_MOVE;
  }, flags);
  return ret;
}


Bool Seeker::tryGet(Node const *ref, Node *target, Node *&retVal, Word flags)
{
  Bool ret = false;
  this->extForeach(ref, target, [&ret, &retVal](TiInt action, Node *o, Seeker::NoticePtr const &notice)->Verb {
    if (action != Seeker::Action::TARGET_MATCH) return Seeker::Verb::MOVE;
    retVal = o;
    ret = true;
    return Verb::STOP;
  }, flags);
  return ret;
}


Bool Seeker::find(Node const *ref, Node *target, TypeInfo const *ti, Node *&retVal, Word flags)
{
  Bool ret = false;
  this->extForeach(ref, target, [ti, &ret, &retVal](TiInt action, Node *o, Seeker::NoticePtr const &notice)->Verb {
    if (action != Seeker::Action::TARGET_MATCH) return Seeker::Verb::MOVE;
    if (o->isDerivedFrom<Passage>()) {
      o = static_cast<Passage*>(o)->get();
    }
    if (o->isDerivedFrom(ti)) {
      retVal = o;
      ret = true;
      return Verb::STOP;
    } else {
      return Verb::MOVE;
    }
  }, flags);
  return ret;
}


//==============================================================================
// Set Functions

Seeker::Verb Seeker::_set(TiObject *self, Node const *ref, Node *target, SetCallback const &cb, Word flags)
{
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Identifier>()) {
    return seeker->set_identifier(static_cast<Identifier const*>(ref), target, cb, flags);
  } else if (ref->isA<LinkOperator>()) {
    return seeker->set_linkOperator(static_cast<LinkOperator const*>(ref), target, cb, flags);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("ref"), S("Unrecognized reference type."));
  }
}


Seeker::Verb Seeker::_set_identifier(
  TiObject *self, Identifier const *identifier, Node *data, SetCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  Seeker::Verb retVal = Seeker::Verb::MOVE;
  if (data->isDerivedFrom<NodeStack>()) {
    auto stack = static_cast<NodeStack*>(data);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      auto data = stack->getElement(i);
      if (data == 0) continue;
      if (i != stack->getCount() - 1) {
        retVal = cb(Action::OWNER_SCOPE, data, Seeker::NoticePtr::null);
        if (retVal == Verb::SKIP) continue;
        else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
      }
      retVal = seeker->set_identifierLevel(identifier, data, cb, flags);
      if (!Seeker::isMove(retVal)) return retVal;
    }
  } else {
    auto node = data;
    while (node != 0) {
      if (node != data) {
        Node *obj = node;
        retVal = cb(Action::OWNER_SCOPE, obj, Seeker::NoticePtr::null);
        if (retVal == Verb::SKIP) {
          node = node->getOwner();
          continue;
        } else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
      }
      retVal = seeker->set_identifierLevel(identifier, node, cb, flags);
      if (!Seeker::isMove(retVal)) return retVal;
      node = node->getOwner();
      flags |= Seeker::Flags::SKIP_OWNED;
    }
  }
  return retVal;
}


Seeker::Verb Seeker::_set_identifierLevel(
  TiObject *self, Identifier const *identifier, Node *data, SetCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Scope>()) {
    return seeker->set_identifierOnScope(identifier, static_cast<Scope*>(data), cb, flags);
  } else {
    return Seeker::Verb::MOVE;
  }
}


Seeker::Verb Seeker::_set_identifierOnScope(
  TiObject *self, Identifier const *identifier, Scope *scope, SetCallback const &cb, Word flags
) {
  Seeker::Verb verb = Seeker::Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Definition>(scope->getElement(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
      if (isPerform(verb)) {
        def->setTarget(getSharedPtr(obj));
      }
      if (!Seeker::isMove(verb)) break;
    }
  }
  if (Seeker::isMove(verb)) {
    Node *obj = 0;
    verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
    if (isPerform(verb)) {
      // Add a new definition.
      auto def = Definition::create();
      def->setName(identifier->getValue());
      def->setTarget(getSharedPtr(obj));
      scope->add(def);
    }
  }
  return verb;
}


Seeker::Verb Seeker::_set_identifierOnMap(
  TiObject *self, Identifier const *identifier, MapContaining<Node> *map, SetCallback const &cb, Word flags
) {
  Verb verb = Verb::MOVE;
  auto obj = map->getElement(identifier->getValue().get());
  verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
  if (isPerform(verb)) {
    map->setElement(identifier->getValue().get(), obj);
  }
  return verb;
}


Seeker::Verb Seeker::_set_linkOperator(
  TiObject *self, LinkOperator const *link, Node *data, SetCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  return seeker->foreach(first, data,
    [=](TiInt action, Node *newData, Seeker::NoticePtr const &notice)->Verb
    {
      if (action == Action::TARGET_MATCH) return seeker->set_linkOperatorRouting(link, newData, cb, flags);
      else return cb(action, newData, notice);
    },
    flags
  );
}


Seeker::Verb Seeker::_set_linkOperatorRouting(
  TiObject *self, LinkOperator const *link, Node *data, SetCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == S(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Identifier>()) {
      if (data->isDerivedFrom<Scope>()) {
        return seeker->set_identifierOnScope(
          static_cast<Identifier*>(second), static_cast<Scope*>(data), cb, flags
        );
      } else {
        auto map = ti_cast<MapContaining<Node>>(data);
        if (map != 0) {
          return seeker->set_identifierOnMap(static_cast<Identifier*>(second), map, cb, flags);
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, S("link"), S("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, S("link"), S("Unknown link operator type."), link->getType());
  }
  return Seeker::Verb::MOVE;
}


//==============================================================================
// Remove Functions

Seeker::Verb Seeker::_remove(
  TiObject *self, Node const *ref, Node *target, RemoveCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Identifier>()) {
    return seeker->remove_identifier(static_cast<Identifier const*>(ref), target, cb, flags);
  } else if (ref->isA<LinkOperator>()) {
    return seeker->remove_linkOperator(static_cast<LinkOperator const*>(ref), target, cb, flags);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("ref"), S("Unrecognized reference type."));
  }
}


Seeker::Verb Seeker::_remove_identifier(
  TiObject *self, Identifier const *identifier, Node *data, RemoveCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  Seeker::Verb retVal = Seeker::Verb::MOVE;
  if (data->isDerivedFrom<NodeStack>()) {
    auto stack = static_cast<NodeStack*>(data);
    for (Int i = stack->getCount() - 1; i >= 0; --i) {
      auto data = stack->getElement(i);
      if (data == 0) continue;
      if (i != stack->getCount() - 1) {
        retVal = cb(Action::OWNER_SCOPE, data, Seeker::NoticePtr::null);
        if (retVal == Verb::SKIP) continue;
        else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
      }
      retVal = seeker->remove_identifierLevel(identifier, data, cb, flags);
      if (!Seeker::isMove(retVal)) return retVal;
    }
  } else {
    auto node = data;
    while (node != 0) {
      if (node != data) {
        retVal = cb(Action::OWNER_SCOPE, node, Seeker::NoticePtr::null);
        if (retVal == Verb::SKIP) {
          node = node->getOwner();
          continue;
        } else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
      }
      retVal = seeker->remove_identifierLevel(identifier, node, cb, flags);
      if (!Seeker::isMove(retVal)) return retVal;
      node = node->getOwner();
      flags |= Seeker::Flags::SKIP_OWNED;
    }
  }
  return retVal;
}


Seeker::Verb Seeker::_remove_identifierLevel(
  TiObject *self, Identifier const *identifier, Node *data, RemoveCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Scope>()) {
    return seeker->remove_identifierOnScope(identifier, static_cast<Scope*>(data), cb, flags);
  } else {
    return Seeker::Verb::MOVE;
  }
}


Seeker::Verb Seeker::_remove_identifierOnScope(
  TiObject *self, Identifier const *identifier, Scope *scope, RemoveCallback const &cb, Word flags
) {
  Seeker::Verb verb = Seeker::Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Definition>(scope->getElement(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
      if (isPerform(verb)) {
        scope->remove(i);
        --i;
      }
      if (!Seeker::isMove(verb)) return verb;
    }
  }
  return verb;
}


Seeker::Verb Seeker::_remove_identifierOnMap(
  TiObject *self, Identifier const *identifier, DynamicMapContaining<Node> *map,
  RemoveCallback const &cb, Word flags
) {
  Verb verb = Verb::MOVE;
  auto index = map->findElementIndex(identifier->getValue().get());
  if (index != -1) {
    auto obj = map->getElement(index);
    verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
    if (isPerform(verb)) {
      map->removeElement(index);
    }
  }
  return verb;
}


Seeker::Verb Seeker::_remove_linkOperator(
  TiObject *self, LinkOperator const *link, Node *data, RemoveCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  return seeker->foreach(first, data,
    [=](TiInt action, Node *newData, Seeker::NoticePtr const &notice)->Verb
    {
      if (action == Action::TARGET_MATCH) return seeker->remove_linkOperatorRouting(link, newData, cb, flags);
      else return cb(action, newData, notice);
    },
    flags
  );
}


Seeker::Verb Seeker::_remove_linkOperatorRouting(
  TiObject *self, LinkOperator const *link, Node *data, RemoveCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == S(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Identifier>()) {
      if (data->isDerivedFrom<Scope>()) {
        return seeker->remove_identifierOnScope(
          static_cast<Identifier*>(second), static_cast<Scope*>(data), cb, flags
        );
      } else {
        auto map = ti_cast<DynamicMapContaining<Node>>(data);
        if (map != 0) {
          return seeker->remove_identifierOnMap(static_cast<Identifier*>(second), map, cb, flags);
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, S("link"), S("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, S("link"), S("Unknown link operator type."), link->getType());
  }
  return Seeker::Verb::MOVE;
}


//==============================================================================
// Foreach Functions

Seeker::Verb Seeker::_foreach(
  TiObject *self, Node const *ref, Node *target, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (ref->isA<Identifier>()) {
    return seeker->foreach_identifier(static_cast<Identifier const*>(ref), target, cb, flags);
  } else if (ref->isA<LinkOperator>()) {
    return seeker->foreach_linkOperator(static_cast<LinkOperator const*>(ref), target, cb, flags);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("ref"), S("Unrecognized reference type."));
  }
}


Seeker::Verb Seeker::_foreach_identifier(
  TiObject *self, Identifier const *identifier, Node *data, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  Seeker::Verb retVal = Seeker::Verb::MOVE;
  if (identifier->getValue() == S("Root")) {
    if (data->isDerivedFrom<NodeStack>()) {
      auto stack = static_cast<NodeStack*>(data);
      for (Int i = 0; i < stack->getCount(); ++i) {
        auto element = stack->getElement(i);
        if (element != 0) {
          return cb(Action::TARGET_MATCH, element, Seeker::NoticePtr::null);
        }
      }
    } else {
      auto node = data;
      while (node->getOwner() != 0) node = node->getOwner();
      return cb(Action::TARGET_MATCH, node, Seeker::NoticePtr::null);
    }
  } else {
    if (data->isDerivedFrom<NodeStack>()) {
      auto stack = static_cast<NodeStack*>(data);
      for (Int i = stack->getCount() - 1; i >= 0; --i) {
        auto data = stack->getElement(i);
        if (data == 0) continue;
        if (i != stack->getCount() - 1) {
          retVal = cb(Action::OWNER_SCOPE, data, Seeker::NoticePtr::null);
          if (retVal == Verb::SKIP) continue;
          else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
        }
        retVal = seeker->foreach_identifierLevel(identifier, data, cb, flags);
        if (!Seeker::isMove(retVal)) return retVal;
      }
    } else {
      auto node = data;
      while (node != 0) {
        if (node != data) {
          retVal = cb(Action::OWNER_SCOPE, node, Seeker::NoticePtr::null);
          if (retVal == Verb::SKIP) {
            node = node->getOwner();
            continue;
          } else if (retVal == Verb::SKIP_GROUP || !Seeker::isMove(retVal)) break;
        }
        retVal = seeker->foreach_identifierLevel(identifier, node, cb, flags);
        if (!Seeker::isMove(retVal)) return retVal;
        node = node->getOwner();
        flags |= Seeker::Flags::SKIP_OWNED;
      }
    }
  }
  return retVal;
}


Seeker::Verb Seeker::_foreach_identifierLevel(
  TiObject *self, Identifier const *identifier, Node *data, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (data->isDerivedFrom<Scope>()) {
    return seeker->foreach_identifierAtScope(identifier, static_cast<Scope*>(data), cb, flags);
  } else {
    return Seeker::Verb::MOVE;
  }
}


Seeker::Verb Seeker::_foreach_identifierAtScope(
  TiObject *self, Identifier const *identifier, Scope *scope, ForeachCallback const &cb, Word flags
) {
  Seeker::Verb verb = Seeker::Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Definition>(scope->getElement(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Alias>()) {
        verb = cb(Action::ALIAS_TRACE_START, obj, Seeker::NoticePtr::null);
        if (verb == Verb::SKIP) return Verb::MOVE;
        else if (!Seeker::isMove(verb)) return verb;
        PREPARE_SELF(seeker, Seeker);
        auto alias = static_cast<Alias*>(obj);
        verb = seeker->foreach(
          alias->getReference().get(), alias->getOwner(), cb, flags & ~Flags::SKIP_OWNED
        );
        if (!Seeker::isMove(verb)) return verb;
        verb = cb(Action::ALIAS_TRACE_END, obj, Seeker::NoticePtr::null);
        if (verb != Verb::MOVE) return verb;
      } else {
        verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
        if (!Seeker::isMove(verb)) return verb;
      }
    }
  }

  PREPARE_SELF(seeker, Seeker);

  if (scope->getBridgeCount() > 0) {
    verb = cb(Action::USE_SCOPES_START, scope, Seeker::NoticePtr::null);
    if (verb == Verb::SKIP) return verb;
    for (Int i = 0; i < scope->getBridgeCount(); ++i) {
      auto bridgeRef = scope->getBridge(i);
      // If the thing we are looking for is actually this bridgeRef, then we need to skip, otherwise we end up in
      // an infinite loop.
      if (bridgeRef->getTarget() == identifier) continue;
      auto bridgeTarget = seeker->tryGet(
        bridgeRef->getTarget().get(), scope, Seeker::Flags::SKIP_USES | Seeker::Flags::SKIP_USES_FOR_ALIASES
      );
      if (bridgeTarget == 0) continue;
      verb = cb(Action::USE_SCOPE, bridgeTarget, Seeker::NoticePtr::null);
      if (verb == Verb::SKIP) continue;
      else if (verb == Verb::SKIP_GROUP || !Seeker::isMove(verb)) break;

      verb = seeker->foreach_identifierLevel(identifier, bridgeTarget, cb, flags);
      if (!Seeker::isMove(verb)) return verb;
    }
    verb = cb(Action::USE_SCOPES_END, scope, Seeker::NoticePtr::null);
  }

  return verb;
}


Seeker::Verb Seeker::_foreach_identifierOnScope(
  TiObject *self, Identifier *identifier, Scope *scope, ForeachCallback const &cb, Word flags
) {
  Verb verb = Verb::MOVE;
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto def = ti_cast<Definition>(scope->getElement(i));
    if (def != 0 && def->getName() == identifier->getValue()) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Alias>()) {
        verb = cb(Action::ALIAS_TRACE_START, obj, Seeker::NoticePtr::null);
        if (verb == Verb::SKIP) return Verb::MOVE;
        else if (!Seeker::isMove(verb)) return verb;
        PREPARE_SELF(seeker, Seeker);
        auto alias = static_cast<Alias*>(obj);
        verb = seeker->foreach(
          alias->getReference().get(), alias->getOwner(), cb, flags & ~Flags::SKIP_OWNED
        );
        if (!Seeker::isMove(verb)) break;
        verb = cb(Action::ALIAS_TRACE_END, obj, Seeker::NoticePtr::null);
        if (verb != Verb::MOVE) return verb;
      } else {
        verb = cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
        if (!Seeker::isMove(verb)) break;
      }
    }
  }
  return verb;
}


Seeker::Verb Seeker::_foreach_identifierOnMap(
  TiObject *self, Identifier const *identifier, MapContaining<Node> *map, ForeachCallback const &cb,
  Word flags
) {
  auto index = map->findElementIndex(identifier->getValue().get());
  if (index == -1) return Verb::MOVE;
  auto obj = map->getElement(index);
  if (obj->isDerivedFrom<Alias>()) {
    auto verb = cb(Action::ALIAS_TRACE_START, obj, Seeker::NoticePtr::null);
    if (verb == Verb::SKIP) return Verb::MOVE;
    else if (!Seeker::isMove(verb)) return verb;
    PREPARE_SELF(seeker, Seeker);
    auto alias = static_cast<Alias*>(obj);
    verb = seeker->foreach(
      alias->getReference().get(), alias->getOwner(), cb, flags & ~Flags::SKIP_OWNED
    );
    if (verb != Verb::MOVE) return verb;
    return cb(Action::ALIAS_TRACE_END, obj, Seeker::NoticePtr::null);
  } else {
    return cb(Action::TARGET_MATCH, obj, Seeker::NoticePtr::null);
  }
}


Seeker::Verb Seeker::_foreach_linkOperator(
  TiObject *self, LinkOperator const *link, Node *data, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  auto first = link->getFirst().get();
  return seeker->foreach(first, data,
    [=](TiInt action, Node *newData, Seeker::NoticePtr const &notice)->Verb
    {
      if (action == Action::TARGET_MATCH) return seeker->foreach_linkOperatorRouting(link, newData, cb, flags);
      else return cb(action, newData, notice);
    },
    flags
  );
}


Seeker::Verb Seeker::_foreach_linkOperatorRouting(
  TiObject *self, LinkOperator const *link, Node *data, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  if (link->getType() == S(".")) {
    auto second = link->getSecond().get();
    if (second->isA<Identifier>()) {
      if (data->isDerivedFrom<Scope>()) {
        return seeker->foreach_identifierOnScope(
          static_cast<Identifier*>(second), static_cast<Scope*>(data), cb, flags
        );
      } else {
        auto map = ti_cast<MapContaining<Node>>(data);
        if (map != 0) {
          return seeker->foreach_identifierOnMap(static_cast<Identifier*>(second), map, cb, flags);
        }
      }
    } else {
      throw EXCEPTION(InvalidArgumentException, S("link"), S("Unrecognized type for link operator's second part."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, S("link"), S("Unknown link operator type."), link->getType());
  }
  return Seeker::Verb::MOVE;
}


//==============================================================================
// Other Functions

Seeker::Verb Seeker::_extForeach(
  TiObject *self, Node const *ref, Node *target, ForeachCallback const &cb, Word flags
) {
  PREPARE_SELF(seeker, Seeker);
  Int tracingAlias = 0;
  Int tracingUse = 0;
  return seeker->foreach(ref, target, [flags, cb, &tracingAlias, &tracingUse](TiInt action, Node *o, Seeker::NoticePtr const &notice)->Verb {
    if (action == Action::ALIAS_TRACE_START) {
      ++tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::ALIAS_TRACE_END) {
      --tracingAlias;
      return Verb::MOVE;
    } else if (action == Action::OWNER_SCOPE) {
      if (tracingAlias == 0 && (flags & Flags::SKIP_OWNERS) != 0) return Verb::SKIP_GROUP;
      else return Verb::MOVE;
    } else if (action == Action::USE_SCOPES_START) {
      if (
        (tracingUse > 0 || (flags & Flags::SKIP_USES) != 0) &&
        (tracingAlias == 0 || (flags & Flags::SKIP_USES_FOR_ALIASES) != 0)
      ) {
        return Verb::SKIP;
      } else {
        ++tracingUse;
        return Verb::MOVE;
      }
    } else if (action == Action::USE_SCOPES_END) {
      --tracingUse;
      return Verb::MOVE;
    } else if (action != Action::TARGET_MATCH && action != Action::ERROR) {
      return Verb::MOVE;
    }
    return cb(action, o, notice);
  }, flags);
}

} // namespace
