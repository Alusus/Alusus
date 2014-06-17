/**
 * @file Core/Lexer/RandomCharGroupUnit.h
 * Contains the header of class Core::Lexer::RandomCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_RANDOM_CHAR_GROUP_UNIT_H
#define LEXER_RANDOM_CHAR_GROUP_UNIT_H

namespace Core { namespace Lexer
{

/**
 * @brief A random character group unit.
 * @ingroup lexer_char_group_units
 *
 * Represents a randomly selected list of characters.
 */
class RandomCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Member Variables

  /**
   * @brief A string containing the list of characters in the group.
   *
   * The string is only used as an array of characters and do not actually
   * represent a string.
   */
  private: Str charList;


  //============================================================================
  // Constructor / Destructor

  public: RandomCharGroupUnit()
  {
  }

  public: RandomCharGroupUnit(Char const *list)
  {
    if (list == 0) {
      throw InvalidArgumentException(STR("list"), STR("Core::Lexer::RandomCharGroupUnit::RandomCharGroupUnit"),
                                     STR("list is null."));
    }
    this->charList = list;
  }

  public: virtual ~RandomCharGroupUnit()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual CharGroupUnitType getType() const
  {
    return CharGroupUnitType::RANDOM;
  }

  /**
   * @brief Set a pointer to the list of characters in the group.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setCharList(Char const *list)
  {
    if (list == 0) {
      throw InvalidArgumentException(STR("list"), STR("Core::Lexer::RandomCharGroupUnit::setCharList"),
                                     STR("list is null."));
    }
    if (this->charList.size() != 0) {
      throw GeneralException(STR("Modifying an already set char list is not allowed."),
                             STR("Core::Lexer::InvertCharGroupUnit::setCharList"));
    }
    this->charList = list;
  }

  /**
   * @brief Get a pointer to the list of characters in the group.
   * @return Returns a pointer to an array of characters.
   */
  public: Char const* getCharList() const
  {
    return this->charList.c_str();
  }

  /// Get the size of the character list returned by getCharList.
  public: Int getCharListSize() const
  {
    return this->charList.size();
  }

}; // class

} } // namespace

#endif
