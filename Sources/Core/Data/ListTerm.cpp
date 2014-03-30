/**
 * @file Core/Data/ListTerm.cpp
 * Contains the implementation of class Data::ListTerm.
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

/**
 * This accepts the following types of arguments:<br/>
 * FLAGS: Parsing flags to be passed to Term's constructor.<br/>
 * REF: A reference to the stack variable to be used for the loop. Needed for
 *      dynamic lists.<br/>
 * DATA: For dynamic lists, this must hold a list of data objects used during
 *       the loop. For static lists, this must be an identifiable object to
 *       hold the filter. This must be either an integer specifying the
 *       selected term in the list, or a list of integers used as boolean
 *       flags for which terms are enabled and which are disabled. This can
 *       also be a reference to either integer or list of integers. If this
 *       is null, no filtering will be done.<br/>
 * TERM: A pointer to the list of terms if it's a static list. Otherwise, a
 *       pointer to a single Term object.
 */
ListTerm::ListTerm(const std::initializer_list<Argument<TermElement>> &args)
{
  for (auto arg : args) {
    switch (arg.id.val) {
      case TermElement::FLAGS:
        this->setFlags(arg.intVal);
        break;
      case TermElement::REF:
        this->targetRef = arg.ioVal.io_cast<Reference>();
        if (this->targetRef == 0 && arg.ioVal != 0) {
          throw InvalidArgumentException(STR("ref"), STR("Core::Data::ListTerm::ListTerm"),
                                         STR("Object must be of type Reference."),
                                         arg.ioVal->getMyTypeInfo()->getUniqueName());
        }
        break;
      case TermElement::DATA:
        this->data = arg.ioVal;
        break;
      case TermElement::TERM:
        this->terms = arg.ioVal;
        break;
    }
  }
  if (this->terms == 0) {
    throw InvalidArgumentException(STR("term"), STR("Core::Data::ListTerm::ListTerm"),
                                   STR("Must not be null."));
  } else if (this->terms->isDerivedFrom<List>()) {
    if (this->data != 0 && !this->data->isDerivedFrom<List>() && this->data->isDerivedFrom<Integer>() &&
        !this->data->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("data"), STR("Core::Data::ListTerm::ListTerm"),
                                     STR("Filters must be of type List, Integer or Reference."));
    }
  } else if (this->terms->isDerivedFrom<Term>()) {
    if (this->data == 0 || (!this->data->isDerivedFrom<List>() &&
                            !this->data->isDerivedFrom<Reference>())) {
      throw InvalidArgumentException(STR("data"), STR("Core::Data::ListTerm::ListTerm"),
                                     STR("Data must be of type List or Reference."));
    }
    if (this->targetRef == 0) {
      throw InvalidArgumentException(STR("ref"), STR("Core::Data::ListTerm::ListTerm"),
                                     STR("Target variable must be provided with data lists."));
    }
  } else {
    throw InvalidArgumentException(STR("term"), STR("Core::Data::ListTerm::ListTerm"),
                                   STR("Provided object is of an invalid type."),
                                   this->terms->getMyTypeInfo()->getUniqueName());
  }
}


//==============================================================================
// Member Functions

/**
 * @param terms A List of Term objects to be used as a static list.
 * @param filter An optional filter object. This can either be an Integer
 *               specifying which term is to be active, a list of Integers
 *               (used as booleans) specifying which term is active and
 *               which term isn't, or a Reference to either.
 */
void ListTerm::setStatic(const SharedPtr<List> &terms, const SharedPtr<IdentifiableObject> &filter)
{
  if (terms == 0) {
    throw InvalidArgumentException(STR("terms"), STR("Core::Data::ListTerm::setStatic"),
                                   STR("Cannot be null."));
  }
  if (filter != 0 && !filter->isA<List>() &&
      !filter->isA<Integer>() &&
      !filter->isA<Reference>()) {
    throw InvalidArgumentException(STR("filter"), STR("Core::Data::ListTerm::setStatic"),
                                   STR("Must be either List, Integer, or a Reference."));
  }
  this->terms = terms;
  this->data = filter;
  this->targetRef.reset();
}


/**
 * Dynamic lists are a list of data objects applied on a single term object.
 * @param term A single term object to receive the data list.
 * @param data A List of data objects, or a Reference to such List.
 * @param var A reference to the stack variable to be used for the loop.
 */
void ListTerm::setDynamic(const SharedPtr<Term> &term, const SharedPtr<IdentifiableObject> &data,
                          const SharedPtr<Reference> &ref)
{
  if (term == 0) {
    throw InvalidArgumentException(STR("term"), STR("Core::Data::ListTerm::setDynamic"),
                                   STR("Cannot be null."));
  }
  if (data != 0 && !data->isA<List>() && !data->isA<Reference>()) {
    throw InvalidArgumentException(STR("data"), STR("Core::Data::ListTerm::setDynamic"),
                                   STR("Must be of type List or Reference."));
  }
  if (ref == 0) {
    throw InvalidArgumentException(STR("ref"), STR("Core::Data::ListTerm::setDynamic"),
                                   STR("Cannot be null."));
  }
  this->terms = term;
  this->data = data;
  this->targetRef = ref;
}


void ListTerm::reset()
{
  this->terms.reset();
  this->data.reset();
  this->targetRef.reset();
}


/**
 * @param index The index of the term to return. If the list contains a
 *              singular term, that term will be returned regardless of this
 *              parameter.
 */
SharedPtr<Term> ListTerm::getTerm(Int index) const
{
  List *list = io_cast<List>(this->terms.get());
  if (list) {
    if (static_cast<Word>(index) >= list->getCount()) {
      throw InvalidArgumentException(STR("index"), STR("Core::Data::ListTerm::getTerm"),
                                     STR("Out of range."));
    }
    SharedPtr<Term> term = list->get(index).s_cast<Term>();
    if (!term->isDerivedFrom<Term>()) {
      throw GeneralException(STR("List contains a non-Term object."), STR("Core::Data::ListTerm::getTerm"));
    }
    return term;
  } else {
    return this->terms.s_cast<Term>();
  }
}


/**
 * This is a reference to the stack variable used as a loop variable by data
 * driven list terms.
 */
void ListTerm::setTargetRef(const SharedPtr<Reference> &ref)
{
  if (!this->isDynamic()) {
    throw GeneralException(STR("Can set target var only for dynamic lists."),
                           STR("Core::Data::ListTerm::setTargetRef"));
  }
  this->targetRef = ref;
}


//==============================================================================
// DataOwner Implementation

void ListTerm::unsetIndexes(Int from, Int to)
{
  if (this->terms != 0) Data::unsetIndexes(this->terms.get(), from, to);
  if (this->data != 0) Data::unsetIndexes(this->data.get(), from, to);
  if (this->targetRef != 0) Data::unsetIndexes(this->targetRef.get(), from, to);
}

} } // namespace
