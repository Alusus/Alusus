/**
 * @file Core/Basic/SubsetIndex.h
 * Contains definition of Basic::SubsetIndex template class.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SUBSETINDEX_H
#define CORE_BASIC_SUBSETINDEX_H

namespace Core::Basic
{

/**
 * @brief An index for a subset of an array.
 */
class SubsetIndex
{
  //============================================================================
  // Member Variables

  private: Srl::Array<Int> indices;


  //============================================================================
  // Constructors

  public: SubsetIndex()
  {
  }


  //============================================================================
  // Member Functions

  public: void clear()
  {
    this->indices.clear();
  }

  public: void onAdded(Int index, Bool withinSubset)
  {
    Int pos = this->findPos(index);
    if (pos == -1) {
      if (withinSubset) this->indices.add(index);
    } else {
      if (withinSubset) this->indices.insert(pos, index);
      for (Int i = pos; i < this->indices.getLength(); ++i) ++this->indices(i);
    }
  }

  public: void onUpdated(Int index, Bool withinSubset)
  {
    Int pos = this->findPos(index);
    if (pos == -1) return;
    if (withinSubset && this->indices(pos) != index) this->indices.insert(pos, index);
    else if (!withinSubset && this->indices(pos) == index) this->indices.remove(pos);
  }

  public: void onRemoved(Int index)
  {
    Int pos = this->findPos(index);
    if (pos == -1) return;
    if (this->indices(pos) == index) this->indices.remove(pos);
    for (Int i = pos; i < this->indices.getLength(); ++i) --this->indices(i);
  }

  public: Word getSize() const
  {
    return this->indices.getLength();
  }

  public: Int get(Int pos) const
  {
    if (pos < 0 || pos >= this->indices.getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("pos"), S("Out of range"), pos);
    }
    return this->indices(pos);
  }

  private: Int findPos(Int index) const
  {
    if (this->indices.getLength() == 0) {
      return -1;
    }
    if (index <= this->indices(0)) {
      return 0;
    } else if (index > this->indices(this->indices.getLength() - 1)) {
      return -1;
    } else {
      return this->_findPos(index, 0, this->indices.getLength() - 1);
    }
  }

  private: Int _findPos(Int index, Int startPos, Int endPos) const
  {
    ASSERT(endPos >= startPos && startPos >= 0);
    if (startPos == endPos-1) {
      return endPos;
    } else {
      Int midPos = (startPos + endPos) / 2;
      if (index == this->indices(midPos)) {
        return midPos;
      } else if (index < this->indices(midPos)) {
        return this->_findPos(index, startPos, midPos);
      } else {
        return this->_findPos(index, midPos, endPos);
      }
    }
  }

}; // class

} // namespace

#endif
