/**
 * @file Core/Data/Grammar/ConstTerm.h
 * Contains the header of class Core::Data::Grammar::ConstTerm.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_CONSTTERM_H
#define CORE_DATA_GRAMMAR_CONSTTERM_H

namespace Core::Data::Grammar
{

/**
 * @brief Handles constant grammar terms.
 * @ingroup core_data_grammar
 *
 * This class handles constant terms in grammar formulas. Constant terms
 * are used to check for exact matches of strings.
 */
class ConstTerm : public Term
{
  //============================================================================
  // Type Info

  TYPE_INFO(ConstTerm, Term, "Core.Data.Grammar", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  /// A pointer to the string constant to be matched with input characters.
  private: TiWStr matchString;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (matchString, TiWStr, VALUE, setMatchString(value), &matchString)
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ConstTerm);

  IMPLEMENT_ATTR_CONSTRUCTOR(ConstTerm);

  public: virtual ~ConstTerm()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the match string.
   *
   * Once a match string is set, modifying (or setting) this value is not
   * allowed. This is to prevent accidentally modifying it while being in use
   * by some state.
   */
  public: void setMatchString(Char const *str)
  {
    if (str == 0) {
      throw EXCEPTION(InvalidArgumentException, S("str"), S("Argument should not be null."), str);
    }
    if (this->matchString.getWStr().size() > 0) {
      throw EXCEPTION(GenericException, S("Modifying an already set match string is not allowed."));
    }
    this->matchString = str;
  }

  public: void setMatchString(TiWStr *str)
  {
    if (str == 0) {
      throw EXCEPTION(InvalidArgumentException, S("str"), S("Argument should not be null."), str);
    }
    if (this->matchString.getWStr().size() > 0) {
      throw EXCEPTION(GenericException, S("Modifying an already set match string is not allowed."));
    }
    this->matchString = *str;
  }

  /// Get the match string.
  public: WStr const& getMatchString() const
  {
    return this->matchString.getWStr();
  }

}; // class

} // namespace

#endif
