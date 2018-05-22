/**
 * @file Core/Data/Grammar/StandardFactory.h
 * Contains the header of class Core::Data::Grammar::StandardFactory.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_STANDARDFACTORY_H
#define CORE_DATA_GRAMMAR_STANDARDFACTORY_H

namespace Core::Data::Grammar
{

class StandardFactory : public Factory
{
  //============================================================================
  // Member Variables

  private: SharedPtr<Processing::Handlers::ConstTokenizingHandler> constTokenHandler;

  private: SharedPtr<Processing::Handlers::StringLiteralTokenizingHandler> stringLiteralHandler;

  private: SharedPtr<Processing::Handlers::GenericParsingHandler> parsingHandler;

  private: SharedPtr<Processing::Handlers::ImportParsingHandler> importHandler;

  private: SharedPtr<Processing::Handlers::DumpAstParsingHandler> dumpAstParsingHandler;

  private: SharedPtr<Processing::Handlers::ModifierParsingHandler> leadingModifierHandler;
  private: SharedPtr<Processing::Handlers::ModifierParsingHandler> trailingModifierHandler;

  private: SharedPtr<Processing::Handlers::GenericCommandParsingHandler> doCommandParsingHandler;


  //============================================================================
  // Constructor & Destructor

  public: StandardFactory()
  {
  }

  public: virtual ~StandardFactory()
  {
  }


  //============================================================================
  // Member Functions

  /// Create the entire core grammar.
  public: void createGrammar(Basic::ListContaining<TiObject> *rootScope, Standard::RootManager *root, Bool exprOnly);

  /// Create the list of char group definitions for the Core's grammar.
  private: void createCharGroupDefinitions();

  /// Create the list of token definitions for the Core's grammar.
  private: void createTokenDefinitions();

  /// Create the list of produciton definitions for the Core's grammar.
  private: void createProductionDefinitions(Bool exprOnly);

  /// Create a const token that uses the ConstTokenizingHandler.
  protected: virtual SharedPtr<Data::Grammar::SymbolDefinition> createConstTokenDef(Char const *text);

}; // class

} // namespace

#endif
