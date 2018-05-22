/**
 * @file Core/Data/Grammar/ListTerm.cpp
 * Contains the implementation of class Core::Data::Grammar::ListTerm.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

//==============================================================================
// Member Functions

void ListTerm::validate() const
{
  if (this->terms == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("term"), STR("Must not be null."));
  } else if (this->terms->isDerivedFrom<List>()) {
    if (this->data != 0 && !this->data->isDerivedFrom<List>() && this->data->isDerivedFrom<TiInt>() &&
        !this->data->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Filters must be of type List, TiInt or Reference."));
    }
  } else if (this->terms->isDerivedFrom<Term>()) {
    if (this->data == 0 || (!this->data->isDerivedFrom<List>() &&
                            !this->data->isDerivedFrom<Reference>())) {
      throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Data must be of type List or Reference."));
    }
    if (this->targetRef == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("ref"),  STR("Target variable must be provided with data lists."));
    }
  } else {
    throw EXCEPTION(InvalidArgumentException, STR("term"), STR("Provided object is of an invalid type."),
                    this->terms->getMyTypeInfo()->getUniqueName());
  }
}


/**
 * @param terms A List of Term objects to be used as a static list.
 * @param filter An optional filter object. This can either be an TiInt
 *               specifying which term is to be active, a list of Integers
 *               (used as booleans) specifying which term is active and
 *               which term isn't, or a Reference to either.
 */
void ListTerm::setStatic(SharedPtr<List> const &terms, SharedPtr<Node> const &filter)
{
  if (terms == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("terms"), STR("Cannot be null."));
  }
  if (filter != 0 && !filter->isA<List>() &&
      !filter->isA<TiInt>() &&
      !filter->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("filter"),
                    STR("Must be either List, TiInt, or a Reference."));
  }
  UPDATE_OWNED_SHAREDPTR(this->terms, terms);
  UPDATE_OWNED_SHAREDPTR(this->data, filter);
  this->targetRef.reset();
}


/**
 * Dynamic lists are a list of data objects applied on a single term object.
 * @param term A single term object to receive the data list.
 * @param data A List of data objects, or a Reference to such List.
 * @param var A reference to the stack variable to be used for the loop.
 */
void ListTerm::setDynamic(const SharedPtr<Term> &term, SharedPtr<Node> const &data,
                          const SharedPtr<Reference> &ref)
{
  if (term == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("term"), STR("Cannot be null."));
  }
  if (data == 0 || (!data->isA<List>() && !data->isDerivedFrom<Reference>())) {
    throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Must be of type List or Reference."));
  }
  if (ref == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("ref"), STR("Cannot be null."));
  }
  UPDATE_OWNED_SHAREDPTR(this->terms, terms);
  UPDATE_OWNED_SHAREDPTR(this->data, data);
  this->targetRef = ref;
}


void ListTerm::reset()
{
  RESET_OWNED_SHAREDPTR(this->terms);
  RESET_OWNED_SHAREDPTR(this->data);
  this->targetRef.reset();
}


/**
 * @param index The index of the term to return. If the list contains a
 *              singular term, that term will be returned regardless of this
 *              parameter.
 */
SharedPtr<Term> ListTerm::getTerm(Int index) const
{
  List const *list = tio_cast<List>(this->terms.get());
  if (list) {
    if (static_cast<Word>(index) >= list->getCount()) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
    }
    SharedPtr<Term> term = list->get(index).s_cast<Term>();
    if (!term->isDerivedFrom<Term>()) {
      throw EXCEPTION(GenericException, STR("List contains a non-Term object."));
    }
    return term;
  } else {
    return this->terms.s_cast<Term>();
  }
}


//==============================================================================
// DataHaving Implementation

void ListTerm::unsetIndexes(Int from, Int to)
{
  if (this->terms != 0) Data::unsetIndexes(this->terms.get(), from, to);
  if (this->data != 0) Data::unsetIndexes(this->data.get(), from, to);
  if (this->targetRef != 0) Data::unsetIndexes(this->targetRef.get(), from, to);
}

} // namespace
