/**
 * @file Core/Lexer/TokenDefinition.h
 * Contains the header of class Lexer::TokenDefinition.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_TOKEN_DEFINITION_H
#define LEXER_TOKEN_DEFINITION_H

namespace Core { namespace Lexer
{

/**
 * @brief Represents a single lexer token.
 * @ingroup lexer
 *
 * This class contains the information needed by a single token. This includes
 * the token's identifier as well as the pointer to the token's formula.
 */
class TokenDefinition
{
  //============================================================================
  // Member Variables

  /**
   * @brief The token's identifier
   *
   * This identifier must be unique among all tokens in the lexer.
   */
  private: Word id;

  /**
   * @brief The token's formula head term.
   *
   * This variable points to the term at the root of the formula tree.
   */
  private: SharedPtr<Term> formulaHead;

  /**
   * @brief Specifies whether this is a definition for an internal token.
   *
   * Internal tokens are tokens that come part of other tokens and never
   * represent complete tokens by themselves.
   */
  private: Bool inner;


  //============================================================================
  // Constructors / Destructor

  public: TokenDefinition() : id(UNKNOWN_ID), inner(false)
  {
  }

  /**
   * @brief Initialize the object from the supplied arguments.
   *
   * @param i The id of the token definition. This value should not be 0.
   * @param f A pointer to the token's formula head. This object is the root
   *          of the tree that represent's the token's formula.
   * @param n Specifies whether this definition is for an internal token.
   *          Internal tokens are tokens that come part of other tokens and
   *          never represent complete tokens by themselves.
   */
  public: TokenDefinition(Word i, const SharedPtr<Term> &f, Bool n=false) :
    id(i), formulaHead(f), inner(n)
  {
    // Validation.
    if (i == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("i"), STR("Core::Lexer::TokenDefinition::TokenDefinition"),
                                     STR("An id of 0 is reserved for uknown tokens."));
    }
    if (f == 0) {
      throw InvalidArgumentException(STR("f"), STR("Core::Lexer::TokenDefinition::TokenDefinition"));
    }
  }

  public: ~TokenDefinition()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the token definition's ID.
   *
   * This identifier must be unique among all token definitions in the lexer.
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the id while being in use by some state.
   */
  public: void setId(Word id)
  {
    if (this->id != UNKNOWN_ID) {
      throw GeneralException(STR("Modifying an already set id is not allowed."),
                             STR("Core::Lexer::TokenDefinition::setId"));
    }
    this->id = id;
  }

  /**
   * @brief Get the token definition ID.
   *
   * This identifier is unique among all tokens in the lexer.
   */
  public: Word getId() const
  {
    return this->id;
  }

  /**
   * @brief Set the formula head object.
   *
   * This variable points to the term at the root of the formula tree.
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setFormulaHead(const SharedPtr<Term> &h)
  {
    if (h == 0) {
      throw InvalidArgumentException(STR("h"), STR("Core::Lexer::TokenDefinition::setFormulaHead"));
    }
    if (this->formulaHead != 0) {
      throw GeneralException(STR("Modifying an already set formula head is not allowed."),
                             STR("Core::Lexer::TokenDefinition::setFormulaHead"));
    }
    this->formulaHead = h;
  }

  /**
   * @brief Get the formula head object.
   *
   * This variable points to the term at the root of the formula tree.
   */
  public: const SharedPtr<Term>& getFormulaHead() const
  {
    return this->formulaHead;
  }

  /**
   * @brief Set whether this definition is for an internal token.
   *
   * Internal tokens are tokens that come part of other tokens and never
   * represent complete tokens by themselves.
   */
  public: void setInner(Bool i)
  {
    this->inner = i;
  }

  /**
   * @brief Get whether this definition is for an internal token.
   *
   * Internal tokens are tokens that come part of other tokens and never
   * represent complete tokens by themselves.
   */
  public: Bool isInner() const
  {
    return this->inner;
  }

}; // class

} } // namespace

#endif
