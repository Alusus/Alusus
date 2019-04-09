/**
 * @file Core/Data/Grammar/StandardFactory.h
 * Contains the header of class Core::Data::Grammar::StandardFactory.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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

/// A grammar factory for the Core's grammar.
class StandardFactory : public Factory
{
  //============================================================================
  // Member Variables

  private: SharedPtr<Processing::Handlers::ConstTokenizingHandler> constTokenHandler;

  private: SharedPtr<Processing::Handlers::StringLiteralTokenizingHandler> stringLiteralHandler;
  private: SharedPtr<Processing::Handlers::StringLiteralTokenizingHandler> charLiteralHandler;

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
  public: void createGrammar(DynamicContaining<TiObject> *rootScope, Main::RootManager *root, Bool exprOnly);

  /// Create char group definitions.
  private: void createCharGroupDefinitions();

  /// Create token definitions.
  private: void createTokenDefinitions();

  /// Create production definitions.
  private: void createProductionDefinitions(Bool exprOnly);

  /// Create token data used by productions.
  private: void createTokenDataModule();

  /// Create statement template production definitions.
  private: void createStatementsProductionModule();

  /// Create command template production definitions.
  private: void createCommandTemplateProductionDefinitions();

  /// Create Expression production module.
  private: void createExpressionProductionModule();

  /// Create Subject production module.
  private: void createSubjectProductionModule();

  /// Create Set production definitions.
  private: void createSetProductionDefinitions();

  /// Create modifier productions.
  private: void createModifierProductionDefinitions();

  /// Create Main production module.
  private: void createMainProductionModule(Bool exprOnly);

  /// Create a const token that uses the ConstTokenizingHandler.
  protected: virtual SharedPtr<SymbolDefinition> createConstTokenDef(Char const *text);

}; // class

} // namespace

#endif
