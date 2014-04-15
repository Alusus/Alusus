/**
 * @file Core/Lexer/CharGroupDefinition.h
 * Contains the header of class Core::Lexer::CharGroupDefinition.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CHAR_GROUP_DEFINITION_H
#define LEXER_CHAR_GROUP_DEFINITION_H

namespace Core { namespace Lexer
{

/**
 * @brief Represents a single char group definition.
 * @ingroup lexer
 *
 * This class contains the definition of a single character group. The
 * definition includes the CharGroupUnit tree and the identifier of the char
 * group.
 */
class CharGroupDefinition
{
  //============================================================================
  // Member Variables

  /**
   * @brief The char group's identifier
   *
   * This identifier must be unique among all char groups definitions in the
   * lexer.
   */
  private: Word id;

  /**
   * @brief The char group object.
   *
   * This object is the root of the tree that defines the character group.
   */
  private: SharedPtr<CharGroupUnit> charGroupUnit;


  //============================================================================
  // Constructors / Destructor

  public: CharGroupDefinition()
  {
    this->id = UNKNOWN_ID;
  }

  /**
     * @brief Initialize the object.
     *
     * @param i The char group's identifier. This value must not be 0.
     * @param u A pointer to the CharGroupUnit object representing the formula
     *          of this character group definition.
     */
  public: CharGroupDefinition(Word i, const SharedPtr<CharGroupUnit> &u) :
    id(i), charGroupUnit(u)
  {
    if (i == 0) {
      throw InvalidArgumentException(STR("i"), STR("Core::Lexer::CharGroupDefinition::CharGroupDefinition"),
                                     STR("The id should not be UKNOWN_ID."));
    }
    if (u == 0) {
      throw InvalidArgumentException(STR("u"), STR("Core::Lexer::CharGroupDefinition::CharGroupDefinition"));
    }
  }

  public: ~CharGroupDefinition()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the char group's ID.
   *
   * This identifier must be unique among all char groups in the larser.
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the id while being in use by some state.
   */
  public: void setId(Word id)
  {
    if (this->id != UNKNOWN_ID) {
      throw GeneralException(STR("Modifying an already set id is not allowed."),
                             STR("Core::Lexer::CharGroupDefinition::setId"));
    }
    this->id = id;
  }

  /**
   * @brief Get the ID of this char group definition.
   *
   * If this value is 0, then this object is not initialized. A value of 0 is
   * reserved for unknown character group definitions.
   */
  public: Word getId() const
  {
    return this->id;
  }

  /**
   * @brief Set the formula head object.
   *
   * This variable points to the unit at the root of the formula tree.
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setCharGroupUnit(const SharedPtr<CharGroupUnit> &u)
  {
    if (u == 0) {
      throw InvalidArgumentException(STR("h"), STR("Core::Lexer::CharGroupDefinition::setCharGroupUnit"));
    }
    if (this->charGroupUnit != 0) {
      throw GeneralException(STR("Modifying an already set char group unit is not allowed."),
                             STR("Core::Lexer::CharGroupDefinition::setCharGroupUnit"));
    }
    this->charGroupUnit = u;
  }

  /// Get the formula head object.
  public: const SharedPtr<CharGroupUnit>& getCharGroupUnit() const
  {
    return this->charGroupUnit;
  }

};

} } // namespace

#endif
