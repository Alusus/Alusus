/**
 * @file Core/Lexer/InvertCharGroupUnit.h
 * Contains the header of class Core::Lexer::InvertCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_INVERT_CHAR_GROUP_UNIT_H
#define LEXER_INVERT_CHAR_GROUP_UNIT_H

namespace Core { namespace Lexer
{

/**
 * @brief An inversion of a character group unit.
 * @ingroup lexer_char_group_units
 *
 * Represented the group of characters that is not included in the given
 * character group unit.
 */
class InvertCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Member Variables

  /**
   * @brief The character group that is to be excluded.
   *
   * This group represents all the characters that are intended to be
   * excluded.
   */
  private: SharedPtr<CharGroupUnit> childCharGroupUnit;


  //============================================================================
  // Constructor / Destructor

  public: InvertCharGroupUnit()
  {
  }

  /**
   * @brief Initialize the object.
   *
   * @param cgu The character group that is to be excluded (inverted).
   */
  public: InvertCharGroupUnit(CharGroupUnit *cgu) : childCharGroupUnit(cgu)
  {
  }

  public: virtual ~InvertCharGroupUnit()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual CharGroupUnitType getType() const
  {
    return CharGroupUnitType::INVERT;
  }

  /**
   * @brief Set the pointer to the child char group unit.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setChildCharGroupUnit(const SharedPtr<CharGroupUnit> &u)
  {
    if (u == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Lexer::InvertCharGroupUnit::setChildCharGroupUnit"));
    }
    if (this->childCharGroupUnit != 0) {
      throw GeneralException(STR("Modifying an already set child char group unit is not allowed."),
                             STR("Core::Lexer::InvertCharGroupUnit::setChildCharGroupUnit"));
    }
    this->childCharGroupUnit = u;
  }

  /// Get a pointer to the char group unit that is to be excluded (inverted).
  public: const SharedPtr<CharGroupUnit>& getChildCharGroupUnit() const
  {
    return this->childCharGroupUnit;
  }

}; // class

} } // namespace

#endif
