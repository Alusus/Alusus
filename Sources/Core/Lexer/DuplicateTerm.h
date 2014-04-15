/**
 * @file Core/Lexer/DuplicateTerm.h
 * Contains the header of class Core::Lexer::DuplicateTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_DUPLICATE_TERM_H
#define LEXER_DUPLICATE_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles duplicate grammar terms.
 * @ingroup lexer_terms
 *
 * This class handles duplicate terms in grammar formulas. Duplicate terms can
 * appear for an undetermined number of times. However, this class allows some
 * restrictions to be applied like forcing the term to appear at least once,
 * for example.
 */
class DuplicateTerm : public Term
{
  //============================================================================
  // Member Variables

  /// A pointer to the term required to be duplicated.
  private: SharedPtr<Term> childTerm;

  /**
   * @brief The minimum number of occurances required for the duplicate term.
   *
   * The duplicate term should appear at least a number of time equivalent to
   * this value before it's accepted.
   */
  private: Int minOccurances;

  /**
   * @brief The maximum number of occurances allowed for the duplicate term.
   *
   * The duplicate term can appear no more than this number of times. If this
   * value is 0, the number of occurances is unlimited.
   */
  private: Int maxOccurances;


  //============================================================================
  // Constructors / Destructor

  public: DuplicateTerm() : minOccurances(0), maxOccurances(0)
  {
  }

  /**
   * @brief Initializes the object's data.
   *
   * @param t A pointer to the term required to be duplicated.
   * @param mno The minimum number of occurances required for the duplicate
   *            term. The duplicate term has to appear at least a number of
   *            times equivalent to this value before it can be accepted.
   * @param mxo The maximum number of occurances allowed for the duplicate
   *            term. The duplicate term can appear no more than a number of
   *            times equivalent to this value. If this value is 0 the maximum
   *            occurances is unlimited.
   */
  public: DuplicateTerm(const SharedPtr<Term> &t, Int mno=0, Int mxo=0) :
    childTerm(t), minOccurances(mno), maxOccurances(mxo)
  {
    // Validation.
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"));
    }
    if (mno < 0) {
      throw InvalidArgumentException(STR("mno"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"),
                                     STR("mno should be >= 0."));
    }
    if (mxo < mno && mxo != 0) {
      throw InvalidArgumentException(STR("mno"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"),
                                     STR("mxo should be 0 or >= mno."));
    }
  }

  /**
   * @brief Initializes the object's data.
   *
   * @param t A pointer to the term required to be duplicated.
   * @param mno The minimum number of occurances required for the duplicate
   *            term. The duplicate term has to appear at least a number of
   *            times equivalent to this value before it can be accepted.
   * @param mxo The maximum number of occurances allowed for the duplicate
   *            term. The duplicate term can appear no more than a number of
   *            times equivalent to this value. If this value is 0 the maximum
   *            occurances is unlimited.
   */
  public: DuplicateTerm(Term *t, Int mno=0, Int mxo=0) :
    childTerm(t), minOccurances(mno), maxOccurances(mxo)
  {
    // Validation.
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"));
    }
    if (mno < 0) {
      throw InvalidArgumentException(STR("mno"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"),
                                     STR("mno should be >= 0."));
    }
    if (mxo < mno && mxo != 0) {
      throw InvalidArgumentException(STR("mno"), STR("Core::Lexer::DuplicateTerm::DuplicateTerm"),
                                     STR("mxo should be 0 or >= mno."));
    }
  }

  public: virtual ~DuplicateTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::DUPLICATE;
  }

  /**
   * @brief Set the term to be duplicated.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setChildTerm(const SharedPtr<Term> &t)
  {
    if (this->childTerm != 0) {
      throw GeneralException(STR("Modifying an already set child term is not allowed."),
                             STR("Core::Lexer::DuplicateTerm::setChildTerm"));
    }
    this->childTerm = t;
  }

  /// Get a pointer to the term to be duplicated.
  public: const SharedPtr<Term>& getChildTerm() const
  {
    return this->childTerm;
  }

  /**
   * @brief Set the minimum number of occurances required for the term.
   *
   * The duplicate term should appear at least a number of time equivalent to
   * this value before it's accepted.
   */
  public: void setMinOccurances(Int m)
  {
    if (m < 0) {
      throw InvalidArgumentException(STR("m"), STR("Core::Lexer::DuplicateTerm::setMinOccurances"),
                                     STR("m should be >= 0."));
    }
    this->minOccurances = m;
  }

  /**
   * @brief Get the minimum number of occurances required for the term.
   *
   * The duplicate term should appear at least a number of time equivalent to
   * this value before it's accepted.
   */
  public: Int getMinOccurances() const
  {
    return this->minOccurances;
  }

  /**
   * @brief Set the maximum number of occurances allowed.
   *
   * The duplicate term can appear no more than this number of times. If this
   * value is 0, the number of occurances is unlimited.
   */
  public: void setMaxOccurances(Int m)
  {
    if (m < 0) {
      throw InvalidArgumentException(STR("m"), STR("Core::Lexer::DuplicateTerm::setMinOccurances"),
                                     STR("m should be >= 0."));
    }
    if (m < this->minOccurances && m != 0) {
      throw InvalidArgumentException(STR("m"), STR("Core::Lexer::DuplicateTerm::setMinOccurances"),
                                     STR("Max occurances should not be smaller than min occurances."));
    }
    this->maxOccurances = m;
  }

  /**
   * @brief Get the maximum number of occurances allowed.
   *
   * The duplicate term can appear no more than this number of times. If this
   * value is 0, the number of occurances is unlimited.
   */
  public: Int getMaxOccurances() const
  {
    return this->maxOccurances;
  }

}; // class

} } // namespace

#endif
