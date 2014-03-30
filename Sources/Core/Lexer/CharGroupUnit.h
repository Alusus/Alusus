/**
 * @file Core/Lexer/CharGroupUnit.h
 * Contains the header of class Lexer::CharGroupUnit.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CHAR_GROUP_UNIT_H
#define LEXER_CHAR_GROUP_UNIT_H

/**
 * @defgroup lexer_char_group_units Char Group Unit Classes
 * @ingroup lexer
 * @brief Units used to build character groups.
 */

namespace Core { namespace Lexer
{

/**
 * @brief The base of character group unit classes.
 * @ingroup lexer_char_group_units
 *
 * Contains the required abstract function definitions needed for character
 * group unit classes.
 */
class CharGroupUnit
{
  //============================================================================
  // Constructor / Destructor

  public: CharGroupUnit()
  {
  }

  public: virtual ~CharGroupUnit()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual CharGroupUnitType getType() const = 0;

}; // class

} } // namespace

#endif
