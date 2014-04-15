/**
 * @file Core/Lexer/CharGroupTerm.h
 * Contains the header of class Core::Lexer::CharGroupTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CHAR_GROUP_TERM_H
#define LEXER_CHAR_GROUP_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles character group grammar terms.
 * @ingroup lexer_terms
 *
 * This class handles instances of character groups in grammar formulas.
 */
class CharGroupTerm : public Term
{
  //============================================================================
  // Member Variables

  /// The id of the character group used to match inputs.
  private: Word charGroupId;

  /**
   * @brief A pointer to the character group unit object used to match inputs.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the character group unit whose id is referenced by this object.
   * This will save the state machine from searching by ID every time it
   * uses this object.
   */
  private: SharedPtr<CharGroupUnit> charGroupUnit;


  //============================================================================
  // Constructor / Destructor

  public: CharGroupTerm() : charGroupId(UNKNOWN_ID)
  {
  }

  /**
   * @brief Initialize the object.
   * @param id The id of the character group. This value must not be 0.
   */
  public: CharGroupTerm(Word id) : charGroupId(id)
  {
    if (id == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupTerm::CharGroupTerm"));
    }
  }

  /**
   * @brief Initialize the object.
   * @param id The id of the character group. This value must not be 0.
   * @param cgu A pointer to the character group unit object represented by
   *            id. This value is set for caching purposes to save the state
   *            machine having to search by id every time it uses this object.
   */
  public: CharGroupTerm(Word id, const SharedPtr<CharGroupUnit> &cgu) :
    charGroupId(id), charGroupUnit(cgu)
  {
    if (id == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupTerm::CharGroupTerm"));
    }
    if (cgu == 0) {
      throw InvalidArgumentException(STR("cgu"), STR("Core::Lexer::CharGroupTerm::CharGroupTerm"));
    }
  }

  public: virtual ~CharGroupTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::CHAR_GROUP;
  }

  /**
   * @brief Set the id of the referenced character group definition.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the id while being in use by some state.
   */
  public: void setCharGroupId(Word id)
  {
    if (id == UNKNOWN_ID) {
      throw InvalidArgumentException(STR("id"), STR("Core::Lexer::CharGroupTerm::setCharGroupId"));
    }
    if (this->charGroupId != UNKNOWN_ID) {
      throw GeneralException(STR("Modifying an already set reference id is not allowed."),
                             STR("Core::Lexer::CharGroupTerm::setCharGroupId"));
    }
    this->charGroupId = id;
  }

  /// Get the id of the referenced character group definition.
  public: Word getCharGroupId() const
  {
    return this->charGroupId;
  }

  /**
   * @brief Set the pointer to the character group unit object.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the character group unit whose id is referenced by this
   * object. This will save the state machine from searching by ID every time
   * it uses this object.<br>
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   *
   * @param cgu The pointer to the character group unit object referenced by
   *            this object.
   */
  public: void setCharGroupUnit(const SharedPtr<CharGroupUnit> &cgu)
  {
    if (cgu == 0) {
      throw InvalidArgumentException(STR("cgu"), STR("Core::Lexer::CharGroupTerm::setCharGroupUnit"));
    }
    if (this->charGroupUnit != 0) {
      throw GeneralException(STR("Modifying an already set char group unit is not allowed."),
                             STR("Core::Lexer::CharGroupTerm::setCharGroupUnit"));
    }
    this->charGroupUnit = cgu;
  }

  /**
   * @brief Get the pointer to the character group unit object.
   *
   * This pointer is set for caching purposes. It's purpose is to hold the
   * pointer to the character group unit whose id is referenced by this
   * object. This will save the state machine from searching by ID every time
   * it uses this object.
   *
   * @return The pointer to the character group unit object referenced by this
   *         object.
   */
  public: const SharedPtr<CharGroupUnit>& getCharGroupUnit() const
  {
    return this->charGroupUnit;
  }

}; // class

} } // namespace

#endif
