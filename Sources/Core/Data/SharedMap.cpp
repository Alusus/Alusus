/**
 * @file Core/Data/SharedMap.cpp
 * Contains the implementation of class Core::Data::SharedMap.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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
// Constructors

SharedMap::SharedMap(Bool useIndex, const std::initializer_list<Argument<Char const*>> &args) : inherited(0), parent(0)
{
  if (useIndex) this->index = new Index(&this->list);
  else this->index = 0;

  for (auto arg : args) {
    if (sbstr_cast(arg.id) == STR("@parent")) {
      this->parentReference = arg.ioVal.io_cast<Reference>();
      if (this->parentReference == 0 && arg.ioVal != 0) {
        throw GeneralException(STR("Provided parent reference is not of type Reference."),
                               STR("Core::Data::SharedMap::SharedMap"));
      }
    } else {
      this->add(arg.id, arg.ioVal);
    }
  }
}


//============================================================================
// Member Functions

void SharedMap::attachToParent(SharedMap *p)
{
  ASSERT(this->parent == 0);
  ASSERT(this->inherited == 0);
  this->parent = p;
  this->parent->contentChangeNotifier.connect(this, &SharedMap::onParentContentChanged);
  this->parent->destroyNotifier.connect(this, &SharedMap::onParentDestroyed);
  this->inherited = new std::vector<Bool>(this->list.size(), false);
  this->inheritFromParent();
}


void SharedMap::detachFromParent()
{
  ASSERT(this->parent != 0);
  this->removeInheritted();
  this->parent->contentChangeNotifier.unconnect(this, &SharedMap::onParentContentChanged);
  this->parent->destroyNotifier.unconnect(this, &SharedMap::onParentDestroyed);
  this->parent = 0;
  delete this->inherited;
  this->inherited = 0;
}


void SharedMap::inheritFromParent()
{
  ASSERT(this->parent != 0);
  for (Int i = 0; static_cast<Word>(i) < this->getParentDefCount(); ++i) this->onAdded(i);
}


void SharedMap::removeInheritted()
{
  ASSERT(this->inherited != 0);
  for (Int i = this->inherited->size()-1; i >= 0; --i) {
    if (this->inherited->at(i)) {
      this->onRemoved(i);
    }
  }
}


void SharedMap::onParentContentChanged(Container *obj, ContentChangeOp op, Int index)
{
  if (op == ContentChangeOp::ADD) this->onAdded(index);
  else if (op == ContentChangeOp::UPDATE) this->onUpdated(index);
  else if (op == ContentChangeOp::REMOVE) this->onRemoved(index);
}


void SharedMap::onAdded(Int index)
{
  ASSERT(this->parent != 0);
  ASSERT(this->inherited != 0);
  ASSERT(static_cast<Word>(index) < this->getParentDefCount());
  Char const *key = this->parent->getKey(index).c_str();
  Int myIndex = this->findIndex(key);
  SharedPtr<IdentifiableObject> obj;
  if (myIndex != -1 && myIndex != index) {
    obj = this->getShared(myIndex);
    this->list.erase(this->list.begin()+myIndex);
    this->inherited->erase(this->inherited->begin()+myIndex);
    if (this->index != 0) this->index->remove(myIndex);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, myIndex);
    this->list.insert(this->list.begin()+index, Entry(key, obj));
    this->inherited->insert(this->inherited->begin()+index, false);
  } else if (myIndex == -1) {
    obj = this->parent->getShared(index);
    this->list.insert(this->list.begin()+index, Entry(key, obj));
    this->inherited->insert(this->inherited->begin()+index, true);
  }
  if (this->index != 0) this->index->add(index);
  this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, index);
}


void SharedMap::onUpdated(Int index)
{
  ASSERT(this->parent != 0);
  ASSERT(this->inherited != 0);
  ASSERT(static_cast<Word>(index) < this->getParentDefCount());
  if (this->inherited->at(index)) {
    this->list[index].second = this->parent->getShared(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
  }
}


void SharedMap::onRemoved(Int index)
{
  ASSERT(this->parent != 0);
  ASSERT(this->inherited != 0);
  ASSERT(static_cast<Word>(index) < this->getParentDefCount()+1);
  if (this->inherited->at(index)) {
    this->list.erase(this->list.begin()+index);
    this->inherited->erase(this->inherited->begin()+index);
    if (this->index != 0) this->index->remove(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, index);
  } else {
    Str key = this->getKey(index);
    SharedPtr<IdentifiableObject> obj = this->getShared(index);
    this->list.erase(this->list.begin()+index);
    this->inherited->erase(this->inherited->begin()+index);
    if (this->index != 0) this->index->remove(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, index);
    this->add(key.c_str(), obj);
  }
}


Int SharedMap::add(Char const *key, SharedPtr<IdentifiableObject> const &val)
{
  Int i = this->findIndex(key);
  if (i != -1) {
    if (this->inherited != 0 && this->inherited->at(i)) {
      this->list[i].second = val;
      this->inherited->at(i) = false;
      this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, i);
    } else {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::add"),
                                     STR("Already exists."));
    }
  } else {
    i = this->list.size();
    this->list.push_back(Entry(key, val));
    if (this->inherited != 0) this->inherited->push_back(false);
    if (this->index != 0) this->index->add();
    this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, i);
  }
  return i;
}


void SharedMap::insert(Int index, Char const *key, SharedPtr<IdentifiableObject> const &val)
{
  if (this->findIndex(key) != -1) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::insert"),
                                   STR("Already exists."));
  }
  if (static_cast<Word>(index) < this->getParentDefCount()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::insert"),
                                   STR("Cannot insert at a position that breaks parent's sequence."));
  }
  this->list.insert(this->list.begin()+index, Entry(key, val));
  if (this->inherited != 0) this->inherited->insert(this->inherited->begin()+index, false);
  if (this->index != 0) this->index->add(index);
  this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, index);
}


Int SharedMap::set(Char const *key, SharedPtr<IdentifiableObject> const &val, Bool insertIfNew)
{
  Int idx = this->findIndex(key);
  if (idx == -1) {
    if (insertIfNew) {
      idx = this->list.size();
      this->list.push_back(Entry(key, val));
      if (this->inherited != 0) this->inherited->push_back(false);
      if (this->index != 0) this->index->add();
      this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, idx);
    } else {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::set"),
                                     STR("Not found."));
    }
  } else {
    this->list[idx].second = val;
    if (this->inherited != 0) this->inherited->at(idx) = false;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, idx);
  }
  return idx;
}


void SharedMap::set(Int index, SharedPtr<IdentifiableObject> const &val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::set"),
                                   STR("Out of range."));
  }
  this->list[index].second = val;
  if (this->inherited != 0) this->inherited->at(index) = false;
  this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
}


SharedPtr<IdentifiableObject> const& SharedMap::getShared(Char const *key) const
{
  Int idx = this->findIndex(key);
  if (idx == -1) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::getShared"),
                                   STR("Not found in the map."));
  }
  return this->list[idx].second;
}


SharedPtr<IdentifiableObject> const& SharedMap::getShared(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::getShared"),
                                   STR("Out of range."));
  }
  return this->list[index].second;
}


void SharedMap::clear()
{
  Int i = 0;
  while (static_cast<Word>(i) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    if (!this->inherited->at(i)) {
      this->list[i].second = this->parent->getShared(i);
      this->inherited->at(i) = true;
      this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, i);
    }
    ++i;
  }
  Int j = this->getCount() - 1;
  while (j >= i) {
    this->remove(j);
    --j;
  }
}


Bool SharedMap::isInherited(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::isInherited"),
                                   STR("Out of range."));
  }
  if (this->inherited == 0) return false;
  else return this->inherited->at(index);
}


Int SharedMap::getIndex(Char const *key) const
{
  Int idx = this->findIndex(key);
  if (idx == -1) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::getIndex"),
                                   STR("Not found in the map."), key);
  }
  return idx;
}


//==============================================================================
// Initializable Implementation

void SharedMap::initialize(IdentifiableObject *owner)
{
  if (this->parentReference != 0) {
    Tracer *tracer = owner->getInterface<Tracer>();
    IdentifiableObject *p = tracer->traceValue(this->parentReference.get());
    if (p == 0) {
      throw GeneralException(STR("Parent reference points to missing definition."),
                             STR("Data::SharedMap::initialize"));
    }
    SharedMap *pm = io_cast<SharedMap>(p);
    if (pm == 0) {
      throw GeneralException(STR("Parent reference points to an object of an invalid type."),
                             STR("Data::SharedMap::initialize"));
    }
    this->setParent(pm);
  }
}


//==============================================================================
// DataOwner Implementation

void SharedMap::unsetIndexes(Int from, Int to)
{
  if (this->parentReference != 0) {
    Data::unsetIndexes(this->parentReference.get(), from, to);
  }
  for (Word i = 0; i < this->getCount(); ++i) {
    if (this->inherited == 0 || !this->inherited->at(i)) {
      IdentifiableObject *obj = this->get(i);
      if (obj != 0) Data::unsetIndexes(obj, from, to);
    }
  }
}


//==============================================================================
// MapContainer Implementation

void SharedMap::set(Int index, IdentifiableObject *val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::set"),
                                   STR("Out of range."));
  }
  this->list[index].second = getSharedPtr(val, true);
  if (this->inherited != 0) this->inherited->at(index) = false;
  this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
}


void SharedMap::remove(Int index)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::remove"),
                                   STR("Out of range."));
  }
  if (this->inherited != 0 && this->inherited->at(index)) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::remove"),
                                   STR("Given entry belongs to parent."));
  }
  if (static_cast<Word>(index) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    this->list[index].second = this->parent->getShared(index);
    this->inherited->at(index) = true;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
  } else {
    this->list.erase(this->list.begin()+index);
    if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+index);
    if (this->index != 0) this->index->remove(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, index);
  }
}


IdentifiableObject* SharedMap::get(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::get"),
                                   STR("Out of range."));
  }
  return this->list[index].second.get();
}


void SharedMap::remove(Char const *key)
{
  Int idx = this->findIndex(key);
  if (idx == -1 || (this->inherited != 0 && this->inherited->at(idx) == true)) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::remove"),
                                   STR("Not found."));
  }
  if (static_cast<Word>(idx) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    this->list[idx].second = this->parent->getShared(idx);
    this->inherited->at(idx) = true;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, idx);
  } else {
    this->list.erase(this->list.begin()+idx);
    if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+idx);
    if (this->index != 0) this->index->remove(idx);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, idx);
  }
}


IdentifiableObject* SharedMap::get(Char const *key) const
{
  Int idx = this->findIndex(key);
  if (idx == -1) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::get"),
                                   STR("Not found in the map."));
  }
  return this->list[idx].second.get();
}


SbStr const& SharedMap::getKey(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::SharedMap::getKey"),
                                   STR("Out of range."), index);
  }
  return this->list[index].first.sbstr();
}


Int SharedMap::findIndex(Char const *key) const
{
  if (key == 0) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::SharedMap::findIndex"),
                                   STR("Cannot be null."));
  }
  // Do we have an index to speed up search?
  if (this->index != 0) {
    return this->index->find(Str(key));
  } else {
    for (Word i = 0; i < this->list.size(); ++i) {
      if (this->list[i].first == key) return i;
    }
    return -1;
  }
}

} } // namespace
