/**
 * @file Srl/ArrayIndex.h
 * Contains the class Srl::ArrayIndex.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SRL_ARRAYINDEX_H
#define SRL_ARRAYINDEX_H

namespace Srl
{

template<class T> class ArrayIndex {
  //=================
  // Member Variables

  private: Array<ArchInt> indices;
  private: Array<T> *values;

  //===============
  // Initialization

  public: ArrayIndex() {
    this->values = 0;
  }

  public: ArrayIndex(Array<T> *v) {
    this->values = v;
    this->add(-1);
  }

  //=================
  // Member Functions

  public: void add(ArchInt i) {
    if (i == -1) {
      // Add any new items at the end of the values list.
      if (this->values->getLength() <= this->indices.getLength()) return;
      this->indices.reserve(this->values->getBufSize());
      while (this->values->getLength() > this->indices.getLength()) {
        i = this->indices.getLength();
        ArchInt pos;
        this->findPos(this->values->at(i), pos);
        this->indices.insert(pos, i);
      }
    } else if (i < this->values->getLength()) {
      // Add a new item at a specific location.
      // First update the existing indices.
      for (ArchInt j = 0; j < this->indices.getLength(); ++j) {
        if (this->indices(j) >= i) ++this->indices(j);
      }
      // Now insert the new value.
      ArchInt pos;
      this->findPos(this->values->at(i), pos);
      this->indices.insert(pos, i);
    } else {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range"), i);
    }
  }

  public: void remove(ArchInt i) {
    if (i >= this->indices.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("i"), S("Out of range"), i);
    }
    for (ArchInt j = 0; j < this->indices.getLength(); ++j) {
      if (this->indices(j) == i) {
        this->indices.remove(j);
        --j;
      } else if (this->indices(j) > i) {
        --this->indices(j);
      }
    }
  }

  public: void clear() {
    this->indices.clear();
  }

  public: ArchInt findPos(T const &v) {
    ArchInt r;
    if (this->findPos(v, r)) return this->indices(r);
    else return -1;
  }

  public: Bool findPos(T const &v, ArchInt &result) {
    if (this->indices.getLength() == 0) {
      result = 0;
      return false;
    }

    T &vf = this->values->at(this->indices(0));
    if (v < vf) {
      result = 0;
      return false;
    } else if (v == vf) {
      result = 0;
      return true;
    }

    T &vb(this->values->at(this->indices(this->indices.getLength() - 1)));
    if (v > vb) {
      result = this->indices.getLength();
      return false;
    } else if (v == vb) {
      result = this->indices.getLength() - 1;
      return true;
    }

    return this->_findPos(v, 0, this->indices.getLength() - 1, result);
  }

  private: Bool _findPos(T const &v, ArchInt startPos, ArchInt endPos, ArchInt &result) {
    if (startPos == endPos - 1) {
      result = endPos;
      return false;
    } else {
      ArchInt midPos = (startPos + endPos) / 2;
      T &vm = this->values->at(this->indices(midPos));
      if (v == vm) {
        result = midPos;
        return true;
      } else if (v < vm) {
        return this->_findPos(v, startPos, midPos, result);
      } else {
        return this->_findPos(v, midPos, endPos, result);
      }
    }
  }
}; // class

} // namespace

#endif
