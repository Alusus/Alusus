/**
 * @file Core/Lexer/ConcatTerm.h
 * Contains the header of class Core::Lexer::ConcatTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_CONCAT_TERM_H
#define LEXER_CONCAT_TERM_H

namespace Core { namespace Lexer
{

/**
 * @brief Handles grammar terms concatenation.
 * @ingroup lexer_terms
 *
 * This class handles concatenation in grammar formulas. This class holds an
 * indefinite number of terms that should come in sequence.
 */
class ConcatTerm : public ListTerm
{
  //============================================================================
  // Constructors / Destructor

  public: ConcatTerm()
  {
  }

  public: ConcatTerm(Int count, ...);


  //============================================================================
  // Member Functions

  /// Get the type identifier of this object.
  public: virtual TermType getType() const
  {
    return TermType::CONCAT;
  }

}; // class

} } // namespace

#endif
