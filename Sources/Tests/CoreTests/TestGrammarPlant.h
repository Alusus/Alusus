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

#ifndef TEST_TEST_GRAMMAR_PLANT_H
#define TEST_TEST_GRAMMAR_PLANT_H

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

  // TODO: Once the lexer is converted to use the Grammar data structures, remove all these member variables and keep
  //       only repository.

  /// @name Char Groups Definition IDs
  /// @{

  public: const Int BIN_DIGIT_CHAR_GROUP;
  public: const Int OCT_DIGIT_CHAR_GROUP;
  public: const Int DEC_DIGIT_CHAR_GROUP;
  public: const Int HEX_DIGIT_CHAR_GROUP;
  public: const Int LETTER_CHAR_GROUP;
  public: const Int ANY_CHAR_NO_ES_CHAR_GROUP;
  public: const Int ANY_CHAR_NO_ES_OR_SINGLE_QUOTE_CHAR_GROUP;
  public: const Int ANY_CHAR_NO_ES_OR_DOUBLE_QUOTE_CHAR_GROUP;
  public: const Int SPACING_CHAR_GROUP;

  /// @}

  /**
   * @name Token Definition IDs
   *
   * Ignored tokens (tokens used for spaces and comments) has an id equal or
   * greater than IGNORED_TOKEN_BASE.
   *
   * @{
   */

  public: const Int IDENTIFIER_TOKEN;
  public: const Int INT_LITERAL_TOKEN;
  public: const Int _DEC_INT_LITERAL_TOKEN;
  public: const Int _BIN_INT_LITERAL_TOKEN;
  public: const Int _OCT_INT_LITERAL_TOKEN;
  public: const Int _HEX_INT_LITERAL_TOKEN;
  public: const Int FLOAT_LITERAL_TOKEN;
  public: const Int _FLOAT_EXPONENT_TOKEN;
  public: const Int _FLOAT_POSTFIX_TOKEN;
  public: const Int CHAR_LITERAL_TOKEN;
  public: const Int STRING_LITERAL_TOKEN;
  public: const Int _STRING_LITERAL_PART_TOKEN;
  public: const Int _CHAR_CODE_POSTFIX_TOKEN;
  public: const Int _ES_CHAR_WITH_SINGLE_QUOTE_TOKEN;
  public: const Int _ES_CHAR_WITH_DOUBLE_QUOTE_TOKEN;
  public: const Int _ES_CHAR_WITH_QUOTES_TOKEN;
  public: const Int _ES_SEQUENCE_TOKEN;
  public: const Int CUSTOM_LITERAL_TOKEN;

  public: const Int SPACES_TOKEN;
  public: const Int LINE_COMMENT_TOKEN;
  public: const Int BLOCK_COMMENT_TOKEN;

  /// @}

  /// @name Grammar Objects
  /// @{

  private: SharedPtr<Lexer::CharGroupDefinitionList> charGroupDefinitions;

  private: SharedPtr<Lexer::TokenDefinitionList> tokenDefinitions;

  private: SharedPtr<Standard::GenericParsingHandler> parsingHandler;

  /// @}


  //============================================================================
  // Constructor

  public: TestGrammarPlant();

  public: virtual ~TestGrammarPlant()
  {
  }


  //============================================================================
  // Member Functions

  /// Create the list of char group definitions for the Core's grammar.
  protected: virtual void createCharGroupDefinitions();

  /// Create the list of token definitions for the Core's grammar.
  protected: virtual void createTokenDefinitions();

  /// Create the list of produciton definitions for the Core's grammar.
  protected: virtual void createProductionDefinitions();

  protected: virtual Word addConstToken(Char const *text)
  {
    return this->tokenDefinitions->addConstToken(text);
  }

  public: const SharedPtr<Lexer::CharGroupDefinitionList>& getCharGroupDefinitions()
  {
    return this->charGroupDefinitions;
  }

  public: const SharedPtr<Lexer::TokenDefinitionList>& getTokenDefinitions()
  {
    return this->tokenDefinitions;
  }

}; // class

} } // namespace

#endif
