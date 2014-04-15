/**
 * @file Core/Lexer/ConstTerm.h
 * Contains the header of class Core::Lexer::ConstTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CONST_TERM_H
#define LEXER_CONST_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles constant grammar terms.
 * @ingroup lexer_terms
 *
 * This class handles constant terms in grammar formulas. Constant terms
 * are used to check for exact matches of strings.
 */
class ConstTerm : public Term
{
  //============================================================================
  // Member Variables

  /// A pointer to the string constant to be matched with input characters.
  private: Str matchString;


  //============================================================================
  // Constructor / Destructor

  public: ConstTerm()
  {
  }

  public: ConstTerm(const Char *str)
  {
    if (str == 0) {
      throw InvalidArgumentException(STR("str"), STR("Core::Lexer::ConstTerm::ConstTerm"));
    }
    this->matchString = str;
  }

  public: virtual ~ConstTerm()
  {
  }


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::CONST;
  }

  /**
     * @brief Set the match string.
     *
     * Once a match string is set, modifying (or setting) this value is not
     * allowed. This is to prevent accidentally modifying it while being in use
     * by some state.
     */
  public: void setMatchString(const Char *str)
  {
    if (str == 0) {
      throw InvalidArgumentException(STR("str"), STR("Core::Lexer::ConstTerm::setMatchString"));
    }
    if (this->matchString.size() > 0) {
      throw GeneralException(STR("Modifying an already set match string is not allowed."),
                             STR("Core::Lexer::ConstTerm::setMatchString"));
    }
    this->matchString= str;
  }

  /// Get the match string.
  public: const Str& getMatchString() const
  {
    return this->matchString;
  }

}; // class

} } // namespace

#endif
