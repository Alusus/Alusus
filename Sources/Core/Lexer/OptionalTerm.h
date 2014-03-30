/**
 * @file Core/Lexer/OptionalTerm.h
 * Contains the header of class Lexer::OptionalTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_OPTIONAL_TERM_H
#define LEXER_OPTIONAL_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles optional grammar terms.
 * @ingroup lexer_terms
 *
 * This class handles optional terms in grammar formulas. An optional term can
 * appear zero or one time.
 */
class OptionalTerm : public Term
{
  //============================================================================
  // Member Variables

  /// A pointer to the term required to be optional.
  protected: SharedPtr<Term> childTerm;


  //============================================================================
  // Constructors / Destructor

  public: OptionalTerm()
  {
  }

  public: OptionalTerm(const SharedPtr<Term> &t) : childTerm(t)
  {
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::OptionalTerm::OptionalTerm"));
    }
  }

  public: OptionalTerm(Term *t) : childTerm(t)
  {
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::OptionalTerm::OptionalTerm"));
    }
  }

  public: virtual ~OptionalTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::OPTIONAL;
  }

  /**
   * @brief Set the pointer to the child term object.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setChildTerm(const SharedPtr<Term> &t)
  {
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::OptionalTerm::setChildTerm"));
    }
    if (this->childTerm != 0) {
      throw GeneralException(STR("Modifying an already set child term is not allowed."),
                             STR("Core::Lexer::OptionalTerm::setChildTerm"));
    }
    this->childTerm = t;
  }

  /// Get the pointer to the child term object.
  public: const SharedPtr<Term>& getChildTerm() const
  {
    return this->childTerm;
  }

}; // class

} } // namespace

#endif
