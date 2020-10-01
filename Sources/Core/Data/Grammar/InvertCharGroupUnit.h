/**
 * @file Core/Data/Grammar/InvertCharGroupUnit.h
 * Contains the header of class Core::Data::Grammar::InvertCharGroupUnit.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_INVERTCHARGROUPUNIT_H
#define CORE_DATA_GRAMMAR_INVERTCHARGROUPUNIT_H

namespace Core::Data::Grammar
{

/**
 * @brief An inversion of a character group unit.
 * @ingroup data_char_group_units
 *
 * Represented the group of characters that is not included in the given
 * character group unit.
 */
class InvertCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Type Info

  TYPE_INFO(InvertCharGroupUnit, CharGroupUnit, "Core.Data.Grammar", "Core", "alusus.org");


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
  public: InvertCharGroupUnit(SharedPtr<CharGroupUnit> const &cgu) : childCharGroupUnit(cgu)
  {
  }

  public: virtual ~InvertCharGroupUnit()
  {
    RESET_OWNED_SHAREDPTR(this->childCharGroupUnit);
  }

  public: static SharedPtr<InvertCharGroupUnit> create(SharedPtr<CharGroupUnit> const &cgu)
  {
    return newSrdObj<InvertCharGroupUnit>(cgu);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the pointer to the child char group unit.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setChildCharGroupUnit(SharedPtr<CharGroupUnit> const &u)
  {
    if (u == 0) {
      throw EXCEPTION(InvalidArgumentException, S("u"), S("Should not be null."));
    }
    if (this->childCharGroupUnit != 0) {
      throw EXCEPTION(GenericException, S("Modifying an already set child char group unit is not allowed."));
    }
    this->childCharGroupUnit = u;
    this->childCharGroupUnit->setOwner(this);
  }

  /// Get a pointer to the char group unit that is to be excluded (inverted).
  public: SharedPtr<CharGroupUnit> const& getChildCharGroupUnit() const
  {
    return this->childCharGroupUnit;
  }

}; // class

} // namespace

#endif
