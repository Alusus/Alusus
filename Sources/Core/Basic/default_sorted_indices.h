/**
 * @file Core/Basic/default_sorted_indices.h
 * Contains definitions for default SortedIndex types used for basic types of
 * indices.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_DEFAULT_SORTED_INDICES_H
#define BASIC_DEFAULT_SORTED_INDICES_H

namespace Core { namespace Basic
{


template<class RECORD, class FIELD, FIELD RECORD::*OFFSET> const FIELD& direct_sorted_index_accessor(RECORD const *r)
{
  return r->*OFFSET;
}

template<class RECORD, class FIELD, FIELD* RECORD::*OFFSET> const FIELD& direct_sorted_index_accessor(RECORD const *r)
{
  return *(r->*OFFSET);
}


/**
 * @brief A sorted index that access elements contained inside the record.
 * @ingroup basic_utils
 *
 * A direct sorted index is an index that access members directly contained in
 * the record (embedded) rather than accessible through a pointer.
 *
 * @tparam RECORD The type of the entire data record that contains the required
 *                field.
 * @tparam FIELD The type of the field for which search functionality is needed.
 * @tparam OFFSET The offset of that field within the data record (the member
 *                pointer to the required field within the class).
 */
template<class RECORD, class FIELD, FIELD RECORD::*OFFSET>
class DirectSortedIndex : public SortedIndex<RECORD,FIELD,direct_sorted_index_accessor<RECORD,FIELD,OFFSET> >
{
  public: DirectSortedIndex(std::vector<RECORD> *r) :
    SortedIndex<RECORD,FIELD,direct_sorted_index_accessor<RECORD,FIELD,OFFSET> >(r)
  {
  }

}; // class

// A specialization for pointers.
template<class RECORD, class FIELD, FIELD* RECORD::*OFFSET>
class DirectSortedIndex<RECORD,FIELD*,OFFSET> :
    public SortedIndex<RECORD,FIELD,direct_sorted_index_accessor<RECORD,FIELD,OFFSET> >
{
  public: DirectSortedIndex(std::vector<RECORD> *r) :
    SortedIndex<RECORD,FIELD,direct_sorted_index_accessor<RECORD,FIELD,OFFSET> >(r)
  {
  }

}; // class


template<class RECORD, class CONTAINER, class FIELD, CONTAINER* RECORD::*POINTER, FIELD CONTAINER::*OFFSET>
const FIELD& accessIndirectSortedIndex(RECORD const *r)
{
  return r->*POINTER->*OFFSET;
}

template<class RECORD, class CONTAINER, class FIELD, CONTAINER* RECORD::*POINTER, FIELD* CONTAINER::*OFFSET>
const FIELD& accessIndirectSortedIndex(RECORD const *r)
{
  return *(r->*POINTER->*OFFSET);
}


/**
 * @brief A sorted index that access elements pointed-to from the record.
 * @ingroup basic_utils
 *
 * An indirect sorted index is an index that access members indirectly contained
 * in the record (contained inside another container that is pointed to by this
 * record).
 *
 * @tparam RECORD The type of the entire data record that contains the required
 *                field.
 * @tparam CONTAINER The type of the container class (the class that contains
 *                   FIELD and is pointed to by RECORD).
 * @tparam FIELD The type of the field for which search functionality is needed.
 * @tparam POINTER A pointer to the pointer to CONTAINER within RECORD.
 * @tparam OFFSET The offset of that field within CONTAINER (the member pointer
 *                to the required field within the class).
 */
template<class RECORD, class CONTAINER, class FIELD, CONTAINER* RECORD::*POINTER, FIELD CONTAINER::*OFFSET>
class IndirectSortedIndex :
    public SortedIndex<RECORD,FIELD,accessIndirectSortedIndex<RECORD,CONTAINER,FIELD,POINTER,OFFSET> >
{
  public: IndirectSortedIndex(std::vector<RECORD> *r) :
    SortedIndex<RECORD,FIELD,accessIndirectSortedIndex<RECORD,CONTAINER,FIELD,POINTER,OFFSET> >(r)
  {
  }

}; // class

// A specialization for pointers.
template<class RECORD, class CONTAINER, class FIELD, CONTAINER* RECORD::*POINTER, FIELD* CONTAINER::*OFFSET>
class IndirectSortedIndex<RECORD,CONTAINER,FIELD*,POINTER,OFFSET> :
    public SortedIndex<RECORD,FIELD,accessIndirectSortedIndex<RECORD,CONTAINER,FIELD,POINTER,OFFSET> >
{
  public: IndirectSortedIndex(std::vector<RECORD> *r) :
    SortedIndex<RECORD,FIELD,accessIndirectSortedIndex<RECORD,CONTAINER,FIELD,POINTER,OFFSET> >(r)
  {
  }

}; // class

} } // namespace

#endif
