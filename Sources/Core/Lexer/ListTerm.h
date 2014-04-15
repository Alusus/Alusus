/**
 * @file Core/Lexer/ListTerm.h
 * Contains the header of class Core::Lexer::ListTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_LIST_TERM_H
#define LEXER_LIST_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief The base of terms that consists of a list of terms.
 * @ingroup lexer_terms
 *
 * Classes for terms that consist of list of terms can inherit from this class
 * to benefit from the list manipulation functions.
 */
class ListTerm : public Term
{
  //============================================================================
  // Member Variables

  /// A pointer to the head of the child terms link list.
  protected: SharedPtr<Term> childTermsHead;


  //============================================================================
  // Constructor / Destructor

  protected: ListTerm()
  {
  }

  /// Delete all child terms.
  public: virtual ~ListTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// @name List Manipulation Functions
  /// @{

  /// Attach a term to the childs link list.
  public: void attachChildTerm(const SharedPtr<Term> &term);

  /// Attach a list of terms to the childs link list.
  public: void attachChildTerms(Term **terms, Int count);

  /// @}

  /// @name Terms Retrieval Functions
  /// @{

  /// Get the head of the child terms list
  public: const SharedPtr<Term>& getChildTermsHead() const
  {
    return this->childTermsHead;
  }

  /// Get the count of child terms in the list.
  public: Int getChildTermCount() const;

  /// Get a specific child term from the list.
  public: const SharedPtr<Term>& getChildTerm(Int i) const;

  /// @}

}; // class

} } // namespace

#endif
