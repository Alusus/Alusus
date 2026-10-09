/**
 * @file Core/Ast/Node.cpp
 * Contains the implementation of class Core::Ast::Node.
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
// Destructor

Node::~Node()
{
  DISOWN_SHAREDPTR(this->modifiers);
  DISOWN_SHAREDPTR(this->metadata);
}


//==============================================================================
// Binding Implementation

IMPLEMENT_BINDING_STANDALONE(Node, Binding,
  (prodId, TiWord, VALUE, setProdId(value), &prodId),
  (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get()),
  (modifiers, List, SHARED_REF, setModifiers(value), modifiers.get()),
  (metadata, Map, SHARED_REF, setMetadata(value), metadata.get())
);


//==============================================================================
// Modifier Functions

void Node::setModifiers(SharedPtr<List> const &m)
{
  UPDATE_OWNED_SHAREDPTR(this->modifiers, m);
}


void Node::setModifiers(List *m)
{
  this->setModifiers(getSharedPtr(m));
}


void Node::addModifier(SharedPtr<Node> const &modifier)
{
  if (this->modifiers == 0) {
    this->setModifiers(List::create({}, { modifier }));
  } else {
    this->modifiers->add(modifier);
  }
}


void Node::addModifiers(SharedPtr<List> const &m)
{
  if (m == 0) return;
  if (this->modifiers == 0) {
    this->setModifiers(m);
    return;
  }
  for (Int i = 0; i < m->getCount(); ++i) {
    this->addModifier(m->get(i));
  }
}


//==============================================================================
// Metadata Functions

void Node::setMetadata(SharedPtr<Map> const &newMetadata)
{
  UPDATE_OWNED_SHAREDPTR(this->metadata, newMetadata);
}


void Node::setMetadata(Map *newMetadata)
{
  this->setMetadata(getSharedPtr(newMetadata));
}


void Node::setMetadata(Char const *name, SharedPtr<Node> const &obj)
{
  if (this->metadata == 0) {
    this->setMetadata(Map::create({}));
  }
  this->metadata->set(name, obj);
}


void Node::addMetadata(SharedPtr<Map> const &newMetadata)
{
  if (newMetadata == 0) return;
  if (this->metadata == 0) {
    UPDATE_OWNED_SHAREDPTR(this->metadata, newMetadata);
    return;
  }
  for (Word i = 0; i < newMetadata->getCount(); ++i) {
    auto key = newMetadata->getKey(i);
    this->setMetadata(key, newMetadata->get(i));
  }
}


void Node::removeMetadata(Char const *name)
{
  if (this->metadata == 0) return;
  auto index = this->metadata->findIndex(name);
  if (index != -1) this->metadata->remove(index);
}


SharedPtr<Node> const& Node::getMetadata(Char const *name) const
{
  if (this->metadata == 0) return SharedPtr<Node>::null;
  auto index = this->metadata->findIndex(name);
  if (index == -1) return SharedPtr<Node>::null;
  else return this->metadata->get(index);
}


//==============================================================================
// Custom Data Functions

void Node::setCustomData(Char const *name, TioSharedPtr const &obj)
{
  if (this->customData == 0) this->customData = SharedMap<TiObject>::create({});
  this->customData->set(name, obj);
}


void Node::removeCustomData(Char const *name)
{
  if (this->customData == 0) return;
  auto index = this->customData->findIndex(name);
  if (index != -1) this->customData->remove(index);
}


TioSharedPtr const& Node::getCustomData(Char const *name) const
{
  if (this->customData == 0) return TioSharedPtr::null;
  auto index = this->customData->findIndex(name);
  if (index == -1) return TioSharedPtr::null;
  else return this->customData->get(index);
}


//==============================================================================
// Printing Functions

void Node::print(OutStream &stream, Int indents) const
{
  stream << this->getMyTypeInfo()->getTypeName();
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
  }
  this->printModifiers(stream, indents);
  this->printMetadata(stream, indents);
  this->printBinding(stream, indents);
  this->printContaining(stream, indents);
}


void Node::printModifiers(OutStream &stream, Int indents) const
{
  if (this->modifiers != 0 && this->modifiers->getCount() > 0) {
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("modifiers:");
    for (Int i = 0; i < this->modifiers->getCount(); ++i) {
      auto modifier = this->modifiers->get(i).get();
      if (modifier != 0) {
        stream << S("\n");
        printIndents(stream, indents+2);
        modifier->print(stream, indents+2);
      }
    }
  }
}


void Node::printMetadata(OutStream &stream, Int indents) const
{
  if (this->metadata != 0 && this->metadata->getCount() > 0) {
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("metadata:");
    for (Int i = 0; i < this->metadata->getCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+2);
      stream << metadata->getElementKey(i) << S(": ");
      dumpAst(stream, metadata->getElement(i), indents+2);
    }
  }  
}


void Node::printBinding(OutStream &stream, Int indents) const
{
  // Skip the first 4 members of Node and only print members from child classes.
  if (this->getMemberCount() > 4) {
    stream << S("\n");
    printIndents(stream, indents+1);
    stream << S("props:");
    for (Word i = 4; i < this->getMemberCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+2);
      stream << this->getMemberKey(i) << S(": ");
      dumpAst(stream, this->getMember(i), indents+1);
    }
  }
}


void Node::printContaining(OutStream &stream, Int indents) const
{
  auto mapContainer = this->getInterface<MapContaining<Node> const>();
  if (mapContainer != 0) {
    for (Word i = 0; i < mapContainer->getElementCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+1);
      stream << mapContainer->getElementKey(i) << S(": ");
      dumpAst(stream, mapContainer->getElement(i), indents+1);
    }
    return;
  }
  auto container = this->getInterface<Containing<Node> const>();
  if (container != 0) {
    for (Word i = 0; i < container->getElementCount(); ++i) {
      stream << S("\n");
      printIndents(stream, indents+1);
      dumpAst(stream, container->getElement(i), indents+1);
    }
  }
}

} // namespace
