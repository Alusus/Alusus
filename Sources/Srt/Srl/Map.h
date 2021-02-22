/**
 * @file Srl/Map.h
 * Contains the Srl::Map type.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SRL_MAP_H
#define SRL_MAP_H

namespace Srl
{

template<class T1, class T2> class Map {
  //=================
  // Member Variables

  private: Array<T1> keys;
  private: Array<T2> values;
  private: ArrayIndex<T1> *keysIndex;

  //===============
  // Initialization

  public: Map() : keysIndex(0) {
  }

  public: Map(Bool useIndex) {
    if (useIndex) this->keysIndex = new ArrayIndex<T1>(&this->keys);
    else this->keysIndex = 0;
  }

  public: Map(Map<T1, T2> const &map) {
    this->keysIndex = 0;
    this->keys = map.keys;
    this->values = map.values;
  }

  public: Map(Map<T1, T2> const &map, Bool useIndex) {
    this->Keys = map.keys;
    this->values = map.values;
    if (useIndex) {
      this->keysIndex = new ArrayIndex<T1>(&this->keys);
      if (map.keysIndex != 0) this->keysIndex->indices = map.keysIndex->indices;
      else this->keysIndex->add(-1);
    } else {
      this->keysIndex = 0;
    }
  }

  public: ~Map() {
    if (this->keysIndex != 0) delete this->keysIndex;
  }

  //==========
  // Operators

  public: Map<T1, T2>& operator=(Map<T1, T2> const &map) {
    this->keys = map.keys;
    this->values = map.values;
    if (this->keysIndex != 0) {
      if (map.keysIndex != 0) this->keysIndex->indices = map->keysIndex->indices;
      else {
        this->keysIndex->clear();
        this->keysIndex->add(-1);
      }
    }
    return *this;
  }

  public: T2& operator()(T1 const &key) {
    ArchInt i = this->findPos(key);
    if (i == -1) {
      i = this->keys.getLength();
      this->keys.add(key);
      this->values.add(T2());
      if (this->keysIndex != 0) this->keysIndex->add(-1);
    }
    return this->values(i);
  }

  //=================
  // Member Functions

  public: T1 const& keyAt(ArchInt i) const {
    if (i < 0 || i >= this->keys.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
    }
    return this->keys(i);
  }

  public: T2& valAt(ArchInt i) {
    if (i < 0 || i >= this->keys.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
    }
    return this->values(i);
  }

  public: T2 const& valAt(ArchInt i) const {
    if (i < 0 || i >= this->keys.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
    }
    return this->values(i);
  }

  public: Map<T1, T2>& set(T1 const &key, T2 const &value) {
    ArchInt pos = this->findPos(key);
    if (pos == -1) {
      this->keys.add(key);
      this->values.add(value);
      if (this->keysIndex != 0) this->keysIndex->add(-1);
    } else {
      this->values(pos) = value;
    }
    return *this;
  }

  public: Map<T1, T2>& setAt(ArchInt i, T2 const &value) {
    if (i < 0 || i >= this->keys.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
    }
    this->values(i) = value;
    return *this;
  }

  public: void insert(ArchInt i, T1 const &key, T2 const &value) {
    this->keys.insert(i, key);
    this->values.insert(i, value);
    if (this->keysIndex != 0) this->keysIndex->add(i);
  }

  public: Bool remove(T1 const &key) {
    ArchInt pos = this->findPos(key);
    if (pos == -1) return false;
    this->removeAt(pos);
    return true;
  }

  public: void removeAt(ArchInt i) {
    if (i < 0 || i >= this->keys.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range."), i);
    }
    this->keys.remove(i);
    this->values.remove(i);
    if (this->keysIndex != 0) this->keysIndex->remove(i);
  }

  public: void clear() {
    this->keys.clear();
    this->values.clear();
    if (this->keysIndex != 0) this->keysIndex->clear();
  }

  public: ArchInt getLength() const {
    return this->keys.getLength();
  }

  public: ArchInt findPos(T1 const &key) const {
    if (this->keysIndex == 0) {
      return this->keys.findPos(key);
    } else {
      return this->keysIndex->findPos(key);
    }
  }

  public: Array<T1> getKeys() const {
    return this->keys;
  }

  public: Array<T2> getValues() const {
    return this->values;
  }

}; // class

} // namespace

#endif
