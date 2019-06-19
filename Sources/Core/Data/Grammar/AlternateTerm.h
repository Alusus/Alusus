/**
 * @file Core/Data/Grammar/AlternateTerm.h
 * Contains the header of class Core::Data::Grammar::AlternateTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_ALTERNATETERM_H
#define CORE_DATA_GRAMMAR_ALTERNATETERM_H

namespace Core::Data::Grammar
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

  TYPE_INFO(AlternateTerm, ListTerm, "Core.Data.Grammar", "Core", "alusus.org");
  OBJECT_FACTORY(AlternateTerm);


  //============================================================================
  // Types

  public: typedef std::unordered_map<Str, Int, std::hash<std::string>> TextBasedDecisionCache;
  public: typedef std::unordered_map<Word, Int> IdBasedDecisionCache;


  //============================================================================
  // Member Variables

  private: TextBasedDecisionCache textBasedDecisionCache;
  private: IdBasedDecisionCache idBasedDecisionCache;


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(AlternateTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(AlternateTerm, this->validate());

  public: virtual ~AlternateTerm()
  {
  }


  //============================================================================
  // Member Functions

  public: TextBasedDecisionCache* getInnerTextBasedDecisionCache()
  {
    return &this->textBasedDecisionCache;
  }

  public: IdBasedDecisionCache* getInnerIdBasedDecisionCache()
  {
    return &this->idBasedDecisionCache;
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    this->textBasedDecisionCache.clear();
    this->idBasedDecisionCache.clear();
  }

}; // class

} // namespace

#endif
