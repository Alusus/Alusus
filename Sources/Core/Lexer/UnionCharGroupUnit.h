/**
 * @file Core/Lexer/UnionCharGroupUnit.h
 * Contains the header of class Core::Lexer::UnionCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_UNION_CHAR_GROUP_UNIT_H
#define LEXER_UNION_CHAR_GROUP_UNIT_H

namespace Core { namespace Lexer
{

/**
 * @brief A union of character group units.
 * @ingroup lexer_char_group_units
 *
 * Represents the union of multiple character group units.
 */
class UnionCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Member Variables

  /**
   * @brief The list of character groups in the union.
   *
   * This list contains pointers to the character group objects that forms
   * the union.
   */
  private: std::vector<SharedPtr<CharGroupUnit> > charGroupUnits;


  //============================================================================
  // Constructor / Destructor

  public: UnionCharGroupUnit()
  {
  }

  public: UnionCharGroupUnit(Int count, ...);

  public: virtual ~UnionCharGroupUnit()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual CharGroupUnitType getType() const
  {
    return CharGroupUnitType::UNION;
  }

  /// Add a char group unit to the list.
  public: void addCharGroupUnit(const SharedPtr<CharGroupUnit> &u)
  {
    this->charGroupUnits.push_back(u);
  }

  /// Get a pointer to the vector of char groups that forms the union.
  public: std::vector<SharedPtr<CharGroupUnit> >* getCharGroupUnits()
  {
    return &this->charGroupUnits;
  }

}; // class

} } // namespace

#endif
