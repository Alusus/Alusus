/**
 * @file Core/Data/UnionCharGroupUnit.h
 * Contains the header of class Core::Data::UnionCharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_UNIONCHARGROUPUNIT_H
#define CORE_DATA_UNIONCHARGROUPUNIT_H

namespace Core { namespace Data
{

/**
 * @brief A union of character group units.
 * @ingroup data_char_group_units
 *
 * Represents the union of multiple character group units.
 */
class UnionCharGroupUnit : public CharGroupUnit
{
  //============================================================================
  // Type Info

  TYPE_INFO(UnionCharGroupUnit, CharGroupUnit, "Core.Data", "Core", "alusus.net");


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

  public: UnionCharGroupUnit(std::initializer_list<SharedPtr<CharGroupUnit>> const &args);

  public: virtual ~UnionCharGroupUnit()
  {
    for (auto unit : this->charGroupUnits) RESET_OWNED_SHAREDPTR(unit);
  }

  public: static SharedPtr<UnionCharGroupUnit> create(std::initializer_list<SharedPtr<CharGroupUnit>> const &args)
  {
    return std::make_shared<UnionCharGroupUnit>(args);
  }


  //============================================================================
  // Member Functions

  /// Add a char group unit to the list.
  public: void addCharGroupUnit(const SharedPtr<CharGroupUnit> &u)
  {
    this->charGroupUnits.push_back(u);
    if (u != 0) u->setOwner(this);
  }

  /// Get a pointer to the vector of char groups that forms the union.
  public: std::vector<SharedPtr<CharGroupUnit> >* getCharGroupUnits()
  {
    return &this->charGroupUnits;
  }

}; // class

} } // namespace

#endif
