/**
 * @file Core/Lexer/Term.h
 * Contains the header of class Lexer::Term.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_TERM_H
#define LEXER_TERM_H

/**
 * @defgroup lexer_terms Term Classes
 * @ingroup lexer
 * @brief Terms of production formulas.
 */

namespace Core { namespace Lexer
{

/**
 * @brief The base of grammar term classes.
 * @ingroup lexer_terms
 *
 * The base class of all grammar term classes. Term classes are used to
 * construct a grammar formula.
 */
class Term
{
  //============================================================================
  // Member Variables

  /// A pointer to the next term in the link list.
  private: SharedPtr<Term> nextTerm;


  //============================================================================
  // Constructor / Destructor

  protected: Term()
  {
  }

  public: virtual ~Term()
  {
  }


  //============================================================================
  // Member Functions

  /// Set the next term in the link list.
  public: void setNextTerm(const SharedPtr<Term> &term)
  {
    this->nextTerm = term;
  }

  /// Get the pointer to the next term in the link list.
  public: const SharedPtr<Term>& getNextTerm() const
  {
    return this->nextTerm;
  }

  /// Get the type identifier of this object.
  public: virtual TermType getType() const = 0;

}; // class

} } // namespace

#endif
