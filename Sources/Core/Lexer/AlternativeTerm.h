/**
 * @file Core/Lexer/AlternativeTerm.h
 * Contains the header of class Core::Lexer::AlternativeTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_ALTERNATIVE_TERM_H
#define LEXER_ALTERNATIVE_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles alternative grammar terms.
 * @ingroup lexer_terms
 *
 * This class handles alternatives in grammar formulas. This class holds two
 * paths of terms and indicates that either of these two paths is accepted.
 */
class AlternativeTerm : public ListTerm
{
  //============================================================================
  // Constructors

  public: AlternativeTerm()
  {
  }

  public: AlternativeTerm(Int count, ...);


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::ALTERNATIVE;
  }

}; // class

} } // namespace

#endif
