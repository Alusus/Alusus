/**
 * @file Core/Lexer/ReferenceTerm.h
 * Contains the header of class Core::Lexer::ReferenceTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_REFERENCE_TERM_H
#define LEXER_REFERENCE_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles reference terms.
 * @ingroup lexer_terms
 *
 * This class handles reference terms in grammar formulas. Reference terms are
 * used to refer to other grammar formulas (other token definitions).
 */
class ReferenceTerm : public Term
{
  //============================================================================
  // Member Variables

  /// The id of the referenced token definition.
  private: Word referenceId;

  /**
   * @brief A pointer to the referenced formula root.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the formla head of the token definition whose id is referenced
   * by this object. This will save the state machine from searching by ID
   * every time it uses this object.
   */
  private: Term *referenceHead;


  //============================================================================
  // Constructors / Destructor

  public: ReferenceTerm()
  {
    this->referenceId = UNKNOWN_ID;
    this->referenceHead = 0;
  }

  /**
   * @brief Initialize the object with supplied values.
   *
   * @param id The id of the referenced production definition. This value
   *           should not be 0.
   * @param head A pointer to the actual term object to be referenced. This
   *             value is stored for caching purposes to save the state
   *             machine having to search by id each time it uses this object.
   */
  public: ReferenceTerm(Word id, Term *head=0) : referenceId(id), referenceHead(head)
  {
    if (id == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("id"), STR("Core::Lexer::ReferenceTerm::ReferenceTerm"));
    }
  }

  public: virtual ~ReferenceTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::REFERENCE;
  }

  /**
   * @brief Set the id of the referenced production definition.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the reference id while being in use by some state.
   */
  public: void setReferenceId(Word id)
  {
    if (id == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("id"), STR("Core::Lexer::ReferenceTerm::setReferenceId"));
    }
    if (this->referenceId != UNKNOWN_ID) {
      throw GeneralException(STR("Modifying an already set reference id is not allowed."),
                             STR("Core::Lexer::ReferenceTerm::setReferenceId"));
    }
    this->referenceId = id;
  }

  /// Get the id of the referenced token definition.
  public: Word getReferenceId() const
  {
    return this->referenceId;
  }

  /**
   * @brief Set the pointer to the referenced formula head.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the formla head of the token definition whose id is referenced
   * by this object. This will save the state machine from searching by ID
   * every time it uses this object.<br>
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   *
   * @param h The pointer to the formula head of the referenced token
   *            definition.
   */
  public: void setReferenceHead(Term *h)
  {
    this->referenceHead = h;
  }

  /**
   * @brief Get the pointer to the referenced formula head.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the formla head of the token definition whose id is referenced
   * by this object. This will save the state machine from searching by ID
   * every time it uses this object.
   *
   * @return The pointer to the formula head of the referenced token
   *           definition.
   */
  public: Term* getReferenceHead() const
  {
    return this->referenceHead;
  }

}; // class

} } // namespace

#endif
