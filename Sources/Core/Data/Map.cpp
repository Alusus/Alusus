/**
 * @file Core/Data/Map.cpp
 * Contains the implementation of class Core::Data::Map.
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

Map::Map(Bool useIndex, const std::initializer_list<Argument<const Char*>> &args) : inherited(0), parent(0)
{
  if (useIndex) this->index = new Index(&this->list);
  else this->index = 0;

  for (auto arg : args) {
    if (sbstr_cast(arg.id) == STR("@parent")) {
      this->parentReference = arg.ioVal.io_cast<Reference>();
      if (this->parentReference == 0 && arg.ioVal != 0) {
        throw GeneralException(STR("Provided parent reference is not of type Reference."),
                               STR("Core::Data::Map::Map"));
      }
    } else {
      this->add(arg.id, arg.ioVal);
    }
  }
}


//============================================================================
// Member Functions

void Map::attachToParent(Map *p)
{
  ASSERT(this->parent == 0);
  ASSERT(this->inherited == 0);
  this->parent = p;
  this->parent->contentChangeNotifier.connect(this, &Map::onParentContentChanged);
  this->parent->destroyNotifier.connect(this, &Map::onParentDestroyed);
  this->inherited = new std::vector<Bool>(this->list.size(), false);
  this->inheritFromParent();
}


void Map::detachFromParent()
{
  ASSERT(this->parent != 0);
  this->removeInheritted();
  this->parent->contentChangeNotifier.unconnect(this, &Map::onParentContentChanged);
  this->parent->destroyNotifier.unconnect(this, &Map::onParentDestroyed);
  this->parent = 0;
  delete this->inherited;
  this->inherited = 0;
}


void Map::inheritFromParent()
{
  ASSERT(this->parent != 0);
  for (Int i = 0; static_cast<Word>(i) < this->getParentDefCount(); ++i) this->onAdded(i);
}


void Map::removeInheritted()
{
  ASSERT(this->inherited != 0);
  for (Int i = this->inherited->size()-1; i >= 0; --i) {
    if (this->inherited->at(i)) {
      this->onRemoved(i);
    }
  }
}


void Map::onParentContentChanged(Container *obj, ContentChangeOp op, Int index)
{
  if (op == ContentChangeOp::ADD) this->onAdded(index);
  else if (op == ContentChangeOp::UPDATE) this->onUpdated(index);
  else if (op == ContentChangeOp::REMOVE) this->onRemoved(index);
}


void Map::onAdded(Int index)
{
  ASSERT(this->parent != 0);
  ASSERT(this->inherited != 0);
  ASSERT(static_cast<Word>(index) < this->getParentDefCount());
  const Char *key = this->parent->getKey(index).c_str();
  Int myIndex = this->findIndex(key);
  SharedPtr<IdentifiableObject> obj;
  if (myIndex != -1 && myIndex != index) {
    obj = this->get(myIndex);
    this->list.erase(this->list.begin()+myIndex);
    this->inherited->erase(this->inherited->begin()+myIndex);
    if (this->index != 0) this->index->remove(myIndex);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, myIndex);
    this->list.insert(this->list.begin()+index, Entry(key, obj));
    this->inherited->insert(this->inherited->begin()+index, false);
  } else if (myIndex == -1) {
    obj = this->parent->get(index);
    this->list.insert(this->list.begin()+index, Entry(key, obj));
    this->inherited->insert(this->inherited->begin()+index, true);
  }
  if (this->index != 0) this->index->add(index);
  this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, index);
}


void Map::onUpdated(Int index)
{
  ASSERT(this->parent != 0);
  ASSERT(this->inherited != 0);
  ASSERT(static_cast<Word>(index) < this->getParentDefCount());
  if (this->inherited->at(index)) {
    this->list[index].second = this->parent->get(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
  }
}


void Map::onRemoved(Int index)
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
    SharedPtr<IdentifiableObject> obj = this->get(index);
    this->list.erase(this->list.begin()+index);
    this->inherited->erase(this->inherited->begin()+index);
    if (this->index != 0) this->index->remove(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, index);
    this->add(key.c_str(), obj);
  }
}


Int Map::add(const Char *key, const SharedPtr<IdentifiableObject> &val)
{
  Int i = this->findIndex(key);
  if (i != -1) {
    if (this->inherited != 0 && this->inherited->at(i)) {
      this->list[i].second = val;
      this->inherited->at(i) = false;
      this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, i);
    } else {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::Map::add"),
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


void Map::insert(Int index, const Char *key, const SharedPtr<IdentifiableObject> &val)
{
  if (this->findIndex(key) != -1) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::Map::insert"),
                                   STR("Already exists."));
  }
  if (static_cast<Word>(index) < this->getParentDefCount()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::Map::insert"),
                                   STR("Cannot insert at a position that breaks parent's sequence."));
  }
  this->list.insert(this->list.begin()+index, Entry(key, val));
  if (this->inherited != 0) this->inherited->insert(this->inherited->begin()+index, false);
  if (this->index != 0) this->index->add(index);
  this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, index);
}


Int Map::set(const Char *key, const SharedPtr<IdentifiableObject> &val, Bool insertIfNew)
{
  Int idx = this->findIndex(key);
  if (idx == -1) {
    if (insertIfNew) {
      idx = this->list.size();
      this->list.push_back(Entry(key, val));
      if (this->inherited != 0) this->inherited->push_back(false);
      this->index->add();
      this->contentChangeNotifier.emit(this, ContentChangeOp::ADD, idx);
    } else {
      throw InvalidArgumentException(STR("key"), STR("Core::Data::Map::set"),
                                     STR("Not found."));
    }
  } else {
    this->list[idx].second = val;
    if (this->inherited != 0) this->inherited->at(idx) = false;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, idx);
  }
  return idx;
}


void Map::clear()
{
  Int i = 0;
  while (static_cast<Word>(i) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    if (!this->inherited->at(i)) {
      this->list[i].second = this->parent->get(i);
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


//==============================================================================
// Initializable Implementation

void Map::initialize(Manager *manager, const SharedPtr<Module> &module)
{
  GrammarHelper helper(manager->getInterface<Provider>());
  if (this->parentReference != 0) {
    IdentifiableObject *p = helper.traceValue(this->parentReference.get(), module.get());
    if (p == 0) {
      throw GeneralException(STR("Parent reference points to missing definition."),
                             STR("Data::Map::initialize"));
    }
    Map *pm = io_cast<Map>(p);
    if (pm == 0) {
      throw GeneralException(STR("Parent reference points to an object of an invalid type."),
                             STR("Data::Map::initialize"));
    }
    this->setParent(pm);
  }
}


//==============================================================================
// DataOwner Implementation

void Map::unsetIndexes(Int from, Int to)
{
  if (this->parentReference != 0) {
    Data::unsetIndexes(this->parentReference.get(), from, to);
  }
  for (Word i = 0; i < this->getCount(); ++i) {
    if (this->inherited == 0 || !this->inherited->at(i)) {
      IdentifiableObject *obj = this->get(i).get();
      if (obj != 0) Data::unsetIndexes(obj, from, to);
    }
  }
}


//==============================================================================
// MapContainer Implementation

void Map::set(Int index, const SharedPtr<IdentifiableObject> &val)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::Map::set"),
                                   STR("Out of range."));
  }
  this->list[index].second = val;
  if (this->inherited != 0) this->inherited->at(index) = false;
  this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
}


void Map::remove(Int index)
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::Map::remove"),
                                   STR("Out of range."));
  }
  if (this->inherited != 0 && this->inherited->at(index)) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::Map::remove"),
                                   STR("Given entry belongs to parent."));
  }
  if (static_cast<Word>(index) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    this->list[index].second = this->parent->get(index);
    this->inherited->at(index) = true;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, index);
  } else {
    this->list.erase(this->list.begin()+index);
    if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+index);
    if (this->index != 0) this->index->remove(index);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, index);
  }
}


const SharedPtr<IdentifiableObject>& Map::get(Int index) const
{
  if (static_cast<Word>(index) >= this->list.size()) {
    throw InvalidArgumentException(STR("index"), STR("Core::Data::Map::get"),
                                   STR("Out of range."));
  }
  return this->list[index].second;
}


void Map::remove(const Char *key)
{
  Int idx = this->findIndex(key);
  if (idx == -1 || (this->inherited != 0 && this->inherited->at(idx) == true)) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::Map::remove"),
                                   STR("Not found."));
  }
  if (static_cast<Word>(idx) < this->getParentDefCount()) {
    ASSERT(this->parent != 0);
    ASSERT(this->inherited != 0);
    this->list[idx].second = this->parent->get(idx);
    this->inherited->at(idx) = true;
    this->contentChangeNotifier.emit(this, ContentChangeOp::UPDATE, idx);
  } else {
    this->list.erase(this->list.begin()+idx);
    if (this->inherited != 0) this->inherited->erase(this->inherited->begin()+idx);
    if (this->index != 0) this->index->remove(idx);
    this->contentChangeNotifier.emit(this, ContentChangeOp::REMOVE, idx);
  }
}


Int Map::findIndex(const Char *key) const
{
  if (key == 0) {
    throw InvalidArgumentException(STR("key"), STR("Core::Data::Map::findIndex"),
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
