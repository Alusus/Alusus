/**
 * @file Core/Data/MultiplyTerm.cpp
 * Contains the implementation of class Core::Data::MultiplyTerm.
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
// Constructor & Destructor

/**
 * Initialize the object.
 * @param p An object specifying the priority of the inner branch. This can
 *          either be the Integer priority value, or a Reference to it.
 *          The value can either be 1, which means priority is for taking
 *          (or staying) in the branch, or 0 which means priority is to
 *          leave the branch.
 * @param f Parsing flags to be passed to Term's constructor.
 * @param min The minimum number of occurances. This must be >= 0. If null
 *            is specified, the default value of 0 will be considered. This
 *            value must be either Integer or Raference.
 * @param max The maximum number of occurances. This must be >= 0. If null is
 *            specified, the max occurances will be unlimited. This value must
 *            be null, Integer or Reference.
 * @param t The head of the child branch.
 */
MultiplyTerm::MultiplyTerm(SharedPtr<IdentifiableObject> const &p, Word f,
                           SharedPtr<IdentifiableObject> const &min,
                           SharedPtr<IdentifiableObject> const &max,
                           const SharedPtr<Term> &t) :
  priority(p), minOccurances(min), maxOccurances(max), term(t), Term(f)
{
  if (p != 0 && !p->isA<Integer>() && !p->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("p"),
                    STR("Must be of type Integer or Reference."),
                    p->getMyTypeInfo()->getUniqueName());
  }
  if (min != 0 && !min->isA<Integer>() && !min->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("min"),
                    STR("Must be of type Integer or Reference."),
                    min->getMyTypeInfo()->getUniqueName());
  }
  if (max != 0 && !max->isA<Integer>() && !max->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("max"),
                    STR("Must be of type Integer or Reference."),
                    max->getMyTypeInfo()->getUniqueName());
  }
}


MultiplyTerm::MultiplyTerm(const std::initializer_list<Argument<TermElement>> &args)
{
  for (auto arg : args) {
    switch (arg.id.val) {
      case TermElement::FLAGS:
        this->flags = arg.intVal;
        break;
      case TermElement::PRIORITY:
        this->priority = arg.ioVal;
        break;
      case TermElement::MIN:
        this->minOccurances = arg.ioVal;
        break;
      case TermElement::MAX:
        this->maxOccurances = arg.ioVal;
        break;
      case TermElement::TERM:
        this->term = arg.ioVal.io_cast<Term>();
        if (this->term == 0 && arg.ioVal != 0) {
          throw EXCEPTION(InvalidArgumentException, STR("term"),
                          STR("Term value must be of type Term."),
                          arg.ioVal->getMyTypeInfo()->getUniqueName());
        }
        break;
    }
  }
  if (this->priority != 0 && !this->priority->isA<Integer>() && !this->priority->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("priority"),
                    STR("Must be of type Integer or Reference."),
                    this->priority->getMyTypeInfo()->getUniqueName());
  }
  if (this->minOccurances != 0 && !this->minOccurances->isA<Integer>() &&
      !this->minOccurances->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("min"),
                    STR("Must be of type Integer or Reference."),
                    this->minOccurances->getMyTypeInfo()->getUniqueName());
  }
  if (this->maxOccurances != 0 && !this->maxOccurances->isA<Integer>() &&
      !this->maxOccurances->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("max"),
                    STR("Must be of type Integer or Reference."),
                    this->maxOccurances->getMyTypeInfo()->getUniqueName());
  }
}


//==============================================================================
// Member Functions

/**
 * Set the minimum number of occurances the child path is allowed to have.
 * This must be >= 0. If null is specified, the default value of 0 will be
 * considered. This value must be either Integer or Reference.
 */
void MultiplyTerm::setMinOccurances(SharedPtr<IdentifiableObject> const &min)
{
  if (min != 0 && min->isA<Integer>() && !min->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("min"),
                    STR("Must be of type Integer or Reference."),
                    min->getMyTypeInfo()->getUniqueName());
  }
  this->minOccurances = min;
}


/**
 * Set the maximum number of occurances. This must be >= 0. If null is specified
 * the number of occurances will be unlimited. This value must be null, Integer
 * or Reference.
 */
void MultiplyTerm::setMaxOccurances(SharedPtr<IdentifiableObject> const &max)
{
  if (max != 0 && max->isA<Integer>() && !max->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("max"),
                    STR("Must be of type Integer or Reference."),
                    max->getMyTypeInfo()->getUniqueName());
  }
  this->maxOccurances = max;
}


/**
 * Set the priority of the inner branch. This can either be the Integer priority
 * value, or a Reference to it. The value can either be 1, which means priority
 * is for taking (or staying) in the branch, or 0 which means priority is to
 * leave the branch. If null is specified the default value of 1 is considered.
 */
void MultiplyTerm::setPriority(SharedPtr<IdentifiableObject> const &p)
{
  if (p != 0 && p->isA<Integer>() && !p->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("p"),
                    STR("Must be of type Integer or Reference."),
                    p->getMyTypeInfo()->getUniqueName());
  }
  this->priority = p;
}


//==============================================================================
// DataOwner Implementation

void MultiplyTerm::unsetIndexes(Int from, Int to)
{
  if (this->term != 0) Data::unsetIndexes(this->term.get(), from, to);
  if (this->minOccurances != 0) Data::unsetIndexes(this->minOccurances.get(), from, to);
  if (this->maxOccurances != 0) Data::unsetIndexes(this->maxOccurances.get(), from, to);
  if (this->priority != 0) Data::unsetIndexes(this->priority.get(), from, to);
}

} } // namespace
