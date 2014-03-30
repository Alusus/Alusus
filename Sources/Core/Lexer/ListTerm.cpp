/**
 * @file Core/Lexer/ListTerm.cpp
 * Contains the implementation of Lexer::ListTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Lexer
{

//==============================================================================
// Member Functions

/**
 * @details Attach a term object to the child terms link list.
 * @param term A pointer to the term object to be attached.
 */
void ListTerm::attachChildTerm(const SharedPtr<Term> &term)
{
  // Validation.
  if (term == 0) {
    throw InvalidArgumentException(STR("term"), STR("Core::Lexer::ListTerm::attach_term"));
  }
  // Attach the new term.
  if (this->childTermsHead != 0) {
    // We don't need smart pointers while moving through the link list, so we'll use regular pointers to
    // improve performance.
    Term * t = this->childTermsHead.get();
    while (t->getNextTerm().get()) t = t->getNextTerm().get();
    t->setNextTerm(term);
  } else {
    this->childTermsHead = term;
  }
}


/**
 * Attach a list of terms to the child terms link list.
 *
 * @param count The count of elements in "terms".
 * @param terms The array of pointers to terms to be added.
 */
void ListTerm::attachChildTerms(Term **terms, Int count)
{
  // Validation.
  if (count <= 0) {
    throw InvalidArgumentException(STR("count"), STR("Core::Lexer::ListTerm::attachChildTerms"),
                                   STR("Terms count should be > 0."));
  }
  if (terms == 0) {
    throw InvalidArgumentException(STR("terms"), STR("Core::Lexer::ListTerm::attachChildTerms"),
                                   STR("Provided terms array is null."));
  }
  if (terms[0] == 0) {
    throw InvalidArgumentException(STR("terms"), STR("Core::Lexer::ListTerm::attachChildTerms"),
                                   STR("Provided terms array has null element(s)."));
  }
  // Link the terms to each other first.
  for (Int i = 0; i < count-1; i++) {
    // Validate the terms element.
    if (terms[i+1] == 0) {
      throw InvalidArgumentException(STR("terms"), STR("Core::Lexer::ListTerm::attachChildTerms"),
                                     STR("Provided terms array has null element(s)."));
    }
    // Link the term.
    terms[i]->setNextTerm(SharedPtr<Term>(terms[i+1]));
  }
  // Attach the chain by attaching the head.
  this->attachChildTerm(SharedPtr<Term>(terms[0]));
}


/**
 * Count the number of terms attached to this childs linked list.
 */
Int ListTerm::getChildTermCount() const
{
  Int count = 0;
  // We don't need smart pointers while moving through the link list, so we'll use regular pointers to
  // improve performance.
  Term * term = this->childTermsHead.get();
  while (term) {
    count++;
    term = term->getNextTerm().get();
  }
  return count;
}


/**
 * Get the term with the specified index from the childs link list.
 */
const SharedPtr<Term>& ListTerm::getChildTerm(Int i) const
{
  if (i == 0) return this->childTermsHead;
  // We don't need smart pointers while moving through the link list, so we'll use regular pointers to
  // improve performance.
  Term *term = this->childTermsHead.get();
  // We are using regular pointer but we want to return the smart pointer itself, so we'll do the loop
  // in a way that term is used to loop through terms but we return term->getNextTerm() instead of
  // term itself.
  --i;
  while (i > 0) {
    ASSERT(term != 0);
    term = term->getNextTerm().get();
    --i;
  }
  ASSERT(term->getNextTerm() != 0);
  return term->getNextTerm();
}

} } // namespace
