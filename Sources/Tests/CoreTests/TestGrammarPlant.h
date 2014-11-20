/**
 * @file Tests/CoreTests/TestGrammarPlant.h
 * Contains the header of class CoreTests::TestGrammarPlant.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef TEST_TESTGRAMMARPLANT_H
#define TEST_TESTGRAMMARPLANT_H

namespace Tests { namespace CoreTests
{

// TODO: DOC

/**
 * @brief The library of grammar used by unit tests.
 * @ingroup test
 *
 * This singleton class contains hardcoded definitions of char groups, tokens,
 * productions as well as heap data. This class does nothing other than creating
 * and returning those definitions.
 */
class TestGrammarPlant : public Data::GrammarPlant
{
  //============================================================================
  // Member Variables

  private: SharedPtr<Processing::GenericParsingHandler> parsingHandler;


  //============================================================================
  // Constructor

  public: TestGrammarPlant()
  {
    this->createGrammar();
  }

  public: virtual ~TestGrammarPlant()
  {
  }


  //============================================================================
  // Member Functions

  /// Create the entire core grammar.
  private: void createGrammar();

  /// Create the list of char group definitions for the Core's grammar.
  private: void createCharGroupDefinitions();

  /// Create the list of token definitions for the Core's grammar.
  private: void createTokenDefinitions();

  /// Create the list of produciton definitions for the Core's grammar.
  private: void createProductionDefinitions();

}; // class

} } // namespace

#endif
