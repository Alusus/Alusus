/**
 * @file Core/Data/RandomCharGroupUnit.h
 * Contains the header of class Core::Data::RandomCharGroupUnit.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_RANDOMCHARGROUPUNIT_H
#define CORE_DATA_RANDOMCHARGROUPUNIT_H

namespace Core { namespace Data
{

/**
 * @brief A random character group unit.
 * @ingroup data_char_group_units
 *
 * Represents a randomly selected list of characters.
 */
class RandomCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Type Info

  TYPE_INFO(RandomCharGroupUnit, CharGroupUnit, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /**
   * @brief A string containing the list of characters in the group.
   *
   * The string is only used as an array of characters and do not actually
   * represent a string.
   */
  private: WStr charList;


  //============================================================================
  // Constructor / Destructor

  public: RandomCharGroupUnit(Char const *list=0) : charList(list)
  {
  }

  public: virtual ~RandomCharGroupUnit()
  {
  }

  public: static SharedPtr<RandomCharGroupUnit> create(Char const *list=0)
  {
    return std::make_shared<RandomCharGroupUnit>(list);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set a pointer to the list of characters in the group.
   *
   * Once set, modifying this value is not allowed. This is to prevent
   * accidentally modifying the term while being in use by some state.
   */
  public: void setCharList(Char const *list)
  {
    if (list == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("list"), STR("list is null."));
    }
    if (this->charList.size() != 0) {
      throw EXCEPTION(GenericException, STR("Modifying an already set char list is not allowed."));
    }
    this->charList = list;
  }

  /**
   * @brief Get a pointer to the list of characters in the group.
   * @return Returns a pointer to an array of characters.
   */
  public: WChar const* getCharList() const
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
