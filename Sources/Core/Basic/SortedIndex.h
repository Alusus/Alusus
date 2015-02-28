/**
 * @file Core/Basic/SortedIndex.h
 * Contains definition of Basic::SortedIndex template class.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_SORTED_INDEX_H
#define BASIC_SORTED_INDEX_H

namespace Core { namespace Basic
{

/**
 * @brief A sorted index used to speed up searching.
 * @ingroup basic_utils
 *
 * This template class provide generic search functionality with worted indices.
 * It can act on any field of any type within any sequential array. The type,
 * however, must provide comparison operators which this class will use to sort
 * the indices and to perform binary searching.
 *
 * @tparam RECORD The type of the entire data record that contains the required
 *                field.
 * @tparam FIELD The type of the field for which search functionality is needed.
 * @tparam ACCESSOR A pointer to a function that can retrieve the required field
 *                  from the record.
 */
template<class RECORD, class FIELD, const FIELD& (*ACCESSOR)(RECORD const *r)> class SortedIndex
{
  //============================================================================
  // Member Variables

  /// The targeted vector of records.
  private: std::vector<RECORD> *records;

  /// The vector of sorted indices needed for binary searching.
  private: std::vector<Int> indices;


  //============================================================================
  // Constructors

  /// Initialize the object's variables and create the sorted indices list.
  public: SortedIndex(std::vector<RECORD> *r) : records(r)
  {
    this->add();
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Add new items to the list of indices.
   * Add the index of a new item to the list, or add the indices of any
   * additional items found in the target vector to the indices list. The
   * indices will be added at the correct sorted location.
   * This items looks at the end
   * of the list for new items by comparing the count of indices to the count
   * of records, so any new records should always be added to the end of the
   * list.
   * @param index If this value is -1, the function will look at the end of
   *              the records list for items beyond the size of the indices
   *              list. For example, if the records list have 5 items more
   *              than the indices list then there are 5 new items added to
   *              the end of the records list and indices for those new items
   *              will be added to the indices list. If index is not -1 then
   *              the item at index is new and that single index will be added
   *              to the indices list after incrementing existing indices that
   *              are greater than the new index.
   */
  public: void add(Int index=-1)
  {
    if (index == -1) {
      // Add any new items at the end of the records list.
      if (this->records->size() <= this->indices.size()) return;
      if (this->records->capacity() > this->indices.capacity()) {
        this->indices.reserve(this->records->capacity());
      }
      while (this->records->size() > this->indices.size()) {
        index = this->indices.size();
        const FIELD &v(this->getValue(index));
        Int pos;
        this->findPos(v, pos);
        this->indices.insert(this->indices.begin()+pos, index);
      }
    } else if (static_cast<Word>(index) < this->records->size()) {
      // Add a new item at a specific location.
      // First update the existing indices.
      for (Word i = 0; i < this->indices.size(); ++i) {
        if (this->indices[i] >= index) ++this->indices[i];
      }
      // Now insert the new value.
      const FIELD &v(this->getValue(index));
      Int pos;
      this->findPos(v, pos);
      this->indices.insert(this->indices.begin()+pos, index);
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
  }

  /**
   * @brief Remove the given index from the indices list.
   * This function must be called whenever a record is removed from the list.
   * This function can be called before or after the record is removed.
   * @param index The index of the removed record.
   */
  public: void remove(Int index)
  {
    if (static_cast<Word>(index) >= this->indices.size()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    for (Word i = 0; i < this->indices.size(); ++i) {
      if (this->indices[i] == index) {
        this->indices.erase(this->indices.begin()+i);
        --i;
      } else if (this->indices[i] > index) {
        --this->indices[i];
      }
    }
  }

  /**
   * @brief Clear the entire index array.
   * This does not affect the original records, only the indices. The records
   * should be manually deleted. This can be called before or after the
   * records are cleared.
   */
  public: void clear()
  {
    this->indices.clear();
  }

  /**
   * @brief Get the value at the record with the specified index.
   * This function extracts the value at OFFSET from the record, after doing
   * all the required casting.
   */
  private: const FIELD& getValue(Int index) const
  {
    return ACCESSOR(&this->records->at(index));
  }

  /**
   * @brief Binary search for the given item.
   * Binary saerch the list of records for the given value.
   * @return The index of the found record, or -1 otherwise.
   */
  public: Int find(const FIELD &v) const
  {
    Int r;
    if (this->findPos(v, r)) return this->indices[r];
    else return -1;
  }

  /**
   * @brief Find the index's position of the given value.
   * Finds the position within the indices array where the index of the given
   * value is, or should be. This function checks the beginning and the end of
   * the array, and if no match is found it calls the recursive _findPos to
   * perform binary search on the interior of the array.
   * @param v A reference to the value to search for.
   * @param result A reference to receive the found position. If the index is
   *               found this will be its position, otherwise this will be
   *               where such an index should be inserted.
   * @return true if an index for the given value was found, false otherwise.
   * @sa _findPos()
   */
  private: Bool findPos(const FIELD &v, Int &result) const
  {
    if (this->indices.empty()) {
      result = 0;
      return false;
    }
    const FIELD &vf(this->getValue(this->indices.front()));
    if (v < vf) {
      result = 0;
      return false;
    } else if (v == vf) {
      result = 0;
      return true;
    }
    const FIELD &vb(this->getValue(this->indices.back()));
    if (v > vb) {
      result = this->indices.size();
      return false;
    } else if (v == vb) {
      result = this->indices.size()-1;
      return true;
    }
    return this->_findPos(v, 0, this->indices.size()-1, result);
  }

  /**
   * A recursive function that uses binary search algorithm to search a
   * specific portion of the list for the given value. This function doesn't
   * test the values at startPos and endPos and it assumes they are checked
   * by the caller, which is findPos().
   * @param v A reference to the value to search for.
   * @param startPos The start position of the region to search.
   * @param endPos The end position of the region to search.
   * @param result A reference to receive the found position. If the index is
   *               found this will be its position, otherwise this will be
   *               where such an index should be inserted.
   * @return true if an index for the given value was found, false otherwise.
   * @sa findPos()
   */
  private: Bool _findPos(const FIELD &v, Int startPos, Int endPos, Int &result) const
  {
    ASSERT(endPos >= startPos && startPos >= 0);
    if (startPos == endPos-1) {
      result = endPos;
      return false;
    } else {
      Int midPos = (startPos + endPos) / 2;
      const FIELD &vm(this->getValue(this->indices[midPos]));
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

} } // namespace

#endif
