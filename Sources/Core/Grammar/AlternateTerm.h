/**
 * @file Core/Grammar/AlternateTerm.h
 * Contains the header of class Core::Grammar::AlternateTerm.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_ALTERNATETERM_H
#define CORE_GRAMMAR_ALTERNATETERM_H

namespace Core::Grammar
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles alternate grammar terms.
 * @ingroup core_data_grammar
 *
 * This class handles alternatives in grammar formulas. This class holds two
 * (or more) paths of terms and indicates that either of these two paths is
 * accepted. Priority by default is for the paths with the lower index, but
 * it can be overriden.
 */
class AlternateTerm : public ListTerm
{
  //============================================================================
  // Type Info

  TYPE_INFO(AlternateTerm, ListTerm, "Core.Grammar", "Core", "alusus.org");
  OBJECT_FACTORY(AlternateTerm);


  //============================================================================
  // Types

  /// Used by the lexer to map a character to the indexes of the branches that may accept it.
  public: typedef std::unordered_map<WChar, std::vector<Int>> CharBasedDecisionCache;


  //============================================================================
  // Member Variables

  private: TextBasedDecisionCache textBasedDecisionCache;
  private: IdBasedDecisionCache idBasedDecisionCache;
  private: CharBasedDecisionCache charBasedDecisionCache;


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(AlternateTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(AlternateTerm);

  public: virtual ~AlternateTerm()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TermType getTermType() const
  {
    return TermType::ALTERNATE;
  }

  public: TextBasedDecisionCache* getInnerTextBasedDecisionCache()
  {
    return &this->textBasedDecisionCache;
  }

  public: IdBasedDecisionCache* getInnerIdBasedDecisionCache()
  {
    return &this->idBasedDecisionCache;
  }

  public: CharBasedDecisionCache* getCharBasedDecisionCache()
  {
    return &this->charBasedDecisionCache;
  }


  //============================================================================
  // CacheHaving Implementation

  /// @sa CacheHaving::clearCache()
  public: virtual void clearCache()
  {
    this->textBasedDecisionCache.clear();
    this->idBasedDecisionCache.clear();
    this->charBasedDecisionCache.clear();
  }

}; // class

} // namespace

#endif
