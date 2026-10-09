/**
 * @file Core/Grammar/CharGroupUnit.h
 * Contains the header of class Core::Grammar::CharGroupUnit.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_CHARGROUPUNIT_H
#define CORE_GRAMMAR_CHARGROUPUNIT_H

/**
 * @defgroup data_char_group_units Char Group Unit Classes
 * @ingroup core_data_grammar
 * @brief Units used to build character groups.
 */

namespace Core::Grammar
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

  TYPE_INFO(CharGroupUnit, Node, "Core.Grammar", "Core", "alusus.org");


  //============================================================================
  // Constructor / Destructor

  public: CharGroupUnit()
  {
  }

  public: virtual ~CharGroupUnit()
  {
  }

}; // class

} // namespace

#endif
