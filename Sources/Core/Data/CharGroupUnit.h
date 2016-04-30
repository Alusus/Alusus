/**
 * @file Core/Data/CharGroupUnit.h
 * Contains the header of class Core::Data::CharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CHARGROUPUNIT_H
#define CORE_DATA_CHARGROUPUNIT_H

/**
 * @defgroup data_char_group_units Char Group Unit Classes
 * @ingroup data
 * @brief Units used to build character groups.
 */

namespace Core { namespace Data
{

/**
 * @brief The base of character group unit classes.
 * @ingroup data_char_group_units
 *
 * Contains the required abstract function definitions needed for character
 * group unit classes.
 */
class CharGroupUnit : public Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(CharGroupUnit, Node, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Constructor / Destructor

  public: CharGroupUnit()
  {
  }

  public: virtual ~CharGroupUnit()
  {
  }

}; // class

} } // namespace

#endif
