/**
 * @file Tests/CoreTests/Lexer_tests.cpp
 * Contain's the Core::Lexer namespace's unit tests.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core_tests.h"

namespace Tests { namespace CoreTests
{

// Test for a successful tokenization of a simple source.
TEST_CASE("Core::Lexer/successful", "Successful Tokenization Test")
{
  // Prepare the grammar.
  TestGrammarPlant store;
  // Prepare the lexer.
  Lexer::StateMachine lexer;
  lexer.setCharGroupDefinitions(store.getCharGroupDefinitions());
  lexer.setTokenDefinitions(store.getTokenDefinitions());
  // Prepare the tester.
  LexerTester tester(&lexer);

  try {
    SECTION("s1", "Simple expression / string assignment")
    {
      tester.reset();
      tester.addToken(store.IDENTIFIER_TOKEN, STR("strVar"));
      tester.addToken(UNKNOWN_ID, STR("="));
      tester.addToken(store.STRING_LITERAL_TOKEN, STR("\"Hello World\""));
      tester.addToken(UNKNOWN_ID, STR(";"));
      tester.test(STR("strVar = \"Hello World\";"));
    }
    SECTION("s2", "Simple expression / int assignment")
    {
      tester.reset();
      tester.addToken(store.IDENTIFIER_TOKEN, STR("strVar"));
      tester.addToken(UNKNOWN_ID, STR("="));
      tester.addToken(store.INT_LITERAL_TOKEN, STR("123"));
      tester.addToken(UNKNOWN_ID, STR(";"));
      tester.test(STR("strVar = 123;"));
    }
  } catch (Exception &e) {
    FAIL(e.getErrorMessage());
  }
}

} } // namespace
