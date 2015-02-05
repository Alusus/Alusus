/**
 * @file Core/Standard/GrammarPlant.h
 * Contains the header of class Core::Standard::GrammarPlant.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_GRAMMAR_PLANT_H
#define STANDARD_GRAMMAR_PLANT_H

namespace Core { namespace Standard
{

// TODO: DOC

/**
 * @brief The library of default core grammar.
 * @ingroup standard
 *
 * This singleton class contains hardcoded definitions of char groups, tokens,
 * productions as well as heap data. This class does nothing other than creating
 * and returning those definitions.
 */
class GrammarPlant : public Data::GrammarPlant
{
  //============================================================================
  // Member Variables

  private: SharedPtr<StringLiteralTokenizingHandler> stringLiteralHandler;

  private: SharedPtr<Processing::RootParsingHandler> rootHandler;

  private: SharedPtr<Processing::GenericParsingHandler> parsingHandler;

  private: SharedPtr<ImportParsingHandler> importHandler;


  //============================================================================
  // Constructor & Destructor

  public: GrammarPlant(RootManager *root)
  {
    this->createGrammar(root);
  }

  public: virtual ~GrammarPlant()
  {
  }


  //============================================================================
  // Member Functions

  /// Create the entire core grammar.
  private: void createGrammar(RootManager *root);

  /// Create the list of char group definitions for the Core's grammar.
  private: void createCharGroupDefinitions();

  /// Create the list of token definitions for the Core's grammar.
  private: void createTokenDefinitions();

  /// Create the list of produciton definitions for the Core's grammar.
  private: void createProductionDefinitions();

}; // class

} } // namespace

#endif
