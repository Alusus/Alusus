/**
 * @file Core/Data/Grammar/LexerModule.h
 * Contains the header of class Core::Data::Grammar::LexerModule.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_LEXERMODULE_H
#define CORE_DATA_GRAMMAR_LEXERMODULE_H

#include <unordered_map>

namespace Core::Data::Grammar
{

class LexerModule : public Module, public CacheHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(LexerModule, Module, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(CacheHaving)
  ));
  OBJECT_FACTORY(LexerModule);


  //============================================================================
  // Types

  public: typedef std::unordered_map<WChar, std::vector<Int>> CharBasedDecisionCache;


  //============================================================================
  // Member Variables

  private: CharBasedDecisionCache charBasedDecisionCache;


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(LexerModule);

  IMPLEMENT_ATTR_CONSTRUCTOR(LexerModule);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(LexerModule);

  public: LexerModule(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) : Module(attrs, elements, useIndex)
  {
  }

  public: virtual ~LexerModule()
  {
  }

  public: static SharedPtr<LexerModule> create(
    std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements, Bool useIndex
  ) {
    return newSrdObj<LexerModule>(attrs, elements, useIndex);
  }


  //============================================================================
  // Member Functions

  public: CharBasedDecisionCache* getCharBasedDecisionCache()
  {
    return &this->charBasedDecisionCache;
  }


  //============================================================================
  // CacheHaving Implementation

  /// @sa CacheHaving::clearCache()
  public: virtual void clearCache()
  {
    this->charBasedDecisionCache.clear();
  }

}; // class

} // namespace

#endif
